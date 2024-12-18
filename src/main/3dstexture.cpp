#include "3dstexture.h"
#include <stdexcept>
#include <vector>
#include <cstring>

void reverse_morton_order(uint8_t* buffer, int width, int height, int bytesPerPixel)
{
    if (width % 8 != 0 || height % 8 != 0) {
        throw std::invalid_argument("Width and height must be multiples of 8.");
    }

    int tilesX = width / 8;
    int tilesY = height / 8;

    // Create a temporary buffer to hold the transformed data
    std::vector<uint8_t> swizzled(width * height * bytesPerPixel);

    // Iterate over each 8x8 tile
    for (int tileX = 0; tileX < tilesX; ++tileX) {
        int pixelX = tileX * 8;

        for (int tileY = 0; tileY < tilesY; ++tileY) {
            int pixelY = tileY * 8;

            // Calculate the tile number
            int tileNum = tileX + tileY * tilesX;

            // Reorder the pixels within this tile
            for (int i = 0; i < 64; ++i) {
                int srcX = pixelX + (i % 8);
                int srcY = height - (pixelY + (i / 8)) - 1; // Flip along the y-axis

                int srcIdx = (srcY * width + srcX) * bytesPerPixel;
                int destIdx = (tileNum * 64 + morton_order[i]) * bytesPerPixel;

                // Copy pixel data and optionally reverse byte order
                for (int j = 0; j < bytesPerPixel; ++j) {
                    swizzled[destIdx + j] = buffer[srcIdx + bytesPerPixel - j - 1];
                }
            }
        }
    }

    // Copy the transformed data back to the original buffer
    std::memcpy(buffer, swizzled.data(), width * height * bytesPerPixel);
}

unsigned int nextPowerOf2(unsigned int n)
{
    if (n == 0) return 1; // Special case: 0 → 1
    n--;                  // Decrement n to handle exact powers of 2
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;         // Works up to 32-bit integers
    return n + 1;         // Add 1 to get the next power of 2
}

uint8_t* padImage(const uint8_t* imageData, uint16_t inputWidth, uint16_t inputHeight, uint8_t bpp,
                                            uint16_t targetWidth, uint16_t targetHeight, uint8_t pad)
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


Texture::Texture(uint8_t* data, uint16_t width, uint16_t height, GPU_TEXCOLOR mode)
{
    int bpp = 3;
    switch (mode)
    {
        case GPU_RGBA8:
            bpp = 4;
            break;
        case GPU_RGB8:
            bpp = 3;
            break;
        case GPU_RGBA5551:
        case GPU_RGB565:
        case GPU_RGBA4:
            bpp = 2;
            break;
        default:
            throw std::runtime_error("Invalid texture mode");
    }
    
    mOriginalWidth = width;
    mOriginalHeight = height;
    
    mWidth = nextPowerOf2(width);
    mHeight = nextPowerOf2(height);
    
    mClip[2] = (float)width / (float)mWidth;
    mClip[3] = (float)height / (float)mHeight;
    
    uint8_t* pixelSrcData = padImage(data, width, height, bpp, mWidth, mHeight, 0);

    reverse_morton_order(pixelSrcData, mWidth, mHeight, bpp);
    C3D_TexInit(&mTexture, mWidth, mHeight, mode);
    C3D_TexUpload(&mTexture, pixelSrcData);
    C3D_TexFlush(&mTexture);

    delete[] pixelSrcData;
}

Texture::Texture(FOX5Image image)
{
    int bpp = 4;
    GPU_TEXCOLOR mode = GPU_RGBA8;
    
    mOriginalWidth = image.mWidth;
    mOriginalHeight = image.mHeight;
    
    mWidth = nextPowerOf2(image.mWidth);
    mHeight = nextPowerOf2(image.mHeight);
    
    mClip[2] = (float)mOriginalWidth / (float)mWidth;
    mClip[3] = (float)mOriginalHeight / (float)mHeight;
    
    uint8_t* pixelSrcData = padImage(image.mData.data(), mOriginalWidth, mOriginalHeight, bpp, mWidth, mHeight, 0xFF);
    
    // Perform swizzling for RGBA8 mode
    if (mode == GPU_RGBA8)
    {
        size_t dataSize = mWidth * mHeight * bpp;
        for (size_t i = 0; i < dataSize; i += 4) // Assuming 4 bytes per pixel
        {
            // This can be simplified
            uint8_t temp1 = pixelSrcData[i];
            uint8_t temp2 = pixelSrcData[i+1];
            uint8_t temp3 = pixelSrcData[i+2];
            uint8_t temp4 = pixelSrcData[i+3];
            pixelSrcData[i] = temp2;
            pixelSrcData[i + 1] = temp3;
            pixelSrcData[i + 2] = temp4;
            pixelSrcData[i + 3] = temp1;
        }
    }
    
    reverse_morton_order(pixelSrcData, mWidth, mHeight, bpp);
    C3D_TexInit(&mTexture, mWidth, mHeight, mode);
    C3D_TexUpload(&mTexture, pixelSrcData);
    C3D_TexFlush(&mTexture);
    delete[] pixelSrcData;
    
    setFilter(GPU_NEAREST, GPU_NEAREST);
    setWrap(GPU_CLAMP_TO_EDGE, GPU_CLAMP_TO_EDGE);
}

Texture::~Texture()
{
    C3D_TexDelete(&mTexture);
}

void Texture::setFilter(GPU_TEXTURE_FILTER_PARAM magFilter, GPU_TEXTURE_FILTER_PARAM minFilter)
{
    C3D_TexSetFilter(&mTexture, magFilter, minFilter);
}

void Texture::setWrap(GPU_TEXTURE_WRAP_PARAM wrapS, GPU_TEXTURE_WRAP_PARAM wrapT)
{
    C3D_TexSetWrap(&mTexture, wrapS, wrapT);
}

void Texture::bind(uint8_t unit)
{
    C3D_TexBind(unit, &mTexture);
}
