#include <utility>

#include "gtest/gtest.h"

#include "../src/lexer_fsm.c"

class StrProvider {
    private:
        std::string m_str;

    protected:
        size_t counter;

        StrProvider() : counter(0) {}

    public:
        friend int token_stream();

        static StrProvider* instance() {
            static StrProvider instance;
            return &instance;
        }

        void set_src(std::string str) {
            m_str = std::move(str);
            counter = 0;
        }

        int next_char() {
            return m_str.at(counter++);
        }
};

int token_stream() {
    StrProvider* instance = StrProvider::instance();
    return instance->next_char();
}

TEST(LexerFSMTest, LineComment) {
    StrProvider* provider = StrProvider::instance();

    provider->set_src("@");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream),
            LEX_FSM__LEG_SHOT
    );

    provider->set_src("'");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream),
            LEX_FSM__COMMENT_LINE
    );

    provider->set_src("a");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_LINE, token_stream),
            LEX_FSM__COMMENT_LINE
    );

    provider->set_src("'");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_LINE, token_stream),
            LEX_FSM__COMMENT_LINE
    );

    provider->set_src("\n");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_LINE, token_stream),
            LEX_FSM__INIT
    );
}

TEST(LexerFSMTest, BlockComment) {
    // TODO: missing tests for block comment
}