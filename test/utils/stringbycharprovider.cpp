#include "stringbycharprovider.h"

StringByCharProvider* StringByCharProvider::instance() {
    static StringByCharProvider instance;
    return &instance;
}

int StringByCharProvider::nextChar() {
    return counter >= string.length() ? EOF : string.at(counter++);
}

void StringByCharProvider::setString(std::string str) {
    string = std::move(str);
    counter = 0;
}


int token_stream() {
    StringByCharProvider* instance = StringByCharProvider::instance();
    return instance->nextChar();
}