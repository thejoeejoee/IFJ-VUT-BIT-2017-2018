#include "stringbycharprovider.h"

StringByCharProvider* StringByCharProvider::instance() {
    static StringByCharProvider instance;
    return &instance;
}

int StringByCharProvider::next_char() {
    return string.at(counter++);
}

void StringByCharProvider::set_string(std::string str) {
    string = std::move(str);
    counter = 0;
}
