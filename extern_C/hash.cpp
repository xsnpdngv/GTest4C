// hash.cpp
#include <string>
#include <functional>

extern "C" size_t hash_string(const char *str) { return std::hash<std::string>{}(str); }
