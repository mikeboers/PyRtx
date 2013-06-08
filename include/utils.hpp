#ifndef PYRTX_UTILS_H
#define PYRTX_UTILS_H

#include "Python.h"


int is_power_of_two (unsigned int x);
PyObject* dispatch(char const *func_name, unsigned int argc, char const **argv);


#endif
