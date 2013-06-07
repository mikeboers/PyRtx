#include <cstdlib>
#include <cstdio>

#include "RtxPlugin.h"
#include "Python.h"


static bool initialized = false;
static PyObject *runtime_module;


static PyObject* dispatch(char const *func_name, PyObject *args) {

    if (!initialized) {
        Py_Initialize();
        runtime_module = PyImport_ImportModule("PythonRtx");
        if (!runtime_module) {
            printf("PythonRtx: could not import PythonRtx:\n");
            PyErr_Print();
            return NULL;
        }
    }

    PyObject *func = PyObject_GetAttrString(runtime_module, func_name);
    if (!func) {
        printf("PythonRtx: could not find PythonRtx.%s:\n", func_name);
        PyErr_Print();
        return NULL;
    }

    PyObject *res = PyObject_CallObject(func, args);

    if (!res) {
        printf("PythonRtx: exception while calling PythonRtx.%s:\n", func_name);
        PyErr_Print();
    }

    Py_DECREF(func);
    return res;

}

static PyObject* dispatch(char const *func_name, unsigned int argc, char const **argv) {
    PyObject *args = PyTuple_New(argc);
    for (unsigned int i = 0; i < argc; i++) {
        PyTuple_SET_ITEM(args, i, PyString_FromString(argv[i]));
    }
    PyObject *res = dispatch(func_name, args);
    Py_DECREF(args);
    return res;
}


class PythonRtx : public RtxPlugin
{
public:
    PythonRtx() {}
    virtual ~PythonRtx() {}
    virtual int Open (TextureCtx& ctx);
    virtual int Fill (TextureCtx& ctx, FillRequest& req);
    virtual int Close (TextureCtx& ctx);

};


int PythonRtx::Open (TextureCtx &ctx)
{
    ctx.numChannels = 3;
    ctx.minRes.X = ctx.minRes.Y = ctx.maxRes.X = ctx.maxRes.Y = 1;
    ctx.sWrap = ctx.tWrap = TextureCtx::k_Clamp;
    ctx.dataType = TextureCtx::k_Byte;
    ctx.pyramidType = TextureCtx::k_MIP;
    ctx.isLocked = false;

    PyObject *res = dispatch("get_texture", ctx.argc, ctx.argv);
    if (!res) {
        return 1;
    }

    if (!PyTuple_CheckExact(res) || PyTuple_GET_SIZE(res) != 4) {
        printf("PythonRtx: must get a tuple with 4 objects\n");
        Py_DECREF(res);
        return 2;
    }

    PyObject *width = PyTuple_GET_ITEM(res, 0);
    PyObject *height = PyTuple_GET_ITEM(res, 1);
    PyObject *depth = PyTuple_GET_ITEM(res, 2);
    PyObject *data = PyTuple_GET_ITEM(res, 3);

    if (!PyInt_CheckExact(width) || !PyInt_CheckExact(height) || !PyInt_CheckExact(depth)) {
        printf("PythonRtx: width, height, and depth must be integers\n");
        Py_DECREF(res);
        return 2;
    }

    ctx.minRes.X = ctx.maxRes.X = PyInt_AS_LONG(width);
    ctx.minRes.Y = ctx.maxRes.Y = PyInt_AS_LONG(height);
    ctx.numChannels = PyInt_AS_LONG(depth);

    if (!PyString_CheckExact(data)) {
        printf("PythonRtx: data must be a string\n");
        Py_DECREF(res);
        return 3;
    }

    Py_INCREF(data);
    Py_DECREF(res);

    ctx.userData = (void*) data;

    return 0;
}


int
PythonRtx::Fill (TextureCtx& ctx, FillRequest& req) {

    PyObject *py_data = (PyObject*)ctx.userData;

    char *src_data = py_data ? PyString_AS_STRING(py_data) : NULL;
    char *dst_data = (char *)req.tileData;

    for (int yi = 0, y = req.tile.offset.Y * req.tile.size.Y; yi < req.tile.size.Y; yi++, y++) {
    for (int xi = 0, x = req.tile.offset.X * req.tile.size.X; xi < req.tile.size.X; xi++, x++) {
    for (int ci = 0, c = req.channelOffset; ci < req.numChannels; ci++, c++) {
        if (src_data) {
            int offset = c + req.numChannels * x + req.numChannels * req.imgRes.X * y;
            *(dst_data++) = src_data[offset];
        } else {
            *(dst_data++) = 127;
        }
    }}}
    
    return 0;
}


int PythonRtx::Close (TextureCtx& ctx)
{
    printf("PythonRtx::Close()\n");
    if (ctx.userData) {
        Py_DECREF((PyObject*)ctx.userData);
    }
    return 0;
}


RTXPLUGINCREATE
{
    return new PythonRtx();
}
