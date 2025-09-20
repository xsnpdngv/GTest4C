// greeter.c
#include "greeter.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct greeter_t
{
    char *greeting; // "Hello", "Hola", "Bonjour", "Ciao", "Üdv", etc
    char buffer[100]; // output goes here
};

greeter_t *greeterCreate(const char *greeting)
{
    if( ! greeting) { return NULL; }
    greeter_t *self = malloc(sizeof(greeter_t));
    self->greeting = strdup(greeting);
    return self;
}

const char *greeterGreet(greeter_t *self, const char *name)
{
    if( ! self) { return NULL; }
    snprintf(self->buffer, sizeof(self->buffer), "%s, %s!", self->greeting, name ?: "World");
    loggerWriteLog(self->buffer);
    return self->buffer;
}

void greeterDestroy(greeter_t **self)
{
    assert(self);
    if( ! *self) { return; }
    free((*self)->greeting);
    free((*self));
    *self = NULL;
}
