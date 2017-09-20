#ifndef _STRINGBYCHARPROVIDER_H
#define _STRINGBYCHARPROVIDER_H

#include <cstdlib>
#include <iostream>

class StringByCharProvider {
    private:
        std::string string;
        size_t counter;

    protected:
        StringByCharProvider() : counter(0) {}

    public:
        static StringByCharProvider* instance();

        void setString(std::string str);

        int nextChar();
};


int token_stream();

#endif //_STRINGBYCHARPROVIDER_H
