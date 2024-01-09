//
// Created by Mingyu on 24-1-8.
//

#include "utils.h"

bool isFileExists_ifstream(const char *name) {
    std::ifstream f(name);
    return f.good();
}