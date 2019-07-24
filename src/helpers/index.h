#ifndef RUNNER_HELPERS_H
#define RUNNER_HELPERS_H

#include <ostream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <cstring>

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

    template <class String, class Delimiter>
    vector<string> split(String search_line, Delimiter delimiter) {
        vector<string> results;
        int current_summary_length = 0;
        String __searchLine = search_line;
        string token = "none";
        int i = 0;

        if (!strlen(search_line.c_str()) || !strlen(delimiter)) return results;

        while (strlen(token.c_str())) {
            size_t found_index = __searchLine.find(delimiter);

            if (found_index == string::npos) {
                token = "";
                if (strlen(__searchLine.c_str())) {
                    results.push_back(__searchLine);
                }
            } else {
                token = __searchLine.substr(0, found_index);
                current_summary_length += found_index + 1;
                __searchLine = search_line.substr(current_summary_length, strlen(search_line.c_str()));

                results.push_back(token);
            }

            i++;
        }

        return results;
    }

}

#endif //RUNNER_HELPERS_H
