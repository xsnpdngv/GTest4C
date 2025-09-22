// greeter.h
#ifndef GREETER_H_
#define GREETER_H_

typedef struct greeter_t greeter_t;

greeter_t *greeterCreate(const char *greeting);
const char *greeterGreet(greeter_t *self, const char *name);
void greeterDestroy(greeter_t **self);

#endif // GREETER_H_
