#include <cstdlib>
#include <cstdio>

#include "RtxPlugin.h"
#include "Python.h"

#include "utils.hpp"


class PythonRtx : public RtxPlugin
{
public:
    PythonRtx() {}
    virtual ~PythonRtx() {}
    virtual int Open (TextureCtx &ctx);
    virtual int Fill (TextureCtx &ctx, FillRequest &req);
    virtual int Close (TextureCtx &ctx);
protected:
    int _width, _height;
    int _Open(TextureCtx &ctx);

};


int PythonRtx::Open (TextureCtx &ctx) {   
    int res = _Open(ctx);
    return res;
}

int PythonRtx::_Open(TextureCtx &ctx) {

    ctx.numChannels = 3;
    ctx.minRes.X = ctx.minRes.Y = ctx.maxRes.X = ctx.maxRes.Y = 1;
    ctx.sWrap = ctx.tWrap = TextureCtx::k_Clamp;
    ctx.dataType = TextureCtx::k_Byte;
    ctx.pyramidType = TextureCtx::k_MIP;

    // I don't know why, but without this there are strange artifacts.
    ctx.isLocked = true;

    PyObject *res = dispatch("rtx_open", ctx.argc, ctx.argv);
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

    _width  = ctx.minRes.X = ctx.maxRes.X = PyInt_AS_LONG(width);
    _height = ctx.minRes.Y = ctx.maxRes.Y = PyInt_AS_LONG(height);
    ctx.numChannels = PyInt_AS_LONG(depth);

    printf("PythonRtx: data dimensions: %d, %d, %d\n", ctx.minRes.X, ctx.minRes.Y, ctx.numChannels);
    if (ctx.minRes.X <= 0 || ctx.minRes.Y <= 0 || ctx.numChannels <= 0) {
        printf("PythonRtx: dimensions must be >= 1\n");
        return 3;
    }

    if (!is_power_of_two(_width) || !is_power_of_two(_height)) {
        printf("PythonRtx: dimensions must be powers of 2\n");
        return 3;
    }

    if (!PyString_CheckExact(data)) {
        printf("PythonRtx: data must be a string\n");
        Py_DECREF(res);
        return 4;
    }

    int expected = ctx.minRes.X * ctx.minRes.Y * ctx.numChannels;
    if (PyString_GET_SIZE(data) != expected) {
        printf("PythonRtx: expected %d bytes, but got %d\n", expected, (int)PyString_GET_SIZE(data));
        return 5;
    }

    Py_INCREF(data);
    Py_DECREF(res);

    ctx.userData = (void*) data;

    return 0;
}


int PythonRtx::Fill (TextureCtx& ctx, FillRequest& req) {

    // Don't need the gil for this.

    PyObject *py_data = (PyObject*)ctx.userData;

    char *src_data = py_data ? PyString_AS_STRING(py_data) : NULL;
    char *dst_data = (char *)req.tileData;

    // This can be made more efficient by copying entire rows of data if all of
    // the channels are being requested.
    if (req.numChannels == ctx.numChannels) {
        int row_size = req.tile.size.X * req.numChannels;
        for (int yi = 0, y = req.tile.offset.Y * req.tile.size.Y; yi < req.tile.size.Y; yi++, y++) {
            int offset = req.numChannels * req.tile.offset.X * req.tile.size.X + req.numChannels * _width * y;
            memcpy((void*)(dst_data + yi * row_size), (void*)(src_data + offset), row_size);
        }
    } else {
        for (int yi = 0, y = req.tile.offset.Y * req.tile.size.Y; yi < req.tile.size.Y; yi++, y++) {
        for (int xi = 0, x = req.tile.offset.X * req.tile.size.X; xi < req.tile.size.X; xi++, x++) {
        for (int ci = 0, c = req.channelOffset; ci < req.numChannels; ci++, c++) {
            int offset = c + req.numChannels * x + req.numChannels * _width * y;
            *(dst_data++) = src_data[offset];
        }}}
    }
    return 0;
}


int PythonRtx::Close (TextureCtx& ctx)
{
    if (ctx.userData) {
        Py_DECREF((PyObject*)ctx.userData);
    }
    return 0;
}


RTXPLUGINCREATE
{
    Py_Initialize();
    return new PythonRtx();
}
