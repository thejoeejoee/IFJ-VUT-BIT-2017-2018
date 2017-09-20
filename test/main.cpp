#include "gtest/gtest.h"

extern "C" {
#include "../src/debug.h"
}

short log_verbosity = LOG_VERBOSITY_INFO;

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
