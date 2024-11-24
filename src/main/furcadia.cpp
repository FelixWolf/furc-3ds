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

#include "unlit_generic.shbin.h"

#include "furcadia.h"
#include "fox5.h"
#include "testimg.h"

// Function to convert a buffer to Morton order with variable bytes per pixel
void reverse_morton_order(uint8_t* buffer, int width, int height, int bytesPerPixel)
{
    if (width % 8 != 0 || height % 8 != 0)
        throw std::invalid_argument("Width and height must be multiples of 8.");

    int tilesX = width / 8;
    int tilesY = height / 8;

    // Create a temporary buffer to hold the transformed data
    std::vector<uint8_t> result(width * height * bytesPerPixel);

    // Iterate over each 8x8 tile
    for (int tileX = 0; tileX < tilesX; ++tileX)
    {
        int pixelX = tileX * 8;

        for (int tileY = 0; tileY < tilesY; ++tileY)
        {
            int pixelY = tileY * 8;

            // Calculate the tile number
            int tileNum = tileX + tileY * tilesX;

            // Reorder the pixels within this tile
            for (int i = 0; i < 64; ++i)
            {
                int srcX = pixelX + (i % 8);
                int srcY = height - (pixelY + (i / 8)) - 1; // Flip along the y-axis

                int srcIdx = (srcY * width + srcX) * bytesPerPixel;
                int destIdx = (tileNum * 64 + morton_order[i]) * bytesPerPixel;

                // Copy pixel data and optionally reverse byte order
                for (int j = 0; j < bytesPerPixel; ++j)
                    result[destIdx + j] = buffer[srcIdx + bytesPerPixel - j - 1];
            }
        }
    }

    // Copy the transformed data back to the original buffer
    std::memcpy(buffer, result.data(), width * height * bytesPerPixel);
}

uint8_t* padImage(const uint8_t* imageData, uint16_t inputWidth, uint16_t inputHeight, uint8_t bpp,
                                            uint16_t targetWidth, uint16_t targetHeight, uint8_t pad = 0)
{
    // Validate input dimensions
    if (inputWidth > targetWidth || inputHeight > targetHeight) {
        throw std::invalid_argument("Target dimensions must be greater than or equal to input dimensions.");
    }
    
    // Calculate sizes
    int inputRowBytes = inputWidth * bpp; // Bytes per row in input image
    int targetRowBytes = targetWidth * bpp; // Bytes per row in padded image
    int paddedImageSize = targetHeight * targetRowBytes;
    
    // Allocate buffer for the padded image
    uint8_t* paddedImage = new uint8_t[paddedImageSize];

    // Fill the entire buffer with the padding value
    std::memset(paddedImage, pad, paddedImageSize);

    // Copy the input image into the padded buffer row by row
    for (uint16_t y = 0; y < inputHeight; ++y) {
        const uint8_t* srcRow = imageData + y * inputRowBytes; // Source row in input image
        uint8_t* destRow = paddedImage + y * targetRowBytes;  // Destination row in padded image
        std::memcpy(destRow, srcRow, inputRowBytes);          // Copy the row
    }

    return paddedImage; // Caller must free this buffer when done
}

unsigned int nextPowerOf2(unsigned int n) {
    if (n == 0) return 1; // Special case: 0 → 1
    n--;                  // Decrement n to handle exact powers of 2
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;         // Works up to 32-bit integers
    return n + 1;         // Add 1 to get the next power of 2
}

static void* vbo_data;
static int uLoc_projection;
static int uLoc_modelView;

// Function to load and parse shader from file
DVLB_s* loadShader(const std::string& path) {
    // Open the file
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return nullptr;
    }

    // Get file size
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allocate buffer and read file contents
    std::vector<u8> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Failed to read shader file: " << path << std::endl;
        return nullptr;
    }

    // Pass buffer to DVLB_ParseFile
    DVLB_s* vshader_dvlb = DVLB_ParseFile(reinterpret_cast<u32*>(buffer.data()), static_cast<size_t>(size));
    if (!vshader_dvlb) {
        std::cerr << "Failed to parse shader: " << path << std::endl;
    }

    return vshader_dvlb;
}
C3D_Tex texture;

void Furcadia::initialize()
{
    SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

    if(SOC_buffer == NULL)
        throw std::runtime_error("Failed to allocate SOC buffer");

    // Now intialise soc:u service
    if(socInit(SOC_buffer, SOC_BUFFERSIZE) != 0)
        throw std::runtime_error("Failed to initialize SOC service");

    link3dsStdio();
    
    // Initialize libctru
    gfxInitDefault();
    gfxSetDoubleBuffering(GFX_TOP, true);
    gfxSetDoubleBuffering(GFX_BOTTOM, true);
    
    hidInit();
    
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    
    // Initialize ROMFS
    Result rc = romfsInit();
    if (rc)
        throw std::runtime_error("romfsInit failed: " + std::to_string(rc));
    
    gfxSet3D(true);
    
    mScreenLeft  = C3D_RenderTargetCreate(SCREEN_TOP_HEIGHT, SCREEN_TOP_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(mScreenLeft,  GFX_TOP, GFX_LEFT,  DISPLAY_TRANSFER_FLAGS);
    
    mScreenRight = C3D_RenderTargetCreate(SCREEN_TOP_HEIGHT, SCREEN_TOP_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(mScreenRight, GFX_TOP, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);
    
    mScreenBottom = C3D_RenderTargetCreate(SCREEN_BOTTOM_HEIGHT, SCREEN_BOTTOM_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(mScreenBottom, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
    
    mUnlitGenericDLVB = loadShader("romfs:/shaders/unlit_generic.shbin");
    shaderProgramInit(&mUnlitGenericProgram);
    shaderProgramSetVsh(&mUnlitGenericProgram, &mUnlitGenericDLVB->DVLE[0]);
    C3D_BindProgram(&mUnlitGenericProgram);
    
    uLoc_projection = shaderInstanceGetUniformLocation(mUnlitGenericProgram.vertexShader, "projection");
    uLoc_modelView  = shaderInstanceGetUniformLocation(mUnlitGenericProgram.vertexShader, "modelView");
    
    // Configure attributes for use with the vertex shader
    C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1=texcoords
    AttrInfo_AddFixed(attrInfo, 2); // v2=color

    // Set the fixed attribute (color) to solid white
    C3D_FixedAttribSet(2, 1.0, 1.0, 1.0, 1.0);

    // Create the VBO (vertex buffer object)
    vbo_data = linearAlloc(sizeof(vertex_list));
    memcpy(vbo_data, vertex_list, sizeof(vertex_list));

    // Configure buffers
    C3D_BufInfo* bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, vbo_data, sizeof(vertex), 2, 0x210);
    /*
    int totalBytes = gimp_image.width * gimp_image.height * gimp_image.bytes_per_pixel;
    uint8_t* pixelData = new uint8_t[totalBytes];
    memcpy(pixelData, gimp_image.pixel_data, totalBytes);
    
    reverse_morton_order(pixelData, gimp_image.width, gimp_image.height, gimp_image.bytes_per_pixel);
    */
    /*
    uint16_t imageWidth = nextPowerOf2(image.mWidth);
    uint16_t imageHeight = nextPowerOf2(image.mHeight);
    */
    
    FOX5File data = FOX5File("romfs:/3ds/system.fox");
    
    int totalBytes = gimp_image.width * gimp_image.height * gimp_image.bytes_per_pixel;
    uint8_t* pixelSrcData = new uint8_t[totalBytes];
    memcpy(pixelSrcData, gimp_image.pixel_data, totalBytes);
    
    C3D_TexInit(&texture, 128, 128, GPU_RGB8);
    uint8_t* pixelData = padImage(pixelSrcData, gimp_image.width, gimp_image.height, 3, 128, 128, 0);
    
    reverse_morton_order(pixelData, 128, 128, 3);
    C3D_TexUpload(&texture, pixelData);
    
    C3D_TexSetFilter(&texture, GPU_LINEAR, GPU_NEAREST);
    C3D_TexSetWrap(&texture, GPU_CLAMP_TO_EDGE, GPU_CLAMP_TO_EDGE);
    C3D_TexBind(0, &texture);
    delete[] pixelSrcData;
    delete[] pixelData;
    
    // Configure the first fragment shading substage to just pass through the vertex color
    // See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR);
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
}

void Furcadia::update()
{
    gspWaitForVBlank();
    hidScanInput();

    u32 kDown = hidKeysDown();
    if (kDown & KEY_START)
        return;
}

void Furcadia::render()
{
    C3D_Mtx modelView;
    Mtx_Identity(&modelView);
    Mtx_Translate(&modelView, (float)(mTick++), 0, 0, false);
    Mtx_Scale(&modelView, 128, 128, 0);
    C3D_FixedAttribSet(1, 1.0, 1.0, 1.0, 1.0);
    
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    
    C3D_Mtx projection;
    Mtx_OrthoTilt(&projection,    0.0, 400.0, 0.0, 240.0, -10.0, 10.0, false);
    // Left image
    {
        C3D_RenderTargetClear(mScreenLeft, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
        C3D_FrameDrawOn(mScreenLeft);
        // Update the uniforms
        C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);
        C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER,   uLoc_modelView,      &modelView);
        // Draw the VBO
        C3D_DrawArrays(GPU_TRIANGLES, 0, vertex_list_count);
    }
    
    // Right image
    {
        C3D_RenderTargetClear(mScreenRight, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
        C3D_FrameDrawOn(mScreenRight);
        // Update the uniforms
        C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);
        C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER,   uLoc_modelView,      &modelView);
        // Draw the VBO
        C3D_DrawArrays(GPU_TRIANGLES, 0, vertex_list_count);
    }
    
    Mtx_OrthoTilt(&projection, 0.0, 320.0, 0.0, 240.0, -10.0, 10.0, false);
    // Bottom image
    {
        C3D_RenderTargetClear(mScreenBottom, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
        C3D_FrameDrawOn(mScreenBottom);
        // Update the uniforms
        C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);
        C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER,   uLoc_modelView,      &modelView);
        // Draw the VBO
        C3D_DrawArrays(GPU_TRIANGLES, 0, vertex_list_count);
    }
    C3D_FrameEnd(0);
    
    printf("FPS %f; Update time: %f; Render time: %f\n", mFPS, mUpdateTime, mRenderTime);
}

void Furcadia::cleanup()
{
    
    shaderProgramFree(&mUnlitGenericProgram);
    DVLB_Free(mUnlitGenericDLVB);
    
    C3D_Fini();
    
    romfsExit();
    hidExit();
    gfxExit();
    socExit();
}

bool Furcadia::shouldRun()
{
    return mRunning && aptMainLoop();
}