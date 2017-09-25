#include <benchmark/benchmark.h>
#include <cmath>
#include <deque>

extern "C" {
#include "../src/memory.h"
}

class MemoryManagerBenchmark : public benchmark::Fixture {
    protected:
        MemoryManager* manager;
        std::deque<void*> addresses;

        void SetUp(benchmark::State &st) override {
            memory_manager_enter(manager);

            for(int i = 0; i < st.range(0); ++i) {
                addresses.push_front(memory_manager_malloc(1, "", 0, "", manager));
            }
        }

        void TearDown(benchmark::State &st) override {
            memory_manager_exit(manager);
            addresses.clear();
        }
};

BENCHMARK_DEFINE_F(MemoryManagerBenchmark, Alloc)(benchmark::State &st) {
    while(st.KeepRunning()) {
        memory_manager_malloc(static_cast<size_t>(st.range(0)), "", 0, "", manager);
    }
}

BENCHMARK_DEFINE_F(MemoryManagerBenchmark, Free)(benchmark::State &st) {
    while(st.KeepRunning()) {
        for(auto address: addresses)
            memory_manager_free(address, manager);
    }
}

BENCHMARK_REGISTER_F(MemoryManagerBenchmark, Alloc)->Ranges({{4, 16}});
BENCHMARK_REGISTER_F(MemoryManagerBenchmark, Free)->Ranges({{4, 16}});