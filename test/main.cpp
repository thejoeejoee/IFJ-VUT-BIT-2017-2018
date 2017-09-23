#include "gtest/gtest.h"

extern "C" {
#include "../src/debug.h"
#include "../src/memory.h"
}

short log_verbosity = LOG_VERBOSITY_INFO;

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    memory_manager_enter(NULL);
    auto ret = RUN_ALL_TESTS();
    memory_manager_exit(NULL);
    return ret;
}
