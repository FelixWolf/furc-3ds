#ifndef _3DSTEXTURE_H_
#define _3DSTEXTURE_H_
#include <cstdint>
#include <citro3d.h>
#include "fox5.h"

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

void reverse_morton_order(uint8_t* buffer, int width, int height, int bytesPerPixel);
unsigned int nextPowerOf2(unsigned int n);
uint8_t* padImage(const uint8_t* imageData, uint16_t inputWidth, uint16_t inputHeight, uint8_t bpp,
                                            uint16_t targetWidth, uint16_t targetHeight, uint8_t pad = 0);
void uploadTexture(C3D_Tex* texture, uint8_t* data, uint16_t width, uint16_t height, GPU_TEXCOLOR mode);

class Texture
{
public:
    C3D_Tex mTexture;
    uint16_t mWidth;
    uint16_t mHeight;
    uint16_t mOriginalWidth;
    uint16_t mOriginalHeight;
    float mClip[4] = {0};
    u8 *mGPUSrc;
    
    Texture(uint8_t* data, uint16_t width, uint16_t height, GPU_TEXCOLOR mode);
    Texture(FOX5Image image);
    ~Texture();
    
    void setFilter(GPU_TEXTURE_FILTER_PARAM magFilter, GPU_TEXTURE_FILTER_PARAM minFilter);
    void setWrap(GPU_TEXTURE_WRAP_PARAM wrapS, GPU_TEXTURE_WRAP_PARAM wrapT);
    void bind(uint8_t unit);
};

#endif //_3DSTEXTURE_H_