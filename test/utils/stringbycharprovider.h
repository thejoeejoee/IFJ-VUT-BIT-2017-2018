#ifndef _STRINGBYCHARPROVIDER_H
#define _STRINGBYCHARPROVIDER_H

#include <cstdlib>
#include <iostream>

class StringByCharProvider {
    private:
        std::string string;

    protected:
        size_t counter;

        StringByCharProvider() : counter(0) {}

    public:
        static StringByCharProvider* instance();

        void set_string(std::string str);

        int next_char();
};

#endif //_STRINGBYCHARPROVIDER_H
