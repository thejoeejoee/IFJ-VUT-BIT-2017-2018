#include <benchmark/benchmark.h>
#include <cmath>

extern "C" {
#include "../src/lexer.h"
}

#include "../test/utils/stringbycharprovider.h"

class LexerBenchmark : public benchmark::Fixture {
    protected:
        Lexer* lexer;
        StringByCharProvider* provider = StringByCharProvider::instance();

        LexerBenchmark() {
            lexer = lexer_init(token_stream);
        }

        void tokenize(benchmark::State &st) {
            Token token{};
            while(st.KeepRunning()) {
                do {
                    token = lexer_next_token(lexer);
                } while(!(token.type == TOKEN_ERROR || token.type == TOKEN_EOF));
            }
        }
};

BENCHMARK_DEFINE_F(LexerBenchmark, Factorial)(benchmark::State &st) {
    provider->setString(R"RAW(
/'Program 2: Vypocet faktorialu (rekurzivne)'/
Declare Function factorial (n As Integer) As Integer
Function factorial (n As Integer) As Integer
Dim temp_result As Integer
Dim decremented_n As Integer
Dim result As Integer
If n < 2 Then
result = 1
Else
decremented_n = n - 1
temp_result = factorial(decremented_n)
result = n * temp_result
End If
Return result
End Function
)RAW");
    tokenize(st);
}

BENCHMARK_DEFINE_F(LexerBenchmark, StringsManipulation)(benchmark::State &st) {
    provider->setString(R"RAW(
/' Program 3: Prace s retezci a vestavenymi funkcemi '/
Scope
Dim s1 aS String
Dim s2 As String
Dim s1len As Integer
'Hlavni telo programu
s1 As String
s2 As String
s1len As Integer
s1 = !"Toto je nejaky text"
s2 = s1 + !", ktery jeste trochu obohatime"
Print s1; !"\\n"; s2; !"\\n";
s1len = Length(s1)
s1len = s1len - 4 + 1
s1 = SubStr(s2, s1len, 4)
Print !"4 znaky od "; s1len; !". znaku v \\\""; s2; !"\\\":"; s1; !"\\n";
Print !"Zadejte serazenou posloupnost vsech malych pismen a-h, ";
Print !"pricemz se pismena nesmeji v posloupnosti opakovat";
Input s1
Do While (s1 <> !"abcdefgh")
Print !"\\nSpatne zadana posloupnost, zkuste znovu";
Input s1
Loop
End Scope
)RAW");
    tokenize(st);
}

BENCHMARK_REGISTER_F(LexerBenchmark, Factorial);
BENCHMARK_REGISTER_F(LexerBenchmark, StringsManipulation);
