#ifndef PYRTX_UTILS_H
#define PYRTX_UTILS_H

#include "Python.h"

#define Py_GIL_DEBUG 1

#define Py_GIL_ENSURE  \
    if (Py_GIL_DEBUG) { \
        printf("Aquiring GIL at %s:%d from 0x%lx ... ", __FILE__, __LINE__, (long)PyThreadState_Get()); \
        fflush(stdout); \
    } \
    /* PyGILState_STATE _gil_state = PyGILState_Ensure(); */ \
    PyEval_AcquireLock(); \
    if (Py_GIL_DEBUG) printf("Done.\n");

#define Py_GIL_RELEASE \
    if (Py_GIL_DEBUG) { \
        printf("Releasing GIL at %s:%d from 0x%lx ... ", __FILE__, __LINE__, (long)PyThreadState_Get()); \
        fflush(stdout); \
    } \
    /* PyGILState_Release(_gil_state); */ \
    PyEval_ReleaseLock(); \
    if (Py_GIL_DEBUG) printf("Done.\n");

int is_power_of_two (unsigned int x);
PyObject* dispatch(char const *func_name, unsigned int argc, char const **argv);


#endif
