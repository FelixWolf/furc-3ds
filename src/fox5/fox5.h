#ifndef FOX5_H
#define FOX5_H
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <variant>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>


class FOX5Command
{
public:
    enum class Command : uint8_t
    {
        // Shared
        NOP = 0x00,
        LIST_START = 0x4C, // 'L'
        LIST_END = 0x3C, // '<'
        
        // File
        FILE_GENERATOR = 0x67, // 'g'
        FILE_IMAGE_LIST = 0x53, // 'S'
        
        // Object
        OBJECT_AUTHOR_REVISION = 0x72, // 'r'
        OBJECT_AUTHORS = 0x61, // 'a'
        OBJECT_AUTHORS_HASH = 0x68, // 'h'
        OBJECT_LICENSE = 0x6C, // 'l'
        OBJECT_KEYWORDS = 0x6B, // 'k'
        OBJECT_NAME = 0x6E, // 'n'
        OBJECT_DESCRIPTION = 0x64, // 'd'
        OBJECT_FLAGS = 0x21, // '!'
        OBJECT_URI = 0x50, // 'P'
        OBJECT_MORE_FLAGS = 0x3F, // '?'
        OBJECT_IDENTIFIER = 0x69, // 'i'
        OBJECT_EDIT_TYPE = 0x74, // 't'
        OBJECT_FILTER = 0x46, // 'F'
        
        // Shape
        SHAPE_PURPOSE = 0x70, // 'p'
        SHAPE_STATE = 0x73, // 's'
        SHAPE_DIRECTION = 0x44, // 'D'
        SHAPE_RATIO = 0x52, // 'R'
        SHAPE_KITTERSPEAK = 0x4B, // 'K'
        
        // Frame
        FRAME_OFFSET = 0x6F, // 'o'
        FRAME_FURRE_OFFSET = 0x66, // 'f'
        FRAME_ATTACH_PLUGS = 0x54, // 'T'
        FRAME_ATTACH_SOCKETS = 0x55, // 'U'
        
        // Sprite
        CHANNEL_PURPOSE = 0x43, // 'C'
        CHANNEL_IMAGE_ID = 0x63, // 'c'
        CHANNEL_OFFSET = 0x4F // 'O'
    };
    
    Command mCommand;
    
    enum class Type
    {
        UInt8,
        Int8,
        UInt16,
        Int16,
        UInt32,
        Int32,
        String,
        Bytes
    };
    using FOX5Value = std::variant<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, std::string, std::vector<uint8_t>>;

    struct Value {
        Type mType;
        FOX5Value mValue;

        // Utility methods to simplify type access
        template <typename T>
        T& get() {
            // Ensure the type matches before accessing
            if (auto* ptr = std::get_if<T>(&mValue)) {
                return *ptr;
            }
            throw std::bad_variant_access();  // Throw exception if type doesn't match
        }

        template <typename T>
        const T& get() const {
            // Ensure the type matches before accessing
            if (auto* ptr = std::get_if<T>(&mValue)) {
                return *ptr;
            }
            throw std::bad_variant_access();  // Throw exception if type doesn't match
        }
    };
    
    std::vector<Value> mValues;
    void addValue(Type type, const FOX5Value& v)
    {
        mValues.push_back({type, v});
    };
    
    void reserveValues(size_t n) {
        mValues.reserve(mValues.size() + n);
    };
    
    const std::vector<Value>& getValues() const
    {
        return mValues;
    };
    
    template <typename T>
    const Value& getValue(size_t index) const
    {
        return std::get<T>(mValues[index].mValue);
    }
    
    size_t mIndex = 0;
    template <typename T>
    const T& getValue() {
        if (auto* ptr = std::get_if<T>(&mValues[mIndex++].mValue)) {
            return *ptr;
        }
        throw std::bad_variant_access();  // Safely handle wrong type access
    }
    
    void parseData(uint8_t** dataPtr, uint8_t* dataEnd);
    FOX5Command(uint8_t** dataPtr, uint8_t* dataEnd)
    {
        parseData(dataPtr, dataEnd);
    };
};

class FOX5Image
{
public:
    uint32_t mOffset;
    uint32_t mCompressedSize;
    uint16_t mWidth;
    uint16_t mHeight;
    std::vector<uint8_t> mData;
    
    enum class ImageFormat : uint8_t
    {
        E_8BIT = 0,
        E_32BIT = 1
    };
    ImageFormat mImageFormat;
    
    uint32_t getMemSize()
    {
        uint32_t s = mWidth*mHeight;
        if(mImageFormat == ImageFormat::E_32BIT)
            s *= 4;
        return s;
    }
    
    FOX5Image(uint32_t offset, uint32_t compressedSize, uint16_t width, uint16_t height, ImageFormat format)
        : mOffset(offset), mCompressedSize(compressedSize), mWidth(width), mHeight(height), mImageFormat(format)
    {}
};

class FOX5List
{
public:
    FOX5List() = default;
    FOX5List(const std::vector<uint8_t> data){};
};


class FOX5Channel : FOX5List
{
public:
    union
    {
        uint16_t mPurpose;
        struct
        {
            uint8_t : 5;
            bool mRemap : 1;
            bool mShadow : 1;
            bool mMarkup : 1;
        };
    };
    
    uint16_t mImageID; //Number in FOX5File ImageList
    int16_t mOffset[2] = {0};
    
    void parseData(uint8_t** dataPtr, uint8_t* dataEnd);
    FOX5Channel(uint8_t** dataPtr, uint8_t* dataEnd)
    {
        parseData(dataPtr, dataEnd);
    };
};

class FOX5Frame : FOX5List
{
public:
    int16_t mFrameOffset[2] = {0};
    int16_t mFurreOffset[2] = {0};
    
    std::vector<std::shared_ptr<FOX5Channel>> mSprites;
    
    void parseData(uint8_t** dataPtr, uint8_t* dataEnd);
    FOX5Frame(uint8_t** dataPtr, uint8_t* dataEnd)
    {
        parseData(dataPtr, dataEnd);
    };
};

class FOX5Shape : FOX5List
{
public:
    enum class Purpose : uint8_t
    {
        UNSPECIFIED = 0,
        MENU_ICON = 1,
        UI_BUTTON = 2,
        BUTLER = 3,
        PORTRAIT = 4,
        DS_BUTTON = 5,
        
        AVATAR = 11,
        
        FLOOR = 21,
        ITEM = 22,
        WALL = 23,
        REGION = 24,
        EFFECT = 25,
        PAD_ITEM = 28,
        PORTAL_ITEM = 29,
        
        SPECITAG = 35,
        
        LIGHTING = 41,
        AMBIENCE = 42
    };
    Purpose mPurpose;
    
    union
    {
        uint8_t mState;
        struct
        {
            bool mFemale : 1;
            bool mMale : 1;
            bool mUnspecified : 1;
        };
        struct
        {
            bool mClicked : 1;
            bool mMouseOver : 1;
            bool mActivated : 1;
        };
    };
    
    enum class Direction : uint8_t
    {
        UNSPECIFIED = 0,
        SW = 1,
        S = 2,
        SE = 3,
        W = 4,
        NO_DIRECTION = 5,
        E = 6,
        NW = 7,
        LEFT = 7,
        N = 8,
        NE = 9,
        RIGHT = 9,
        UP = 10,
        DOWN = 11
    };
    Direction mDirection;
    
    uint8_t mRatio[2] = {0,0};
    
    struct Kitterspeak {
        uint16_t mCommand;
        int16_t mArg1;
        int16_t mArg2;
        Kitterspeak(uint16_t command, int16_t arg1, int16_t arg2)
            : mCommand(command), mArg1(arg1), mArg2(arg2) {}
    };
    
    std::vector<std::unique_ptr<Kitterspeak>> mKitterspeak;
    
    std::vector<std::shared_ptr<FOX5Frame>> mFrames;
    
    void parseData(uint8_t** dataPtr, uint8_t* dataEnd);
    FOX5Shape(uint8_t** dataPtr, uint8_t* dataEnd)
    {
        parseData(dataPtr, dataEnd);
    };
};

class FOX5Object : FOX5List
{
public:
    uint32_t mAuthorRevision;
    std::vector<std::string> mAuthors;
    enum class License : uint8_t
    {
        STANDARD = 0,
        FREEDOM = 1,
        LIMITED = 2,
        EXCLUSIVE = 3,
        PRIVATE = 4,
        CONDITIONAL = 5
    };
    License mLicense;
    std::vector<std::string> mKeywords;
    std::string mName;
    std::string mDescription;
    
    union
    {
        uint8_t mFlags;
        struct
        {
            bool mWalkable : 1;
            bool mGettable : 1;
            bool mSittable : 1;
            bool mFlyable : 1;
            bool mSwimmable : 1;
            bool mClickable : 1;
            bool mMouseOver : 1;
            bool mKickable : 1;
        };
    };
    std::string mURI;
    union
    {
        uint32_t mMoreFlags;
        struct
        {
            bool mDreamPadAll : 1;
            bool mDreamPadSS : 1;
            bool mDreamPadGS : 1;
            bool mDreamPadLG : 1;
            bool mDreamPadHG : 1;
            bool mDreamPadDEP : 1;
        };
        struct
        {
            bool mHopping : 1;
            bool mFlying : 1;
            bool mSwimming : 1;
            bool mChild : 1;
        };
    };
    int32_t mObjectID;
    uint8_t mEditType;
    uint8_t mFilterTarget;
    uint8_t mFilterMode;
    
    std::vector<std::shared_ptr<FOX5Shape>> mShapes;
    
    void parseData(uint8_t** dataPtr, uint8_t* dataEnd);
    FOX5Object(uint8_t** dataPtr, uint8_t* dataEnd)
    {
        parseData(dataPtr, dataEnd);
    };
};

class FOX5File : FOX5List
{
protected:
    std::ifstream mFile;

public: // Footer
    uint8_t mSeed[16] = {0};
    
    enum class CompressionType : uint8_t
    {
        NOT = 0,
        ZLIB = 1,
        LZMA = 2
    };
    CompressionType mCompressionType;
    
    enum class EncryptionType : uint8_t
    {
        NOT = 0,
        ENCRYPTED = 1
    };
    EncryptionType mEncryptionType;
    
public: // FOX5List
    
    uint8_t mGenerator;
    size_t mImageStart;
    std::vector<std::shared_ptr<FOX5Image>> mImageList;
    std::vector<std::shared_ptr<FOX5Object>> mObjects;
    
public:
    FOX5Image getImage(uint32_t id);
public:
    FOX5File(const std::string& filename);
    
    void parseData(uint8_t** dataPtr, uint8_t* dataEnd);
};

#endif