#ifndef DREAMFILE_H
#define DREAMFILE_H
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <variant>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>


struct DreamTile_t
{
    uint16_t mFloor = 0;
    uint16_t mObject = 0;
    uint8_t mNEWall = 0;
    uint8_t mNWWall = 0;
    uint16_t mRegion = 0;
    uint16_t mEffect = 0;
    uint16_t mLighting = 0;
    uint16_t mAmbient = 0;
};

class Dream
{
protected:
    std::ifstream mFile;

public: // Footer
    std::string mFileName;
    
public: // Dream attributes
    uint8_t mVersionMajor;
    uint8_t mVersionMinor;
    
    uint16_t mWidth = 52;
    uint16_t mHeight = 100;
    uint32_t mRevision = 0;
    bool mEncoded = false;
    bool mPatcht = false;
    std::string mSFXLayerMode;
    uint8_t mSFXOpacity = 0;
    std::string mName;
    std::string mPatchs;
    bool mNoLoad = false;
    bool mAllowJS = false;
    bool mAllowLF = false;
    bool mAllowFURL = false;
    bool mAllowShouts = false;
    bool mAllowLarge = false;
    uint8_t mSwearFilter = 0;
    bool mNoWho = false;
    bool mForceSittable = false;
    bool mNoTab = false;
    bool mNoNovelty = false;
    std::string mRating;
    bool mAllow32BitArt = false;
    bool mIsModern = false;
    bool mParentalControls = false;
    
    std::vector<DreamTile_t> mTiles;
    
public:
    Dream(const std::string& filename);
    ~Dream();
    
    DreamTile_t* get(uint16_t x, uint16_t y);
};

#endif