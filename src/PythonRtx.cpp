#include <cstdlib>
#include <cstdio>

#include "RtxPlugin.h"
#include "Python.h"


static bool initialized = false;
static PyObject *runtime_module;
static PyObject *runtime_entrypoint;


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
    ctx.minRes.X = 1024;
    ctx.minRes.Y = 1024;
    ctx.maxRes.X = 1024;
    ctx.maxRes.Y = 1024;
    ctx.sWrap = TextureCtx::k_Clamp;
    ctx.tWrap = TextureCtx::k_Clamp;
    ctx.dataType = TextureCtx::k_Float;
    ctx.pyramidType = TextureCtx::k_MIP;
    ctx.isLocked = false;

    printf("PythonRtx::Open(...) with %d args.\n", ctx.argc);
    for (int i = 0; i < ctx.argc; i++) {
        printf("\t%d: \"%s\"\n", i, ctx.argv[i]);
    }

    if (!initialized) {
        Py_Initialize();
        runtime_module = PyImport_ImportModule("python_for_renderman.runtime");
        if (!runtime_module) {
            printf("Python could not import 'python_for_renderman.runtime'\n");
            return 1;
        }
        runtime_entrypoint = PyObject_GetAttrString(runtime_module, "entrypoint");
        if (!runtime_entrypoint) {
            printf("Python 'entrypoint' function missing from 'python_for_renderman.runtime'.\n");
            return 2;
        }
        initialized = true;
    }

    PyObject *args = PyTuple_New(ctx.argc);
    for (int i = 0; i < ctx.argc; i++) {
        PyTuple_SET_ITEM(args, i, PyString_FromString(ctx.argv[i]));
    }

    PyObject *res = PyObject_CallObject(runtime_entrypoint, args);
    Py_DECREF(args);
    
    if (!res) {
        printf("Error while executing entrypoint:\n");
        PyErr_Print();
        return 3;
    } else {
        Py_DECREF(res);
    }

    // Could set ctx.userData here...
    return 0;
}


int
PythonRtx::Fill (TextureCtx& ctx, FillRequest& req) {

    /*
    printf("PythonRtx::Fill(...):\n");
    printf("\timgRes: %d, %d\n", req.imgRes.X, req.imgRes.Y);
    printf("\ttile.offset: %d, %d\n", req.tile.offset.X, req.tile.offset.Y);
    printf("\ttile.size: %d, %d\n", req.tile.size.X, req.tile.size.Y);
    //*/

    RtFloat *fdata = (RtFloat *)req.tileData;
    for (int yi = 0, y = req.tile.offset.Y * req.tile.size.Y; yi < req.tile.size.Y; yi++, y++) {
    for (int xi = 0, x = req.tile.offset.X * req.tile.size.X; xi < req.tile.size.X; xi++, x++) {
    for (int ci = 0, c = req.channelOffset; ci < req.numChannels; ci++, c++) {
        switch (c) {
            case 0:
                *(fdata++) = float(x) / float(req.imgRes.X);
                break;
            case 1:
                *(fdata++) = float(y) / float(req.imgRes.Y);
                break;
            case 2:
                *(fdata++) = 0.5;
                break;
        }
    }}}
    
    return 0;
}


int PythonRtx::Close (TextureCtx& ctx)
{
    printf("PythonRtx::Close()\n");
    if (ctx.userData) {
        free (ctx.userData);
    }
    return 0;
}


RTXPLUGINCREATE
{
    return new PythonRtx();
}
