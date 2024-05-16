#pragma once
#include <iostream>

#define ASSERT(x, msg) \
    if (!(x)) { \
        std::cerr << "Assertion failed: " << #x << ", " << msg << std::endl; \
        std::terminate(); \
    }