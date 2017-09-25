#include <benchmark/benchmark.h>

extern "C" {
#include "../src/memory.h"
}

short log_verbosity = LOG_VERBOSITY_NONE;

BENCHMARK_MAIN();