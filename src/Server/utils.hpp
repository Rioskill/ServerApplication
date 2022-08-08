#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>

static void ASSERT(bool check, std::string message) {
    if (!check) {
        std::cerr << message << std::endl;
        exit(1);
    }
}

#endif