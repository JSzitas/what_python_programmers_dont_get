all: unoptimized, optimized, optimizedv, unoptimized_sqr, optimized_sqr, optimizedv_sqr, unoptimized_unal, optimized_unal, optimizedv_unal

unoptimized:
	clang++ -std=c++17 -O0 -g -Wall nonsqr.cpp -o unoptimized
optimized:
	clang++ -std=c++17 -O3 -Wall nonsqr.cpp -o optimized
optimizedv:
	clang++ -std=c++17 -O3 -march=native -Wall nonsqr.cpp -o optimized

unoptimized_sqr:
	clang++ -std=c++17 -O0 -g -Wall sqr.cpp -o unoptimized
optimized_sqr:
	clang++ -std=c++17 -O3 -Wall sqr.cpp -o optimized
optimizedv_sqr:
	clang++ -std=c++17 -O3 -march=native -Wall sqr.cpp -o optimized

unoptimized_unal:
	clang++ -std=c++17 -O0 -g -Wall unal.cpp -o unoptimized
optimized_unal:
	clang++ -std=c++17 -O3 -Wall unal.cpp -o optimized
optimizedv_unal:
	clang++ -std=c++17 -O3 -march=native -Wall unal.cpp -o optimized
clean:
	rm -rf unoptimized, optimized, optimizedv, unoptimized_sqr, optimized_sqr, optimizedv_sqr, unoptimized_unal, optimized_unal, optimizedv_unal