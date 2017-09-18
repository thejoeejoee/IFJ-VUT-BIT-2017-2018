#include "gtest/gtest.h"

#include "../src/debug.h"

short log_verbosity = LOG_VERBOSITY_DEBUG;

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
