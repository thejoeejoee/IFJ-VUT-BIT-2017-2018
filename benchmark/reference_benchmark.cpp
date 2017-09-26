#include <benchmark/benchmark.h>

static void ref() {
    const int cnt = 1000;
    for(int i = 0; i < cnt; i++) {
#ifdef _MSC_VER
        __asm { nop };
#else
        __asm__ __volatile__("nop");
#endif
    }
}

static void Reference(benchmark::State &state) {
    while(state.KeepRunning()) {
        ref();
    }
}

// Register the function as a benchmark
BENCHMARK(Reference);