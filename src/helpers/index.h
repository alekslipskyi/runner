#ifndef RUNNER_HELPERS_H
#define RUNNER_HELPERS_H

#include <ostream>
#include <iostream>
#include <stdio.h>

using namespace std;

namespace helpers {
    template <class Array, class Element>
    bool includes(Array array, Element element) {
        for (auto &elemArray: array) if (string(elemArray) == string(element)) return true;

        return false;
    }

    template <class Array, class Element>
    Element find(Array array, Element element) {
        for (auto &elemArray: array) if (elemArray == element) return elemArray;

        return nullptr;
    }
}

#endif //RUNNER_INDEX_H
