// Tiny QR solver, header only library
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
//
// Copyright (C) 2023- Juraj Szitas
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef TINYQR_H_
#define TINYQR_H_

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <tuple>
#include <utility>
#include <vector>

namespace tinyqr {
template <typename scalar_t>
std::tuple<scalar_t, scalar_t> givens_rotation(scalar_t a, scalar_t b) {
  if (std::abs(b) > std::abs(a)) {
    const scalar_t r = a / b;
    const scalar_t s = 1.0 / sqrt(1.0 + std::pow(r, 2));
    return std::make_tuple(s * r, s);
  } else {
    const scalar_t r = b / a;
    const scalar_t c = 1.0 / sqrt(1.0 + std::pow(r, 2));
    return std::make_tuple(c, c * r);
  }
}
// transpose a square matrix in place
template <typename scalar_t>
void transpose_square(std::vector<scalar_t> &X, const size_t p) {
  for (size_t i = 0; i < p; i++) {
    for (size_t j = i + 1; j < p; j++) {
      std::swap(X[(j * p) + i], X[(i * p) + j]);
    }
  }
}
template <typename scalar_t>
std::vector<scalar_t> make_identity(const size_t n) {
  std::vector<scalar_t> result(n * n, 0.0);
  for (size_t i = 0; i < n; i++) result[i * n + i] = 1.0;
  return result;
}
template <typename scalar_t>
[[maybe_unused]] void validate_qr(const std::vector<scalar_t> &X,
                                  const std::vector<scalar_t> &Q,
                                  const std::vector<scalar_t> &R,
                                  const size_t n, const size_t p) {
  // constant factor here added since epsilon is too small otherwise
  constexpr scalar_t eps = std::numeric_limits<scalar_t>::epsilon() * 1e4;
  // Matrix multiplication QR
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < p; ++j) {
      scalar_t tmp = 0;
      for (size_t k = 0; k < p; ++k) {
        tmp += Q[k * n + i] * R[j * p + k];
      }
      // Compare to original matrix X
      if (std::abs(X[j * n + i] - tmp) > eps) {
        std::cout << "Error in {validate_qr}, " << tmp << " != " << X[i * p + j]
                  << " diff: " << std::abs(X[j * n + i] - tmp)
                  << " eps: " << eps << "\n";
        std::cout << "Failed to recreate input from QR matrices for size " << n
                  << ", " << p << "\n";
        return;
      }
    }
  }
  std::cout << "Validation of QR successful for size " << n << ", " << p
            << std::endl;
}

// this is the implementation of QR decomposition - this does not get exposed,
// only the nice(r) facades do - NOTE that the output of this is technically
// Q transposed, not Q!!!
template <typename scalar_t, const bool cleanup = false>
void qr_impl(std::vector<scalar_t> &Q, std::vector<scalar_t> &R, const size_t n,
             const size_t p, const scalar_t tol) {
  // the key to optimizing this is probably to take R as R transposed - most
  // likely a lot of work is done just in the k loops, which is probably a good
  // place to optimize
  for (size_t j = 0; j < p; j++) {
    for (size_t i = n - 1; i > j; --i) {
      // using tuples and structured bindings should make this fairly ok
      // performance wise
      // check if R[j * n + i] - is not zero; if it is we can skip this
      // iteration
      if (std::abs(R[j * n + i]) <= std::numeric_limits<scalar_t>::min())
        continue;
      const auto [c, s] = givens_rotation(R[(j * n) + (i - 1)], R[j * n + i]);
      // you can make the matrix multiplication implicit, as the givens rotation
      // only impacts a moving 2x2 block
      for (size_t k = 0; k < p; ++k) {
        // first do G'R - keep in mind this is transposed
        const size_t upper = k * n + (i - 1);
        const size_t lower = k * n + i;
        const scalar_t temp_1 = R[upper];
        const scalar_t temp_2 = R[lower];
        // carry out the multiplies on required elements
        R[upper] = c * temp_1 + s * temp_2;
        R[lower] = -s * temp_1 + c * temp_2;
      }
      for (size_t k = 0; k < n; k++) {
        // QG - note that this is not transposed
        const size_t upper = k * n + i;
        const size_t lower = k * n + i - 1;
        const scalar_t temp_1 = Q[upper];
        const scalar_t temp_2 = Q[lower];
        // again, NOT transposed, so s and -s are flipped
        Q[upper] = c * temp_1 - s * temp_2;
        Q[lower] = s * temp_1 + c * temp_2;
      }
    }
  }
  // clean up R - particularly under the diagonal - only useful if you are
  // interested in the actual decomposition
  if constexpr (cleanup) {
    for (auto &val : R) {
      val = std::abs(val) < tol ? 0.0 : val;
    }
  }
}
template <typename scalar_t, const bool cleanup = false>
void qr_impl2(std::vector<scalar_t> &Q, std::vector<scalar_t> &R,
              const size_t n, const size_t p, const scalar_t tol) {
  // the key to optimizing this is probably to take R as R transposed - most
  // likely a lot of work is done just in the k loops, which is probably a good
  // place to optimize
  for (size_t j = 0; j < p; j++) {
    for (size_t i = n - 1; i > j; --i) {
      // using tuples and structured bindings should make this fairly ok
      // performance wise
      // check if R[j * n + i] - is not zero; if it is we can skip this
      // iteration
      if (std::abs(R[i * p + j]) <= std::numeric_limits<scalar_t>::min())
        continue;
      const auto [c, s] = givens_rotation(R[(i - 1) * p + j], R[i * p + j]);
      // you can make the matrix multiplication implicit, as the givens rotation
      // only impacts a moving 2x2 block
      // R is transposed
      for (size_t k = 0; k < p; ++k) {
        // first do G'R - keep in mind this is transposed
        const size_t upper = i * p + k;
        const size_t lower = (i - 1) * p + k;
        const scalar_t temp_1 = R[lower];
        const scalar_t temp_2 = R[upper];
        // carry out the multiplies on required elements
        R[lower] = c * temp_1 + s * temp_2;
        R[upper] = -s * temp_1 + c * temp_2;
      }
      for (size_t k = 0; k < n; k++) {
        // QG - note that this is transposed
        const size_t upper = i * n + k;
        const size_t lower = (i - 1) * n + k;
        const scalar_t temp_1 = Q[upper];
        const scalar_t temp_2 = Q[lower];
        // again, compared to the R loop, transposed, so s and -s are flipped
        Q[upper] = c * temp_1 - s * temp_2;
        Q[lower] = s * temp_1 + c * temp_2;
      }
    }
  }
  // clean up R - particularly under the diagonal - only useful if you are
  // interested in the actual decomposition
  if constexpr (cleanup) {
    for (auto &val : R) {
      val = std::abs(val) < tol ? 0.0 : val;
    }
  }
}
enum QRImpl{
  baseline,
  transpose
};
template <typename scalar_t>
struct QR {
  std::vector<scalar_t> Q;
  std::vector<scalar_t> R;
};
template <typename scalar_t, const QRImpl qr_impl = QRImpl::baseline>
[[maybe_unused]] QR<scalar_t> qr_decomposition(const std::vector<scalar_t> &X,
                                               const size_t n, const size_t p,
                                               const scalar_t tol = 1e-8) {
  // initialize Q and R
  std::vector<scalar_t> Q = tinyqr::make_identity<scalar_t>(n);
  std::vector<scalar_t> R(X.size(), 0.0);
  if constexpr(qr_impl == QRImpl::baseline) {
    std::vector<scalar_t> R = X;
    tinyqr::qr_impl<scalar_t, true>(Q, R, n, p, tol);
  }
  else if constexpr(qr_impl == QRImpl::transpose) {
    for (size_t i = 0; i < n; i++) {
      for (size_t j = 0; j < p; j++) {
        R[i * p + j] = X[j * n + i];
      }
    }
    tinyqr::qr_impl2<scalar_t, true>(Q, R, n, p, tol);
  }
  // keep in mind that only an n*p block of Q is meaningful, as well as only a
  // p*p block of R
  tinyqr::transpose_square(Q, n);
  size_t k = 0;
  for (size_t i = 0; i < p; i++) {
    for (size_t j = 0; j < p; j++) {
      R[k++] = R[i * n + j];
    }
  }
  Q.resize(n * p);
  R.resize(p * p);
  return {Q, R};
}
}  // namespace tinyqr
#endif  // TINYQR_H_"
