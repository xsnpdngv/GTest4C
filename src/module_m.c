// module_m.c
#include "greeter.h"
#include <stdio.h>

int main()
{
    greeter_t *g __attribute__((cleanup(greeterDestroy))) = greeterCreate("Hellloooo");
    printf("%s\n", greeterGreet(g, "Woooorld"));
}
