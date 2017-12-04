#ifndef _COMMON_H
#define _COMMON_H

#define UNUSED(x) (void) (x)
#define MSVC_EXPAND(x) x
#define GET_FIRST_ARG(N, ...) N
#define GET_OVERLOADED_MACRO12(_1, _2, ...) MSVC_EXPAND(GET_FIRST_ARG(__VA_ARGS__, 0))
#define GET_OVERLOADED_MACRO123(_1, _2, _3, ...) MSVC_EXPAND(GET_FIRST_ARG(__VA_ARGS__, 0))
#define GET_OVERLOADED_MACRO1234(_1, _2, _3, _4, ...) MSVC_EXPAND(GET_FIRST_ARG(__VA_ARGS__, 0))
#define GET_OVERLOADED_MACRO12345(_1, _2, _3, _4, _5, ...) MSVC_EXPAND(GET_FIRST_ARG(__VA_ARGS__, 0))

/**
 * @brief Copy string (native c) and get pointer for it
 *
 * @param char* input string
 * @return char* copy of the string
 */
char* c_string_copy(const char* string);

/**
 * Round double to nearest even integer.
 * @param x to round
 * @return rounded
 */
int round_even(double x);

#endif // _COMMON_H
