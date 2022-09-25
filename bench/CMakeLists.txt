include(FetchContent)
FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.10.x)

FetchContent_Declare(googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG main) # need master for benchmark::benchmark

FetchContent_MakeAvailable(
        googletest
        googlebenchmark)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

function(rapidfuzz_add_benchmark NAME SOURCE)
    add_executable(bench_${NAME} ${SOURCE})
    target_link_libraries(bench_${NAME} PRIVATE ${PROJECT_NAME})
    target_link_libraries(bench_${NAME} PRIVATE benchmark::benchmark)
endfunction()

rapidfuzz_add_benchmark(lcs bench-lcs.cpp)
rapidfuzz_add_benchmark(fuzz bench-fuzz.cpp)
rapidfuzz_add_benchmark(levenshtein bench-levenshtein.cpp)
