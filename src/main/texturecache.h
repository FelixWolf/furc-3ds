#ifndef TEXTURECACHE_H
#define TEXTURECACHE_H
#include <unordered_map>
#include <string>
#include "singleton.h"
#include "fox5.h"
#include "3dstexture.h"

class TextureCache : public Singleton<TextureCache>
{
public:
    struct TextureEntry
    {
        std::shared_ptr<Texture> mTexture;
        uint32_t mLastUse = 0;
    };
    std::unordered_map<std::string, std::unordered_map<uint32_t, TextureEntry>> mTextureMap;
    
    uint32_t mMaxAge = 16;
    uint32_t mCurrentAge;
    
    void clearAll();
    void shift(uint32_t since);
    void shift()
    {
        shift(mMaxAge);
    };
    
    std::shared_ptr<Texture> getFromFox(FOX5& fox, uint32_t ptr);
};

#endif // TEXTURECACHE_H