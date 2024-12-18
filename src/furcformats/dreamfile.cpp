#include <cstring>
#include <algorithm>
#include "filecommon.h"
#include "dreamfile.h"
#ifdef HAS_CIPHER
    #include "furccipher.h"
#endif

Dream::Dream(const std::string& filename) :
    mFile(filename, std::ios::in | std::ios::binary)
{
    if (!mFile || !mFile.is_open())
        throw std::runtime_error("Failed to open file.");
    mName = getBasename(filename);
    
    std::transform(mName.begin(), mName.end(), mName.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    // Minimal size is calculated by the minimal requirement for a map:
    // "MAP V##.## Furcadia\nBODY\n"
    mFile.seekg(0, std::ios::end);
    if(mFile.tellg() < strlen("MAP V##.## Furcadia\nBODY\n"))
        throw std::runtime_error("Too small to be a Dream file.");
    mFile.seekg(0, std::ios::beg);
    
    char magic[strlen("MAP V##.## Furcadia")] = {0};
    mFile.getline(magic, strlen("MAP V##.## Furcadia\n"));
    
    if(sscanf(magic, "MAP V%d.%d Furcadia", &mVersionMajor, &mVersionMinor) != 2)
    {
        throw std::runtime_error("Not a valid map file.");
    }
    
    bool valid = false;
    // Limit to 255 read attempts
    for(int i = 0; i < 32; i++)
    {
        char line[255] = {0};
        mFile.getline(line, sizeof(line));
        std::string tmp(line);
        
        if(tmp.compare("BODY") == 0)
        {
            valid = true;
            break;
        }
        
        int offset = tmp.rfind("=");
        std::string key = tmp.substr(0, offset);
        std::transform(key.begin(), key.end(), key.begin(),
                    [](unsigned char c) { return std::tolower(c); });
        std::string value = tmp.substr(offset + 1);
        
        if(key.compare("width") == 0) mWidth = std::stoi(value);
        else if(key.compare("height") == 0) mHeight = std::stoi(value);
        else if(key.compare("revision") == 0) mRevision = std::stoi(value);
        else if(key.compare("encoded") == 0) mEncoded = std::stoi(value);
        else if(key.compare("patcht") == 0) mPatcht = std::stoi(value);
        else if(key.compare("sfxlayermode") == 0) mSFXLayerMode = value;
        else if(key.compare("sfxopacity") == 0) mSFXOpacity = std::stoi(value);
        else if(key.compare("name") == 0) mName = value;
        else if(key.compare("patchs") == 0) mPatchs = value;
        else if(key.compare("noload") == 0)  mNoLoad = std::stoi(value);
        else if(key.compare("allowjs") == 0) mAllowJS = std::stoi(value);
        else if(key.compare("allowlf") == 0) mAllowLF = std::stoi(value);
        else if(key.compare("allowfurl") == 0) mAllowFURL = std::stoi(value);
        else if(key.compare("allowshouts") == 0) mAllowShouts = std::stoi(value);
        else if(key.compare("allowlarge") == 0) mAllowLarge = std::stoi(value);
        else if(key.compare("swearfilter") == 0) mSwearFilter = std::stoi(value);
        else if(key.compare("nowho") == 0) mNoWho = std::stoi(value);
        else if(key.compare("forcesittable") == 0) mForceSittable = std::stoi(value);
        else if(key.compare("notab") == 0) mNoTab = std::stoi(value);
        else if(key.compare("nonovelty") == 0) mNoNovelty = std::stoi(value);
        else if(key.compare("rating") == 0) mRating = value;
        else if(key.compare("allow32bitart") == 0) mAllow32BitArt = std::stoi(value);
        else if(key.compare("ismodern") == 0) mIsModern = std::stoi(value);
        else if(key.compare("parentalcontrols") == 0) mParentalControls = std::stoi(value);
        else
            printf("Unknown map key: %s\n", key.c_str());
    }
    
    if(!valid)
        throw std::runtime_error("Map has no body!");
    
    // 1.10 and older use a old verion of the crypto
    bool useOldCrypto = mVersionMajor <= 1 && mVersionMinor <= 10;
    
    int nChannels = 3;
    if(mVersionMajor >= 1 && mVersionMinor >= 30)
        nChannels += 2;
    if(mVersionMajor >= 1 && mVersionMinor >= 50)
        nChannels += 2;
    
    uint32_t expectedBytes = (mWidth * mHeight * 2) * nChannels;
    std::vector<uint8_t> data(expectedBytes, 0);
    
    if(mFile.read(reinterpret_cast<char*>(data.data()), data.size())){}
    else
        throw std::runtime_error("Failed to read entire map file!");
    
    if(mEncoded)
    {
#ifdef HAS_CIPHER
        data = decrypt(data, useOldCrypto);
#else
        throw std::runtime_error("Can't decrypt without furccipher cipher library");
#endif
    }
    mTiles.resize(mWidth * mHeight);
    
    size_t offset = 0;
    // Read floors
    {
        size_t floorSize = mWidth * mHeight * 2; // Size in bytes
        if (offset + floorSize > data.size()) {
            throw std::runtime_error("Not enough data for floors.");
        }

        // Extract the floor data
        std::vector<uint8_t> tmp(data.begin() + offset, data.begin() + offset + floorSize);
        std::vector<uint16_t> floors(mWidth * mHeight, 0);
#ifdef HAS_CIPHER
        if(mEncoded)
            floors = readEncryptedDream16(tmp, mWidth, mHeight, useOldCrypto);
        else
#endif
            for(int i = 0; i < floors.size(); i++)
                floors[i] = tmp[i * 2] | tmp[i * 2 + 1] << 8;
        
        for(int i = 0; i < mTiles.size(); i++)
            mTiles[i].mFloor = floors[i];
        
        offset += floorSize; // Advance offset
    }

    // Read objects
    {
        size_t objectSize = mWidth * mHeight * 2; // Size in bytes
        if (offset + objectSize > data.size()) {
            throw std::runtime_error("Not enough data for objects.");
        }

        // Extract the object data
        std::vector<uint8_t> tmp(data.begin() + offset, data.begin() + offset + objectSize);
        std::vector<uint16_t> objects(mWidth * mHeight, 0);
#ifdef HAS_CIPHER
        if(mEncoded)
            objects = readEncryptedDream16(tmp, mWidth, mHeight, useOldCrypto);
        else
#endif
            for(int i = 0; i < objects.size(); i++)
                objects[i] = tmp[i * 2] | tmp[i * 2 + 1] << 8;
        
        for(int i = 0; i < mTiles.size(); i++)
            mTiles[i].mObject = objects[i];
        
        offset += objectSize; // Advance offset
    }

    // Read walls
    {
        size_t wallSize = mWidth * mHeight * 2; // Size in bytes (example, adjust as needed)
        if (offset + wallSize > data.size()) {
            throw std::runtime_error("Not enough data for walls.");
        }

        // Extract the wall data
        std::vector<uint8_t> tmp(data.begin() + offset, data.begin() + offset + wallSize);
        std::vector<uint8_t> walls(mWidth * mHeight * 2, 0);
        if(mEncoded)
#ifdef HAS_CIPHER
            walls = readEncryptedDream8(tmp, mWidth, mHeight, useOldCrypto);
        else
#endif
            for(int i = 0; i < walls.size(); i++)
                walls[i] = tmp[i];
        
        for(int i = 0; i < mTiles.size(); i++)
        {
            mTiles[i].mNEWall = walls[i * 2];
            mTiles[i].mNWWall = walls[i * 2 + 1];
        }
        
        offset += wallSize; // Advance offset
    }
    
    if(mVersionMajor >= 1 && mVersionMinor >= 30)
    {
        // Read regions
        {
            size_t regionSize = mWidth * mHeight * 2; // Size in bytes
            if (offset + regionSize > data.size()) {
                throw std::runtime_error("Not enough data for regions.");
            }

            // Extract the region data
            std::vector<uint8_t> tmp(data.begin() + offset, data.begin() + offset + regionSize);
            std::vector<uint16_t> regions(mWidth * mHeight, 0);
            if(mEncoded)
#ifdef HAS_CIPHER
                regions = readEncryptedDream16(tmp, mWidth, mHeight, useOldCrypto);
            else
#endif
                for(int i = 0; i < regions.size(); i++)
                    regions[i] = tmp[i * 2] | tmp[i * 2 + 1] << 8;
            
            for(int i = 0; i < mTiles.size(); i++)
                mTiles[i].mRegion = regions[i];
            
            offset += regionSize; // Advance offset
        }
        
        // Read effects
        {
            size_t effectSize = mWidth * mHeight * 2; // Size in bytes
            if (offset + effectSize > data.size()) {
                throw std::runtime_error("Not enough data for effects.");
            }

            // Extract the region data
            std::vector<uint8_t> tmp(data.begin() + offset, data.begin() + offset + effectSize);
            std::vector<uint16_t> effects(mWidth * mHeight, 0);
            if(mEncoded)
#ifdef HAS_CIPHER
                effects = readEncryptedDream16(tmp, mWidth, mHeight, useOldCrypto);
            else
#endif
                for(int i = 0; i < effects.size(); i++)
                    effects[i] = tmp[i * 2] | tmp[i * 2 + 1] << 8;
            
            for(int i = 0; i < mTiles.size(); i++)
                mTiles[i].mEffect = effects[i];
            
            offset += effectSize; // Advance offset
        }
    }
    
    if(mVersionMajor >= 1 && mVersionMinor >= 50)
    {
        // Read lighting
        {
            size_t lightingSize = mWidth * mHeight * 2; // Size in bytes
            if (offset + lightingSize > data.size()) {
                throw std::runtime_error("Not enough data for lighting.");
            }

            // Extract the lighting data
            std::vector<uint8_t> tmp(data.begin() + offset, data.begin() + offset + lightingSize);
            std::vector<uint16_t> lighting(mWidth * mHeight, 0);
#ifdef HAS_CIPHER
            if(mEncoded)
                lighting = readEncryptedDream16(tmp, mWidth, mHeight, useOldCrypto);
            else
#endif
                for(int i = 0; i < lighting.size(); i++)
                    lighting[i] = tmp[i * 2] | tmp[i * 2 + 1] << 8;
            
            for(int i = 0; i < mTiles.size(); i++)
                mTiles[i].mLighting = lighting[i];
            
            offset += lightingSize; // Advance offset
        }
        
        // Read ambient
        {
            size_t ambientSize = mWidth * mHeight * 2; // Size in bytes
            if (offset + ambientSize > data.size()) {
                throw std::runtime_error("Not enough data for ambient.");
            }

            // Extract the ambient data
            std::vector<uint8_t> tmp(data.begin() + offset, data.begin() + offset + ambientSize);
            std::vector<uint16_t> ambient(mWidth * mHeight, 0);
            if(mEncoded)
#ifdef HAS_CIPHER
                ambient = readEncryptedDream16(tmp, mWidth, mHeight, useOldCrypto);
            else
#endif
                for(int i = 0; i < ambient.size(); i++)
                    ambient[i] = tmp[i * 2] | tmp[i * 2 + 1] << 8;
            
            for(int i = 0; i < mTiles.size(); i++)
                mTiles[i].mAmbient = ambient[i];
            
            offset += ambientSize; // Advance offset
        }
    }
    
    data.resize(0); // Free up space
}

Dream::~Dream()
{
    if(mFile && mFile.is_open())
    {
        mFile.close();
    }
}


DreamTile_t* Dream::get(uint16_t x, uint16_t y)
{
    if (x >= mWidth || y >= mHeight)
    {
        throw std::out_of_range("Coordinates out of bounds");
    }
    size_t index = (mHeight * x) + y;
    return &mTiles[index];
}