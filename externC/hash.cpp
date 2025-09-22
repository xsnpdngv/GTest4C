// hash.cpp
extern "C" {
#include "hash.h"
}
#include <string>

size_t hash_string(const char *str) { return std::hash<std::string>{}(str); }
