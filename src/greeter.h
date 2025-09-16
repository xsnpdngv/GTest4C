// greeter.h
#ifndef GREETER_H
#define GREETER_H

typedef struct greeter_t greeter_t;

greeter_t *greeterCreate(const char *greeting);
const char *greeterGreet(greeter_t *self, const char *name);
void greeterDestroy(greeter_t **self);

#endif
