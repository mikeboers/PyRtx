#include <cstdlib>
#include <cstdio>

#include "RtxPlugin.h"


class Gradient : public RtxPlugin
{
public:
    Gradient() {}
    virtual ~Gradient() {}
    virtual int Open (TextureCtx& ctx);
    virtual int Fill (TextureCtx& ctx, FillRequest& req);
    virtual int Close (TextureCtx& ctx);
};


int
Gradient::Open (TextureCtx &ctx)
{
    ctx.numChannels = 3;
    ctx.minRes.X = 1;
    ctx.minRes.Y = 1;
    ctx.maxRes.X = 32768;
    ctx.maxRes.Y = 32768;
    ctx.sWrap = TextureCtx::k_Clamp;
    ctx.tWrap = TextureCtx::k_Clamp;
    ctx.dataType = TextureCtx::k_Float;
    ctx.pyramidType = TextureCtx::k_MIP;
    ctx.isLocked = false;

    printf("Gradient::Open(...) with %d args.\n", ctx.argc);
    for (int i = 0; i < ctx.argc; i++) {
        printf("\t%d: \"%s\"\n", i, ctx.argv[i]);
    }

    // Could set ctx.userData here...
    return 0;
}


int
Gradient::Fill (TextureCtx& ctx, FillRequest& req) {

    /*
    printf("Gradient::Fill(...):\n");
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


int Gradient::Close (TextureCtx& ctx)
{
    printf("Gradient::Close()\n");
    if (ctx.userData) {
        free (ctx.userData);
    }
    return 0;
}


RTXPLUGINCREATE
{
    return new Gradient();
}
