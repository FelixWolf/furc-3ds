#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cassert>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <malloc.h>

#include "furcadia.h"
#include "fox5.h"
#include "testimg.h"
#include "3dstexture.h"
#include "3dsshader.h"
#include "demoscene.h"

void Furcadia::initialize()
{
    // Initialize libctru
    
    // Prepare networking
    SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

    if(SOC_buffer == NULL)
        throw std::runtime_error("Failed to allocate SOC buffer");

    // Now intialise soc:u service
    if(socInit(SOC_buffer, SOC_BUFFERSIZE) != 0)
        throw std::runtime_error("Failed to initialize SOC service");

    link3dsStdio(); // Enable debugger
    
    osSetSpeedupEnable(true); // Enable higher speed CPU
    
    hidInit(); // Initialize inputs
    gfxInitDefault(); // Initialize graphics
    ndspInit(); // Initialize sound
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE); // Initialize C3D
    
    // Initialize ROMFS
    Result rc = romfsInit();
    if (rc)
        throw std::runtime_error("romfsInit failed: " + std::to_string(rc));
    else
        chdir("romfs:/");
    
    gfxSetDoubleBuffering(GFX_TOP, true);
    gfxSetDoubleBuffering(GFX_BOTTOM, true);
    
    gfxSet3D(true);
    
    mScreenLeft  = C3D_RenderTargetCreate(SCREEN_TOP_HEIGHT, SCREEN_TOP_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(mScreenLeft,  GFX_TOP, GFX_LEFT,  DISPLAY_TRANSFER_FLAGS);
    
    mScreenRight = C3D_RenderTargetCreate(SCREEN_TOP_HEIGHT, SCREEN_TOP_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(mScreenRight, GFX_TOP, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);
    
    mScreenBottom = C3D_RenderTargetCreate(SCREEN_BOTTOM_HEIGHT, SCREEN_BOTTOM_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(mScreenBottom, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
    
    // Configure the first fragment shading substage to just pass through the vertex color
    // See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR);
    C3D_TexEnvFunc(env, C3D_RGB, GPU_MODULATE);
    C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
    C3D_TexEnvColor(env, 0);
    
    mRootScene = std::make_shared<Scene>("Root Scene");
    mRootScene->addSubScene(std::make_shared<DemoScene>("Demo scene"));
    printf("Init complete\n");
}

void Furcadia::update()
{
    gspWaitForVBlank();
    hidScanInput();

    mRootScene->update();
}

void Furcadia::render()
{
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    
    // Left image
    {
        C3D_RenderTargetClear(mScreenLeft, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
        C3D_FrameDrawOn(mScreenLeft);
        mRootScene->renderTop(false);
    }
    
    // Right image
    {
        C3D_RenderTargetClear(mScreenRight, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
        C3D_FrameDrawOn(mScreenRight);
        mRootScene->renderTop(true);
    }
    
    // Bottom image
    {
        C3D_RenderTargetClear(mScreenBottom, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
        C3D_FrameDrawOn(mScreenBottom);
        mRootScene->renderBottom();
    }
    C3D_FrameEnd(0);
    
    // printf("FPS %f; Update time: %f; Render time: %f\n", mFPS, mUpdateTime, mRenderTime);
}

void Furcadia::cleanup()
{
    // Exit in FILO order
    romfsExit();
    C3D_Fini();
    ndspExit();
    gfxExit();
    hidExit();
    socExit();
}

bool Furcadia::shouldRun()
{
    return mRunning && aptMainLoop();
}