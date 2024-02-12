# What Python programmers do not get

This project, named to be slightly inflammatory in the programming language flame wars, 
is an example of speeding a program up purely by optimizing the memory layout. This features 
two versions of otherwise identical code, with algorithms with identical big **O** complexity.

Arguably, the faster algorithm even has a slightly worse constant; what we are comparing here are two 
implementations of QR decomposition. This is based on [Givens rotations](https://en.wikipedia.org/wiki/Givens_rotation), and both versions are logically 
equivalent; they both perform rotations in the same way. There is no evil witchery. 

Both implementations use the same implementation of Given's rotations, and both replace full matrix multiplies
by very similar hand-crafted block matrix multiplications. 

**There are no tricks. The only difference is the memory layout.**

Both algorithms take an input matrix to decompose as an array of numbers (to be precise,
a std::vector). Given that 2d objects are just a convenient
fiction (in programming), what we need to decide is how exactly we will lay out the entries in the matrix in memory:

* **column wise**, also known as column major
* or **row wise**, also known as row major

Suppose we have a matrix:

| 1 | 2 |
|-|-|
| 3 | 4 |

In the first case, our 1d array is equal to [1, 3, 2, 4]. In the second, it is [1, 2, 3, 4].

As I show, **this has significant performance implications.** 

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

I run three benchmarks;

* **Non-Square** matrices of size n >= p (General tall matrix) - what I deem the most common use case
* **Square** (n == p, where n is a multiple of 2) - the second most common use case; you would have this 
  if you were implementing the [QR algorithm](https://en.wikipedia.org/wiki/QR_algorithm)
* **Unaligned Square** (n == p, where n is a multiple of 2 offset by 1) - purely to show what impact
  some small amount of cache misalignment can have

across four different optimization levels:

* **unoptimized** (-O0) 
  - as a baseline, disabling all possible optimization; this is the only case where we observe both 
  versions to be near equivalent; in this case the compiler likely does not take full advantage of 
  the much better cache locality of the second version
* **optimized** (-03)
  - as what is more realistic to observe in production systems
* **optimized natively** (-03 -march=-native) 
  - to see whether optimizing to the CPU architecture 
  actually leads to a significant difference (in principle this should improve things like auto vectorization)
* **optimized with vectorisation disabled** 
  (-03 -fno-slp-vectorize -fno-tree-vectorize) 
  - since the second version is much easier
  optimize via loop unrolling (i.e. loops can actually be unrolled and vectorised) and I want to
  discuss primarily the cache benefits, this shows that we still get large benefits out of making 
  cache happy 

### Running the benchmark 

You can easily build the whole suite via make:
```bash
make all
```
And run it via:
```bash
make run_all
```
This will dump the results into subdirectory 'benchmarks'. 

### The results

After running the previously mentioned tests, we get the following results. I show median as well as average speedups since averages can be 
deceiving (i.e. if you had mostly the same performance and then a few large outliers): 

| Test                   | Average speedup | Median speedup | Total time | Worst speedup | Best speedup | Average speed v1 | Average speed v2 |
|------------------------| --------------: | -------------: | ---------: | ------------: | -----------: | ---------------: | --------------: |
| optimized_sqr.csv      | 14.95           | 12.28          | 45,798.3   | 0.43          | 33.02        | 1,060,330        | 45,663           |
| unvec_unal.csv         | 1.55            | 1.51           | 40,392.2   | 0.02          | 18.68        | 11,625.1         | 5,736.51         |
| optimizedv_nonsqr.csv  | 23.25           | 16.62          | 210,265    | 0.02          | 34.09        | 82,779.1         | 2,573.85         |
| unvec_nonsqr.csv       | 7.39            | 6.02           | 88,387.5   | 0.14          | 23.9         | 64,341.3         | 5,834.42         |
| optimizedv_sqr.csv     | 26.84           | 21.07          | 79,301.9   | 0.15          | 42.56        | 1,309,850        | 31,978.4         |
| optimizedv_unal.csv    | 3.69            | 2.93           | 69,509.7   | 0.32          | 45.23        | 11,523           | 2,352.79         |
| optimized_unal.csv     | 2.50            | 2.19           | 53,312.5   | 0.42          | 9.09         | 12,059           | 3,606.53         |
| unoptimized_unal.csv   | 1.07            | 1.00           | 35,109.9   | 0.18          | 3.29         | 89,008.4         | 82,174.8         |
| optimized_nonsqr.csv   | 12.41           | 9.59           | 130,495    | 0.07          | 19.36        | 67,966.5         | 3,809.31         |
| unoptimized_sqr.csv    | 1.00            | 0.99           | 8,992.35   | 0.34          | 3.14         | 1,182,540        | 1,158,360        |
| unoptimized_nonsqr.csv | 1.00            | 0.99           | 34,637.2   | 0.05          | 6.19         | 76,459.4         | 75,971.5         |
| unvec_sqr.csv          | 8.60            | 7.37           | 28,020.9   | 0.04          | 18.02        | 1,014,400        | 75,699.5         |

Under the unoptimized setting, we see what we would expect; that doing 'more work' equates to longer runtimes; 
transposing the matrix has a small but non-zero cost, and so our second version is seemingly a bit slower. 

In every other case, i.e. if you do not forget to turn on optimization, the second version is drastically better. 
Under every optimized setting, we see at least a 50% speedup; for the unaligned, unvectorised case, as should be expected. 
The best case is clearly the optimized + vectorised case with square matrices, but even aside from that, 
we see very healthy speedups across the board. We see that even in the unaligned cases, with optimization
we are looking at a 2x speedup. Vectorization easily makes this 3x.

**But for aligned data, we are easily over 10x better.** 

Keep in mind that all we did was 'more work' and a change of data layout. There was no manual vectorization, 
no work to make sure the data are aligned (to fit neatly into vector registers), etc. These things would likely improve 
performance even further. 

### Okay, this is cool, but why the name?

I named this repository the way it is for two reasons. Reason number one is, obviously, attention; it is easier to grab some 
if people are getting at least a bit upset. 

The other less silly reason is this; I keep hearing, quite often, from people
who work in Python / Javascript / **insert your favourite high level language** about how you do not need to think about 
performance. 

How you will only gain a bit of performance by switching over to a different language / framework / ... (usually whatever the speaker deems 
an acceptable 'loss of performance', quite often a figure like 10-20%).

How you do not need to worry about 'low level'
details of your program, as they cannot possibly beat big **O** complexity and other concerns, and besides, 
they are surely not worth the added code complexity.

**This repo shows why I consider those arguments misguided.**

I have added minimal code complexity, if any. I have not written thousands of lines of code, or fine tuned something in assembly. 
I changed the data layout, in a fairly intuitive way. Big **O** complexity is important. But so are data layouts, and if you work in a language where you cannot control them, 
you might get lucky, or you might get unlucky. It sucks to get unlucky.

At the end of the day, we all need to think of data layouts if we want performance, regardless of language.

#### Additional reading/talks

I was originally inspired by numerous talks to investigate this topic. 
I strongly recommend watching:

[Mike Acton's legendary talk at CppCon 2014](https://www.youtube.com/watch?v=rX0ItVEVjHc&t=51s)

[A follow-up on Data Oriented Programming](https://www.youtube.com/watch?v=yy8jQgmhbAU)

[A very good talk by Andrei Alexandrescu from CppCon 2019](https://www.youtube.com/watch?v=FJJTYQYB1JQ)

[A really nice hash table implementation](https://www.youtube.com/watch?v=DMQ_HcNSOAI&t=1s)

[Explanation of how much performance can be lost via virtual calls](https://www.youtube.com/watch?v=tD5NrevFtbU&t=1023s)

[Why performance matters](https://www.youtube.com/watch?v=x2EOOJg8FkA&t=203s)




