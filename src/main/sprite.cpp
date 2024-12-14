#include "sprite.h"
#include <cstring>  // for memcpy
#include <3ds.h>    // Assuming you're using 3ds libraries for graphics

// Define the static member outside the class definition
void* Sprite::mVBOData = nullptr;
const vertex Sprite::mVertexList[] = {
    { { 0.f, 0.f, 0.f }, { 0.f, 1.f } },
    { { 1.f, 0.f, 0.f }, { 1.f, 1.f } },
    { { 0.f, 1.f, 0.f }, { 0.f, 0.f } },
    
    { { 1.f, 0.f, 0.f }, { 1.f, 1.f } },
    { { 1.f, 1.f, 0.f }, { 1.f, 0.f } },
    { { 0.f, 1.f, 0.f }, { 0.f, 0.f } }
};
const size_t Sprite::mVertexListCount = sizeof(Sprite::mVertexList) / sizeof(Sprite::mVertexList[0]);

// Constructor of Sprite class
Sprite::Sprite(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture)
{
    // Initialize vbo_data only once
    if (!mVBOData)
    {
        mVBOData = linearAlloc(sizeof(mVertexList));  // Now sizeof() works correctly
        memcpy(mVBOData, mVertexList, sizeof(mVertexList));  // Copy vertex list data to vbo_data

        // Configure buffers (done only once)
        C3D_BufInfo* bufInfo = C3D_GetBufInfo();
        BufInfo_Init(bufInfo);
        BufInfo_Add(bufInfo, mVBOData, sizeof(vertex), 2, 0x210);
    }
    setShader(shader);
    setTexture(texture);
}

void Sprite::setTexture(std::shared_ptr<Texture> texture, bool resize)
{
    mTexture = texture;
    mSize[0] = texture->mOriginalWidth;
    mSize[1] = texture->mOriginalHeight;
}

void Sprite::setShader(std::shared_ptr<Shader> shader)
{
    mShader = shader;
    mULoc_Projection = shader->mVertexUniforms["projection"];
    mULoc_ModelView = shader->mVertexUniforms["modelView"];
}

// Implement the draw method as needed
void Sprite::draw(const C3D_Mtx& projection)
{
    mShader->bind();
    mTexture->bind(0);
    
    C3D_Mtx modelView;
    Mtx_Identity(&modelView);
    Mtx_Translate(&modelView,
        mPosition[0] + mOffset[0],
        -(mPosition[1] + mOffset[1]),
        0,
        false
    );
    Mtx_Scale(&modelView, mSize[0], mSize[1], 0);
    
    C3D_FixedAttribSet(2, mColor[0], mColor[1], mColor[2], mColor[3]);
    C3D_FixedAttribSet(3,
        mTexture->mClip[0] + mClip[0],
        mTexture->mClip[1] + mClip[1],
        mTexture->mClip[2] + mClip[2],
        mTexture->mClip[3] + mClip[3]
    );
    
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, mULoc_Projection, &projection);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, mULoc_ModelView, &modelView);
    C3D_DrawArrays(GPU_TRIANGLES, 0, mVertexListCount);
}