#ifndef PYRTX_UTILS_H
#define PYRTX_UTILS_H

#include "Python.h"


#define GIL_ENTER printf("Aquiring GIL... "); PyGILState_STATE _gil_state; _gil_state = PyGILState_Ensure(); printf("Done.\n");
#define GIL_EXIT  printf("Releasing GIL... "); PyGILState_Release(_gil_state); printf("Done.\n");

int is_power_of_two (unsigned int x);
PyObject* dispatch(char const *func_name, unsigned int argc, char const **argv);


#endif
