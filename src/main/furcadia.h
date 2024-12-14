#ifndef FURCADIA_H_
#define FURCADIA_H_
#include <3ds.h>
#include <citro3d.h>
#include <vector>
#include "appbase.h"
#include "3dsshader.h"
#include "scene.h"

#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

#define SCREEN_TOP_WIDTH  400
#define SCREEN_TOP_HEIGHT 240
#define SCREEN_BOTTOM_WIDTH  320
#define SCREEN_BOTTOM_HEIGHT 240

#define CLEAR_COLOR 0x68B0D8FF

static u32 *SOC_buffer = NULL;

#define DISPLAY_TRANSFER_FLAGS \
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
    GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
    GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))


class Furcadia : public Application
{
protected:
    // Screens
    C3D_RenderTarget* mScreenLeft;
    C3D_RenderTarget* mScreenRight;
    C3D_RenderTarget* mScreenBottom;
    
    uint8_t mTick = 0;
    
    std::shared_ptr<Scene> mRootScene;
    
    void initialize() override;
    void update() override;
    void render() override;
    void cleanup() override;
    bool shouldRun() override;
};

#endif 
