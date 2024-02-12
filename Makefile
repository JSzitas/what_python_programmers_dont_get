all: unoptimized optimized optimizedv unoptimized_sqr optimized_sqr optimizedv_sqr unoptimized_unal optimized_unal optimizedv_unal

unoptimized:
	clang++ -std=c++17 -O0 -g -Wall nonsqr.cpp -o unoptimized_nonsqr
optimized:
	clang++ -std=c++17 -O3 -Wall nonsqr.cpp -o optimized_nonsqr
optimizedv:
	clang++ -std=c++17 -O3 -march=native -Wall nonsqr.cpp -o optimizedv_nonsqr

unoptimized_sqr:
	clang++ -std=c++17 -O0 -g -Wall sqr.cpp -o unoptimized_sqr
optimized_sqr:
	clang++ -std=c++17 -O3 -Wall sqr.cpp -o optimized_sqr
optimizedv_sqr:
	clang++ -std=c++17 -O3 -march=native -Wall sqr.cpp -o optimizedv_sqr

unoptimized_unal:
	clang++ -std=c++17 -O0 -g -Wall unal.cpp -o unoptimized_unal
optimized_unal:
	clang++ -std=c++17 -O3 -Wall unal.cpp -o optimized_unal
optimizedv_unal:
	clang++ -std=c++17 -O3 -march=native -Wall unal.cpp -o optimizedv_unal

unvectorised:
	clang++ -std=c++17 -O3 -fno-slp-vectorize -fno-tree-vectorize -Wall nonsqr.cpp -o unvec_nonsqr
unvectorised_sqr:
	clang++ -std=c++17 -O3 -fno-slp-vectorize -fno-tree-vectorize -Wall sqr.cpp -o unvec_sqr
unvectorised_unal:
	clang++ -std=c++17 -O3 -fno-slp-vectorize -fno-tree-vectorize -Wall unal.cpp -o unvec_unal

clean:
	rm -rf unoptimized_nonsqr optimized_nonsqr optimizedv_nonsqr unoptimized_sqr optimized_sqr optimizedv_sqr unoptimized_unal optimized_unal optimizedv_unal unvectorised unvectorised_sqr unvectorised_unal

collect:
	clang++ -std=c++17 -O2 render.cpp -o render; ./render benchmarks benchmark_aggregate.csv; rm render

quick_bench:
	clang++ -std=c++17 -O3 -march=native -Wall quick.cpp -o quick;
	./quick;
	rm quick;

run_all:
	make all;
	./unoptimized_nonsqr benchmarks/unoptimized_nonsqr.csv;
	./optimized_nonsqr benchmarks/optimized_nonsqr.csv;
	./optimizedv_nonsqr benchmarks/optimizedv_nonsqr.csv;
	./unoptimized_sqr benchmarks/unoptimized_sqr.csv;
	./optimized_sqr benchmarks/optimized_sqr.csv;
	./optimizedv_sqr benchmarks/optimizedv_sqr.csv;
	./unoptimized_unal benchmarks/unoptimized_unal.csv;
	./optimized_unal benchmarks/optimized_unal.csv;
	./optimizedv_unal benchmarks/optimizedv_unal.csv;
	./unvec_nonsqr benchmarks/unvec_nonsqr.csv;
	./unvec_sqr benchmarks/unvec_sqr.csv;
	./unvec_unal benchmarks/unvec_unal.csv;
	make collect;
	make clean

run_optimized:
	make all;
	./optimized_nonsqr benchmarks/optimized_nonsqr.csv;
	./optimizedv_nonsqr benchmarks/optimizedv_nonsqr.csv;
	./optimized_sqr benchmarks/optimized_sqr.csv;
	./optimizedv_sqr benchmarks/optimizedv_sqr.csv;
	./optimized_unal benchmarks/optimized_unal.csv;
	./optimizedv_unal benchmarks/optimizedv_unal.csv;
	./unvec_nonsqr benchmarks/unvec_nonsqr.csv;
	./unvec_sqr benchmarks/unvec_sqr.csv;
	./unvec_unal benchmarks/unvec_unal.csv;
	make clean

run_unvectorised:
	make unvectorised;
	make unvectorised_sqr;
	make unvectorised_unal;
	./unvec_nonsqr benchmarks/unvec_nonsqr.csv;
	./unvec_sqr benchmarks/unvec_sqr.csv;
	./unvec_unal benchmarks/unvec_unal.csv;
	make clean