# What some programmers do not get

This project, named to be slightly inflammatory in the programming language flame wars, 
is an example of speeding a project up purely by optimizing the memory layout. This features 
two versions of otherwise identical code, with algorithms with identical big $O$ complexity.

Arguably, the faster algorithm even has a slightly worse constant; what we are comparing here two 
implementations a QR decomposition. This is based on [Givens rotations](), and both versions are logically 
equivalent; they both perform rotations in the same way. 

**The only difference is the memory layout.**

Both algorithms take the input matrix as an array of numbers. Given that 2d objects are just a convenient
fiction, what we need to decide is how exactly we will lay the values in the matrix out in memory:

* column wise, also known as column major
* or row wise, also known as row major

Suppose we have a matrix:
$\begin{bmatrix}1 & 2\\3 & 4\end{bmatrix}$

In the first case, our 1d array is equal to [1, 3, 2, 4]. In the second, it is [1, 2, 3, 4].

As I will show, **this has significant performance implications.** 

### The implementation

Both algorithms take, as an input, a matrix **X** in column major format. This is crucial for the 
rest of the algorithm. Read on to understand why. 

The first implementation implements the algorithm naively; you pass an input matrix of size **n x p**
(rows x columns) and this is processed, where rotations are applied, row by row. 

To shed some light on this, some simple pseudocode of what we are trying to accomplish
(actual implementation is slightly harder to read, go to the tinyqr.h header file if you would 
like to see it):

```pseudocode
function QR inputs: matrix X, integer columns, integer rows 
  initialize R of size rows x columns with X, 
  inialize Q of size rows x rows 

  for j in columns:
    for i in reverse(rows-1): // reverse means loop runs in reverse, i.e. down from rows, rather than up to rows
      c,s = get_givens_rotation(R[j, i-1], R[j, i])
      // rotation is just a multiplication with a 2x2 block
      for k in columns:
        tmp1 = R[k, i-1]
        tmp2 = R[k, i]
        R[k, i-1] = c * temp_1 + s * temp_2;
        R[k, i] = -s * temp_1 + c * temp_2;
    // analogously for matrix Q; not shown for sake of brevity
    //...
return Q, R  
```

The second implementation is almost identical, but it transposes the (column major) input matrix 
when initializing R:
```pseudocode
function QR inputs: matrix X, integer columns, integer rows 
  initialize R of size columns x rows with t(X), 
  inialize Q of size rows x rows 
```
and correspondingly changes all indices to match:
```
for j in columns:
  for i in reverse(rows-1): // reverse means loop runs in reverse, i.e. down from rows, rather than up to rows
    c,s = get_givens_rotation(R[i-1, j], R[i, j])
    for k in columns:
      tmp1 = R[i, k]
      tmp2 = R[i-1, k]
      R[i, k] = c * temp_1 + s * temp_2;
      R[i-1, k] = -s * temp_1 + c * temp_2;
      //...
return Q, R
```







### The benchmark





### The conclusion


#### Additional reading/talks


