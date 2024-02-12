import numpy.random as rnd
from numpy.linalg import qr as np_qr
from time import perf_counter_ns

def mult_range(start, end, step):
    i = start
    while i < end:
        yield i
        i *= step

def nonsqr():
    speeds = []
    for i in mult_range(2, 64,2):
        for j in mult_range(i, 1024, 2):
            #print(f"Running {j,i}")
            A = rnd.standard_normal((i, j))
            start = perf_counter_ns()
            for k in range(1000):
                res = np_qr(A)
            speeds.append((perf_counter_ns() - start)/1000)
    # multiply converts back to nanoseconds
    print([speed*1e-9 for speed in speeds])

nonsqr()