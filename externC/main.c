#include "hash.h"

#include <stdio.h>
#include <stdint.h>

int main(void)
{
    const char *s = "Hello, C and C++!";
    size_t h = hash_string(s);
    printf("Hash of '%s' is %zu\n", s, h);
    return 0;
}
