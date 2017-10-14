#include "gtest/gtest.h"

extern "C" {
#include "../src/memory.h"
#include "../src/debug.h"
#include "../src/symtable.h"
#include "../src/symtable_function.h"
}

class SymbolTableFunctionTestFixture : public ::testing::Test {
    protected:
        SymbolTable* symbol_table = nullptr;

        virtual void SetUp() {
            symbol_table = symbol_table_function_init(2);
        }

        virtual void TearDown() {
            symbol_table_free(symbol_table);
        }
};

TEST_F(SymbolTableFunctionTestFixture, FindingUndefined) {
    EXPECT_EQ(
            symbol_function_find_declared_function_without_definition(symbol_table),
            nullptr
    );

    auto function = symbol_table_function_get_or_create(symbol_table, "function74533");
    auto function2 = symbol_table_function_get_or_create(symbol_table, "function745741");

    EXPECT_EQ(
            symbol_function_find_declared_function_without_definition(symbol_table),
            nullptr
    );

    function->declared = true;

    EXPECT_EQ(
            symbol_function_find_declared_function_without_definition(symbol_table),
            function
    );

    function->defined = true;
    function2->declared = true;

    EXPECT_EQ(
            symbol_function_find_declared_function_without_definition(symbol_table),
            function2
    );

    function2->defined = true;

    EXPECT_EQ(
            symbol_function_find_declared_function_without_definition(symbol_table),
            nullptr
    );
}

