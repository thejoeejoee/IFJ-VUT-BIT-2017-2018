#include "gtest/gtest.h"

extern "C" {
#include "../src/memory.h"
#include "../src/debug.h"
#include "../src/symtable.h"
#include "../src/symbol_register.h"
}

#include "utils/functioncallcounter.h"



class SymbolRegisterTestFixture : public ::testing::Test {
    protected:
        SymbolRegister* symbol_register;

        virtual void SetUp() {
            symbol_register = symbol_register_init();
        }

        virtual void TearDown() {
            symbol_register_free(&symbol_register);
        }

};

TEST_F(SymbolRegisterTestFixture, PushAndGetFunctions) {

    // Put one item
    SymbolTableListItemSymbolFunction* item = symbol_table_function_get_or_create(symbol_register->functions, "printf");

    // Get one item
    SymbolTableListItemSymbolFunction* find_item = symbol_table_function_get(symbol_register->functions, "printf");

    EXPECT_EQ(
        item,
        find_item
    ) << "Error finding item";

    SymbolTableListItemSymbolFunction* error_item = symbol_table_function_get(symbol_register->functions, "neexistujici");

    EXPECT_EQ(
            NULL,
            error_item
    ) << "Error getting error symbol";

}

TEST_F(SymbolRegisterTestFixture, PushAndGetVarsOneTable) {

    symbol_register_push_variables_table(symbol_register);

}
