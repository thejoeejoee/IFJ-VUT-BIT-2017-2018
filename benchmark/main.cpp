#include <benchmark/benchmark.h>
// TODO: resolve deps problems with dynamic memory manager switching based on cmake build type
#include "../src/memory.c"

short log_verbosity = LOG_VERBOSITY_NONE;

static void BM_StringCreation(benchmark::State &state) {
    while (state.KeepRunning())
        std::string empty_string;
}
// Register the function as a benchmark
BENCHMARK(BM_StringCreation);

// Define another benchmark
static void BM_StringCopy(benchmark::State &state) {
    std::string x = "hello";
    while (state.KeepRunning())
        std::string copy(x);
}

BENCHMARK(BM_StringCopy);

BENCHMARK_MAIN();