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

#include <iostream>
#include "./tinyqr.h"
#include "utils.h"

int main() {
  using scalar_t = double;
  using tinyqr::QRImpl;
  using tinyqr::qr_decomposition;
  // you can validate easily using tinyqr::validate_qr - this is not done here
  // since I already did that elsewhere
  [&](){
    std::vector<double> speeds;
    for (size_t i = 2; i < 64; i *= 2) {
      for (size_t j = i; j < 1024; j *= 2) {
        //std::cout << "n: " << j << " | p: " << i << std::endl;
        auto X = make_random_matrix<scalar_t>(j, i);
        Stopwatch<false, std::chrono::duration<double, std::nano>> sw;
        for(size_t k =0; k < 1000; k++) {
          auto res = qr_decomposition<scalar_t, QRImpl::transpose>(X, j, i);
        }
        speeds.push_back(sw()/1000);
      }
    }
    for(auto &val:speeds) std::cout<< val *1e-9<<",";
  }();
  return 0;
}