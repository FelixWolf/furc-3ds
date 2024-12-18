#ifndef SPRITE_H
#define SPRITE_H
#include "3dstexture.h"
#include "3dsshader.h"
#include "texturecache.h"

typedef struct {
    float position[3];
    float uv[2];
} vertex;

class Sprite
{
public:
    Sprite(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture);
    
    ~Sprite() = default;
    
    void draw(C3D_Mtx* projection);
    
    std::shared_ptr<Shader> mShader;
    uint8_t mULoc_ModelView;
    uint8_t mULoc_Projection;
    void setShader(std::shared_ptr<Shader> shader);
    
    std::shared_ptr<Texture> mTexture;
    void setTexture(std::shared_ptr<Texture> texture, bool resize = true);
    float mPosition[2] = {0};
    float mOffset[2] = {0};
    float mSize[2] = {0};
    float mColor[4] = {1.0, 1.0, 1.0, 1.0};
    float mClip[4] = {0, 0, 1.0, 1.0};
    float mDepth = 0.0;
    
private:
    static void* mVBOData;
    static const vertex mVertexList[];  // Declare as an array of const vertex
    static const size_t mVertexListCount;
};

#endif // SPRITE_H