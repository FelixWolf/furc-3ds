#include "texturecache.h"

std::shared_ptr<Texture> TextureCache::getFromFox(FOX5File& fox, uint32_t ptr)
{
    auto itOuter = mTextureMap.find(fox.mName); // Check if 'name' exists in the outer map
    if (itOuter != mTextureMap.end()) {
        auto itInner = itOuter->second.find(ptr); // Check if 'ptr' exists in the inner map
        if (itInner != itOuter->second.end()) {
            // Entry exists
            itInner->second.mLastUse = mCurrentAge;
            return itInner->second.mTexture; // Return shared_ptr directly
        }
    }
    
    FOX5Image image = fox.getImage(ptr);

    // Create a new Texture object as a shared pointer
    std::shared_ptr<Texture> texture = std::make_shared<Texture>(image);

    // Insert the new entry into the map
    TextureEntry newEntry;
    newEntry.mTexture = texture;
    newEntry.mLastUse = mCurrentAge;

    mTextureMap[fox.mName].emplace(ptr, newEntry);

    // Return the shared pointer
    return texture;
}