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
    SymbolFunction* item = symbol_table_function_get_or_create(symbol_register->functions, "printf");
    SymbolFunction* find_item = symbol_table_function_get(symbol_register->functions, "printf");

    EXPECT_EQ(
            item,
            find_item
    ) << "Error finding item";

    SymbolFunction* error_item = symbol_table_function_get(
            symbol_register->functions,
            "neexistujici"
    );

    EXPECT_EQ(
            error_item,
            nullptr
    ) << "Find unknown function";

}

TEST_F(SymbolRegisterTestFixture, FindingVariablesInStack) {
    SymbolVariable* symbol_variable = symbol_table_variable_get_or_create(
            symbol_register->variables->symbol_table,
            "foo"
    );

    EXPECT_NE(
            symbol_variable,
            nullptr
    ) << "Created variable.";

    EXPECT_NE(
            symbol_variable,
            nullptr
    ) << "Auto allocated data ptr.";

    symbol_register_push_variables_table(symbol_register);

    SymbolVariable* found_variable = symbol_register_find_variable(symbol_register, "foo");

    EXPECT_EQ(
            found_variable,
            nullptr
    ) << "Not found variable after stack push";

    found_variable = symbol_register_find_variable_recursive(symbol_register, "foo");

    EXPECT_EQ(
            found_variable,
            symbol_variable
    ) << "Recursively found variable.";

    SymbolVariable* new_variable = symbol_table_variable_get_or_create(
            symbol_register->variables->symbol_table,
            "bar"
    );

    EXPECT_EQ(
            symbol_register_find_variable(symbol_register, "bar"),
            new_variable
    ) << "Found variable in new frame.";

    symbol_register_pop_variables_table(symbol_register);

    EXPECT_EQ(
            symbol_register_find_variable(symbol_register, "bar"),
            nullptr
    ) << "Pooped frame with variable.";

    EXPECT_EQ(
            symbol_register_find_variable(symbol_register, "foo"),
            found_variable
    ) << "Found variable after stack changes.";
}


TEST_F(SymbolRegisterTestFixture, InvalidStackAccess) {
    SymbolVariable* found_item;
    SymbolVariable* symbol_variable = symbol_table_variable_get_or_create(
            symbol_register->variables->symbol_table,
            "foo_variable_42"
    );
    EXPECT_NE(
            symbol_variable,
            nullptr
    ) << "Found variable";
    symbol_register_pop_variables_table(symbol_register);
    found_item = symbol_table_variable_get(
            symbol_register->variables->symbol_table,
            "foo_variable_42"
    );
    EXPECT_EQ(
            found_item,
            nullptr
    ) << "Not found variable in reset stack.";
}
