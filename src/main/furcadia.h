#ifndef FURCADIA_H_
#define FURCADIA_H_
#include "appbase.h"
#include <3ds.h>
#include <citro3d.h>

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

#define vertex_list_count (sizeof(vertex_list)/sizeof(vertex_list[0]))
typedef struct {
    float position[3];
    float uv[2];
} vertex;
/*
 * A B
 * C D
 */
static const vertex vertex_list[] =
{
    { { 0.f, 0.f, 0.f }, {0.f, 1.f} },
    { { 1.f, 0.f, 0.f }, {1.f, 1.f} },
    { { 0.f, 1.f, 0.f }, {0.f, 0.f} },
    
    { { 1.f, 0.f, 0.f }, {1.f, 1.f} },
    { { 1.f, 1.f, 0.f }, {1.f, 0.f} },
    { { 0.f, 1.f, 0.f }, {0.f, 0.f} },
};

static const uint8_t morton_order[] = {
     0,  1,  4,  5,  16, 17, 20, 21,
     2,  3,  6,  7,  18, 19, 22, 23,
     8,  9, 12, 13,  24, 25, 28, 29,
    10, 11, 14, 15,  26, 27, 30, 31,
    32, 33, 36, 37,  48, 49, 52, 53,
    34, 35, 38, 39,  50, 51, 54, 55,
    40, 41, 44, 45,  56, 57, 60, 61,
    42, 43, 46, 47,  58, 59, 62, 63
};

class Furcadia : public Application
{
protected:
    // Screens
    C3D_RenderTarget* mScreenLeft;
    C3D_RenderTarget* mScreenRight;
    C3D_RenderTarget* mScreenBottom;
    
    // Shaders
    DVLB_s* mUnlitGenericDLVB;
    shaderProgram_s mUnlitGenericProgram;
    
    uint8_t mTick = 0;
    
    void initialize() override;
    void update() override;
    void render() override;
    void cleanup() override;
    bool shouldRun() override;
};

#endif 
