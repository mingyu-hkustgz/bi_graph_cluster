#include "utils.h"

bool isFileExists_ifstream(const char *name) {
    std::ifstream f(name);
    return f.good();
}