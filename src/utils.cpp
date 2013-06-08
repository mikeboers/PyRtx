#include <cstring>

#include "utils.hpp"


int is_power_of_two (unsigned int x) {
    while (((x & 1) == 0) && x > 1) {
        x >>= 1;
    }
    return x == 1;
}


PyObject* dispatch(char const *func_name, unsigned int argc, char const **argv) {

    // Parse arguments, pulling out "module" and "function".
    PyObject *kwargs = PyDict_New();
    char const *module_name = NULL;
    for (unsigned int i = 0; i < argc; i += 2) {
        if (!strcmp(argv[i], "module")) {
            module_name = argv[i + 1];
        } else if (!strcmp(argv[i], "function")) {
            func_name = argv[i + 1];
        } else {
            PyObject *value = PyString_FromString(argv[i + 1]);
            PyDict_SetItemString(kwargs, argv[i], value);
            Py_DECREF(value);
        }
    }

    if (!module_name) {
        printf("PythonRtx: \"module\" must be provided.\n");
        Py_DECREF(kwargs);
        return NULL;
    }

    PyObject *module = PyImport_ImportModule(module_name);
    if (!module) {
        printf("PythonRtx: Exception while importing \"%s\":\n", module_name);
        PyErr_Print();
        Py_DECREF(kwargs);
        return NULL;
    }

    PyObject *func = PyObject_GetAttrString(module, func_name);
    Py_DECREF(module); // Not needed anymore.
    if (!func) {
        printf("PythonRtx: Exception while fetching \"%s:%s\":\n", module_name, func_name);
        PyErr_Print();
        Py_DECREF(kwargs);
        return NULL;
    }

    PyObject *args = Py_BuildValue("()");
    PyObject *res = PyObject_Call(func, args, kwargs);
    Py_DECREF(func);
    Py_DECREF(args);
    Py_DECREF(kwargs);
    if (!res) {
        printf("PythonRtx: Exception while calling \"%s:%s\":\n", module_name, func_name);
        PyErr_Print();
        return NULL;
    }

    return res;

}
