#include <benchmark/benchmark.h>
#include <cmath>
#include <deque>

extern "C" {
#include "../src/memory.h"
}

class HashBenchmark : public benchmark::Fixture {
};

size_t hash(const char* str) {
    unsigned hash = 5381;
    int c;

    while((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}


BENCHMARK_DEFINE_F(HashBenchmark, Hash)(benchmark::State &st) {
    while(st.KeepRunning()) {
        hash("ahoj soniku");
    }
}

BENCHMARK_DEFINE_F(HashBenchmark, Strcmp)(benchmark::State &st) {
    while(st.KeepRunning()) {
        strcmp("ahoj soniku", "ahoj soniku");
    }
}

BENCHMARK_REGISTER_F(HashBenchmark, Hash);
BENCHMARK_REGISTER_F(HashBenchmark, Strcmp);