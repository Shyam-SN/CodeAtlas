add_executable(codeatlas_benchmarks bench_codeatlas.cpp)
target_link_libraries(codeatlas_benchmarks PRIVATE
    codeatlas
    benchmark::benchmark_main
)
