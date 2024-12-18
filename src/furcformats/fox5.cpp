#include <cstring>
#include <algorithm>
#include "filecommon.h"
#include "fox5.h"

#ifdef HAVE_PROPRIETARY
    #include "fox5cipher.h"
    #define HAS_CIPHER
#endif

void FOX5Command::parseData(uint8_t** dataPtr, uint8_t* dataEnd)
{
    uint8_t cmd = readUint8(dataPtr, dataEnd);
    mCommand = static_cast<FOX5Command::Command>(cmd);
    switch(mCommand)
    {
        // Shared
        case FOX5Command::Command::NOP:
            break;
        
        case FOX5Command::Command::LIST_START:
            reserveValues(2);
            addValue(FOX5Command::Type::UInt8, static_cast<uint8_t>(readUint8(dataPtr, dataEnd)));
            addValue(FOX5Command::Type::UInt32, static_cast<uint32_t>(readUint32(dataPtr, dataEnd)));
            break;
        
        case FOX5Command::Command::LIST_END:
            break;
        
        // File
        case FOX5Command::Command::FILE_IMAGE_LIST:
        {
            size_t count = readUint32(dataPtr, dataEnd);
            
            reserveValues(1 + (count * 4));
            addValue(FOX5Command::Type::UInt32, static_cast<uint32_t>(count));
            for(uint32_t i = 0; i < count; i++)
            {
                addValue(FOX5Command::Type::UInt32, static_cast<uint32_t>(readUint32(dataPtr, dataEnd)));
                addValue(FOX5Command::Type::UInt16, static_cast<uint16_t>(readUint16(dataPtr, dataEnd)));
                addValue(FOX5Command::Type::UInt16, static_cast<uint16_t>(readUint16(dataPtr, dataEnd)));
                addValue(FOX5Command::Type::UInt8,  static_cast<uint8_t>(readUint8(dataPtr, dataEnd)));
            }
            break;
        }
        
        case FOX5Command::Command::FILE_GENERATOR:
        {
            addValue(FOX5Command::Type::UInt8,  static_cast<uint8_t>(readUint8(dataPtr, dataEnd)));
            break;
        }
        
        // Object
        case FOX5Command::Command::OBJECT_AUTHOR_REVISION:
        {
            addValue(FOX5Command::Type::UInt16, static_cast<uint16_t>(readUint16(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::OBJECT_AUTHORS:
        {
            uint16_t count = readUint16(dataPtr, dataEnd);
            reserveValues(1 + count);
            addValue(FOX5Command::Type::UInt16, static_cast<uint16_t>(count));
            for(uint16_t i = 0; i < count; i++)
            {
                uint16_t size = readUint16(dataPtr, dataEnd);
                addValue(FOX5Command::Type::String, readString(dataPtr, dataEnd, size));
            }
            break;
        }
        case FOX5Command::Command::OBJECT_AUTHORS_HASH:
        {
            uint16_t count = readUint16(dataPtr, dataEnd);
            reserveValues(1 + count);
            addValue(FOX5Command::Type::UInt16, static_cast<uint16_t>(count));
            for(uint16_t i = 0; i < count; i++)
            {
                uint16_t size = readUint16(dataPtr, dataEnd);
                addValue(FOX5Command::Type::Bytes, readUInt8Array(dataPtr, dataEnd, size));
            }
        }
        case FOX5Command::Command::OBJECT_LICENSE:
        {
            addValue(FOX5Command::Type::UInt8, static_cast<uint8_t>(readUint8(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::OBJECT_KEYWORDS:
        {
            uint16_t count = readUint16(dataPtr, dataEnd);
            reserveValues(1 + count);
            addValue(FOX5Command::Type::UInt16, static_cast<uint16_t>(count));
            for(uint16_t i = 0; i < count; i++)
            {
                uint16_t size = readUint16(dataPtr, dataEnd);
                addValue(FOX5Command::Type::String, readString(dataPtr, dataEnd, size));
            }
            break;
        }
        case FOX5Command::Command::OBJECT_NAME:
        {
            uint16_t size = readUint16(dataPtr, dataEnd);
            addValue(FOX5Command::Type::String, readString(dataPtr, dataEnd, size));
            break;
        }
        case FOX5Command::Command::OBJECT_DESCRIPTION:
        {
            uint16_t size = readUint16(dataPtr, dataEnd);
            addValue(FOX5Command::Type::String, readString(dataPtr, dataEnd, size));
            break;
        }
        case FOX5Command::Command::OBJECT_FLAGS:
        {
            addValue(FOX5Command::Type::UInt8, static_cast<uint8_t>(readUint8(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::OBJECT_URI:
        {
            uint16_t size = readUint16(dataPtr, dataEnd);
            addValue(FOX5Command::Type::String, FOX5Value(readString(dataPtr, dataEnd, size)));
            break;
        }
        case FOX5Command::Command::OBJECT_MORE_FLAGS:
        {
            addValue(FOX5Command::Type::UInt32, static_cast<uint32_t>(readUint32(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::OBJECT_IDENTIFIER:
        {
            addValue(FOX5Command::Type::Int32, static_cast<int32_t>(readInt32(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::OBJECT_EDIT_TYPE:
        {
            addValue(FOX5Command::Type::UInt8, static_cast<uint8_t>(readUint8(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::OBJECT_FILTER:
        {
            reserveValues(2);
            addValue(FOX5Command::Type::UInt8, static_cast<uint8_t>(readUint8(dataPtr, dataEnd)));
            addValue(FOX5Command::Type::UInt8, static_cast<uint8_t>(readUint8(dataPtr, dataEnd)));
            break;
        }
        
        // Shape
        case FOX5Command::Command::SHAPE_PURPOSE:
        {
            addValue(FOX5Command::Type::UInt8, static_cast<uint8_t>(readUint8(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::SHAPE_STATE:
        {
            addValue(FOX5Command::Type::UInt8, static_cast<uint8_t>(readUint8(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::SHAPE_DIRECTION:
        {
            addValue(FOX5Command::Type::UInt8, static_cast<uint8_t>(readUint8(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::SHAPE_RATIO:
        {
            reserveValues(2);
            addValue(FOX5Command::Type::UInt8, static_cast<uint8_t>(readUint8(dataPtr, dataEnd)));
            addValue(FOX5Command::Type::UInt8, static_cast<uint8_t>(readUint8(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::SHAPE_KITTERSPEAK:
        {
            uint16_t count = readUint16(dataPtr, dataEnd);
            reserveValues(1 + (count * 3));
            addValue(FOX5Command::Type::UInt16, static_cast<uint16_t>(count));
            for(uint16_t i = 0; i < count; i++)
            {
                addValue(FOX5Command::Type::UInt16, static_cast<uint16_t>(readUint16(dataPtr, dataEnd)));
                addValue(FOX5Command::Type::Int16, static_cast<int16_t>(readInt16(dataPtr, dataEnd)));
                addValue(FOX5Command::Type::Int16, static_cast<int16_t>(readInt16(dataPtr, dataEnd)));
            }
            break;
        }
        
        // Frame
        case FOX5Command::Command::FRAME_OFFSET:
        {
            reserveValues(2);
            addValue(FOX5Command::Type::Int16, static_cast<int16_t>(readInt16(dataPtr, dataEnd)));
            addValue(FOX5Command::Type::Int16, static_cast<int16_t>(readInt16(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::FRAME_FURRE_OFFSET:
        {
            reserveValues(2);
            addValue(FOX5Command::Type::Int16, static_cast<int16_t>(readInt16(dataPtr, dataEnd)));
            addValue(FOX5Command::Type::Int16, static_cast<int16_t>(readInt16(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::FRAME_ATTACH_PLUGS:
        {
            reserveValues(4);
            addValue(FOX5Command::Type::UInt16, static_cast<uint16_t>(readUint16(dataPtr, dataEnd)));
            addValue(FOX5Command::Type::Int16, static_cast<int16_t>(readInt16(dataPtr, dataEnd)));
            addValue(FOX5Command::Type::Int16, static_cast<int16_t>(readInt16(dataPtr, dataEnd)));
            addValue(FOX5Command::Type::Int16, static_cast<int16_t>(readInt16(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::FRAME_ATTACH_SOCKETS:
        {
            uint16_t count = readUint16(dataPtr, dataEnd);
            reserveValues(1 + (count * 4));
            addValue(FOX5Command::Type::UInt16, static_cast<uint16_t>(count));
            for(uint16_t i = 0; i < count; i++)
            {
                addValue(FOX5Command::Type::UInt8, static_cast<uint8_t>(readUint8(dataPtr, dataEnd)));
                addValue(FOX5Command::Type::Int16, static_cast<int16_t>(readInt16(dataPtr, dataEnd)));
                addValue(FOX5Command::Type::Int16, static_cast<int16_t>(readInt16(dataPtr, dataEnd)));
                addValue(FOX5Command::Type::Int16, static_cast<int16_t>(readInt16(dataPtr, dataEnd)));
            }
            break;
        }
        
        // Channel
        case FOX5Command::Command::CHANNEL_PURPOSE:
        {
            addValue(FOX5Command::Type::UInt16, static_cast<uint16_t>(readUint16(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::CHANNEL_IMAGE_ID:
        {
            addValue(FOX5Command::Type::UInt16, static_cast<uint16_t>(readUint16(dataPtr, dataEnd)));
            break;
        }
        case FOX5Command::Command::CHANNEL_OFFSET:
        {
            reserveValues(2);
            addValue(FOX5Command::Type::Int16, static_cast<int16_t>(readInt16(dataPtr, dataEnd)));
            addValue(FOX5Command::Type::Int16, static_cast<int16_t>(readInt16(dataPtr, dataEnd)));
            break;
        }
        
        default:
            throw std::runtime_error("Unknown command " + std::to_string(cmd));
    }
}

void FOX5Channel::parseData(uint8_t** dataPtr, uint8_t* dataEnd)
{
    while (*dataPtr < dataEnd)
    {
        FOX5Command cmd = FOX5Command(dataPtr, dataEnd);
        switch(cmd.mCommand)
        {
            case FOX5Command::Command::NOP:
                break;
            
            case FOX5Command::Command::LIST_START:
            {
                throw std::runtime_error("FOX5Channel can't contain lists");
                break;
            }
            
            case FOX5Command::Command::LIST_END:
                return;
            
            
            case FOX5Command::Command::CHANNEL_PURPOSE:
            {
                mPurpose = cmd.getValue<uint16_t>();
                break;
            }
            case FOX5Command::Command::CHANNEL_IMAGE_ID:
            {
                mImageID = cmd.getValue<uint16_t>();
                break;
            }
            case FOX5Command::Command::CHANNEL_OFFSET:
            {
                mOffset[0] = cmd.getValue<int16_t>();
                mOffset[1] = cmd.getValue<int16_t>();
                break;
            }
        }
    }
}

void FOX5Frame::parseData(uint8_t** dataPtr, uint8_t* dataEnd)
{
    while (*dataPtr < dataEnd)
    {
        FOX5Command cmd = FOX5Command(dataPtr, dataEnd);
        switch(cmd.mCommand)
        {
            case FOX5Command::Command::NOP:
                break;
            
            case FOX5Command::Command::LIST_START:
            {
                uint8_t level = cmd.getValue<uint8_t>();
                if(level != 4)
                    throw std::runtime_error("Expected sprite level 4 in FOX5Frame");
                
                uint32_t count = cmd.getValue<uint32_t>();
                
                mSprites.resize(count);
                for(uint32_t i = 0; i < count; i++)
                {
                    mSprites[i] = std::make_shared<FOX5Channel>(
                        dataPtr, dataEnd
                    );
                }
                break;
            }
            
            case FOX5Command::Command::LIST_END:
                return;
            
            
            case FOX5Command::Command::FRAME_OFFSET:
            {
                mFrameOffset[0] = cmd.getValue<int16_t>();
                mFrameOffset[1] = cmd.getValue<int16_t>();
                break;
            }
            case FOX5Command::Command::FRAME_FURRE_OFFSET:
            {
                mFurreOffset[0] = cmd.getValue<int16_t>();
                mFurreOffset[1] = cmd.getValue<int16_t>();
                break;
            }
        }
    }
}

void FOX5Shape::parseData(uint8_t** dataPtr, uint8_t* dataEnd)
{
    while (*dataPtr < dataEnd)
    {
        FOX5Command cmd = FOX5Command(dataPtr, dataEnd);
        switch(cmd.mCommand)
        {
            case FOX5Command::Command::NOP:
                break;
            
            case FOX5Command::Command::LIST_START:
            {
                uint8_t level = cmd.getValue<uint8_t>();
                if(level != 3)
                    throw std::runtime_error("Expected frame level 3 in FOX5Shape");
                
                uint32_t count = cmd.getValue<uint32_t>();
                
                mFrames.resize(count);
                for(uint32_t i = 0; i < count; i++)
                {
                    mFrames[i] = std::make_shared<FOX5Frame>(
                        dataPtr, dataEnd
                    );
                }
                break;
            }
            
            case FOX5Command::Command::LIST_END:
                return;
            
                
            case FOX5Command::Command::SHAPE_PURPOSE:
            {
                mPurpose = static_cast<FOX5Shape::Purpose>(cmd.getValue<uint8_t>());
                break;
            }
            case FOX5Command::Command::SHAPE_STATE:
            {
                mState = cmd.getValue<uint8_t>();
                break;
            }
            case FOX5Command::Command::SHAPE_DIRECTION:
            {
                mDirection = static_cast<FOX5Shape::Direction>(cmd.getValue<uint8_t>());
                break;
            }
            case FOX5Command::Command::SHAPE_RATIO:
            {
                mRatio[0] = cmd.getValue<uint8_t>();
                mRatio[1] = cmd.getValue<uint8_t>();
                break;
            }
            case FOX5Command::Command::SHAPE_KITTERSPEAK:
            {
                uint16_t count = cmd.getValue<uint16_t>();
                mKitterspeak.resize(count);
                for(uint16_t i = 0; i < count; i++)
                {
                    uint16_t tmp = cmd.getValue<uint16_t>();
                    mKitterspeak[i] = std::make_shared<Kitterspeak_t>(
                        tmp,
                        cmd.getValue<int16_t>(),
                        cmd.getValue<int16_t>()
                    );
                }
                break;
            }
        }
    }
}

void FOX5Object::parseData(uint8_t** dataPtr, uint8_t* dataEnd)
{
    while (*dataPtr < dataEnd)
    {
        FOX5Command cmd = FOX5Command(dataPtr, dataEnd);
        switch(cmd.mCommand)
        {
            case FOX5Command::Command::NOP:
                break;
            
            case FOX5Command::Command::LIST_START:
            {
                uint8_t level = cmd.getValue<uint8_t>();
                if(level != 2)
                    throw std::runtime_error("Expected shape level 2 in FOX5Object");
                
                uint32_t count = cmd.getValue<uint32_t>();
                
                mShapes.resize(count);
                for(uint32_t i = 0; i < count; i++)
                {
                    mShapes[i] = std::make_shared<FOX5Shape>(
                        dataPtr, dataEnd
                    );
                }
                break;
            }
            
            case FOX5Command::Command::LIST_END:
                return;
            
            
            case FOX5Command::Command::OBJECT_AUTHOR_REVISION:
            {
                mAuthorRevision = cmd.getValue<uint16_t>();
                break;
            }
            case FOX5Command::Command::OBJECT_AUTHORS:
            {
                // TODO: Actually add strings here
                uint16_t count = cmd.getValue<uint16_t>();
                mAuthors.resize(count);
                for(uint16_t i = 0; i < count; i++)
                {
                    mAuthors.push_back(cmd.getValue<std::string>());
                }
                break;
            }
            /*
            case FOX5Command::Command::AUTHORS_HASH:
            {
                // TODO: Actually add strings here
                uint16_t count = cmd.getValue<uint16_t>();
                mAuthors.resize(count);
                for(uint16_t i; i < count; i++)
                {
                    mAuthors.push_back(cmd.getValue<std::string>());
                }
                break;
            }
            */
            case FOX5Command::Command::OBJECT_LICENSE:
            {
                mLicense = static_cast<FOX5Object::License>(cmd.getValue<uint8_t>());
                break;
            }
            case FOX5Command::Command::OBJECT_KEYWORDS:
            {
                // TODO: Actually add strings here
                uint16_t count = cmd.getValue<uint16_t>();
                mKeywords.resize(count);
                for(uint16_t i = 0; i < count; i++)
                {
                    mKeywords.push_back(cmd.getValue<std::string>());
                }
                break;
            }
            case FOX5Command::Command::OBJECT_NAME:
            {
                mName = cmd.getValue<std::string>();
                break;
            }
            case FOX5Command::Command::OBJECT_DESCRIPTION:
            {
                mDescription = cmd.getValue<std::string>();
                break;
            }
            case FOX5Command::Command::OBJECT_FLAGS:
            {
                mFlags = cmd.getValue<uint8_t>();
                break;
            }
            case FOX5Command::Command::OBJECT_URI:
            {
                mURI = cmd.getValue<std::string>();
                break;
            }
            case FOX5Command::Command::OBJECT_MORE_FLAGS:
            {
                mMoreFlags = cmd.getValue<uint32_t>();
                break;
            }
            case FOX5Command::Command::OBJECT_IDENTIFIER:
            {
                mObjectID = cmd.getValue<int32_t>();
                break;
            }
            case FOX5Command::Command::OBJECT_EDIT_TYPE:
            {
                mEditType = cmd.getValue<uint8_t>();
                break;
            }
            case FOX5Command::Command::OBJECT_FILTER:
            {
                mFilterTarget = cmd.getValue<uint8_t>();
                mFilterMode = cmd.getValue<uint8_t>();
                break;
            }
        }
    }
}

void FOX5::parseData(uint8_t** dataPtr, uint8_t* dataEnd)
{
    while (*dataPtr < dataEnd)
    {
        FOX5Command cmd = FOX5Command(dataPtr, dataEnd);
        switch(cmd.mCommand)
        {
            case FOX5Command::Command::NOP:
                break;
            
            case FOX5Command::Command::LIST_START:
            {
                uint8_t level = cmd.getValue<uint8_t>();
                if(level != 1)
                    throw std::runtime_error("Expected object level 1 in FOX5File");
                
                uint32_t count = cmd.getValue<uint32_t>();
                
                mObjects.resize(count);
                for(uint32_t i = 0; i < count; i++)
                {
                    mObjects[i] = std::make_shared<FOX5Object>(
                        dataPtr, dataEnd
                    );
                }
                break;
            }
            
            case FOX5Command::Command::LIST_END:
                return;
            
            
            case FOX5Command::Command::FILE_IMAGE_LIST:
            {
                uint32_t count = cmd.getValue<uint32_t>();
                mImageList.resize(count);
                uint32_t offset = 0;
                for(uint32_t i = 0; i < count; i++)
                {
                    uint32_t compressedSize = cmd.getValue<uint32_t>();
                    uint16_t width = cmd.getValue<uint16_t>();
                    uint16_t height = cmd.getValue<uint16_t>();
                    uint8_t format = cmd.getValue<uint8_t>();
                    
                    mImageList[i] = std::make_shared<FOX5Image>(
                        offset,
                        compressedSize,
                        width, height,
                        static_cast<FOX5Image::ImageFormat>(format)
                    );
                    offset += compressedSize;
                }
                break;
            }
            
            case FOX5Command::Command::FILE_GENERATOR:
            {
                mGenerator = cmd.getValue<uint8_t>();
                break;
            }
        }
    }
}

FOX5Image FOX5::getImage(uint32_t id)
{
    if(id >= mImageList.size())
        throw std::runtime_error("Image index out of bounds");
    FOX5Image im = FOX5Image(*mImageList[id]);
    im.mData.resize(im.getMemSize());
    mFile.seekg(mImageStart + im.mOffset, std::ios::beg);
    mFile.read(reinterpret_cast<char*>(im.mData.data()), im.mCompressedSize);
    if(mEncryptionType == EncryptionType::ENCRYPTED)
    {
#ifdef HAS_CIPHER
        Fox5Cipher(im.mData, im.mCompressedSize, im.mData.size(), mSeed);
#else
        throw std::runtime_error("Can't decrypt without fox5 cipher library");
#endif
    }
    
    if(mCompressionType == FOX5::CompressionType::LZMA)
    {
        im.mData = decompressLZMA(im.mData, im.mData.size());
    }
    else if(mCompressionType != FOX5::CompressionType::NOT)
    {
        throw std::runtime_error("Unknown compression type");
    }
    return im;
}

FOX5::FOX5(const std::string& filename) :
    mFile(filename, std::ios::in | std::ios::binary)
{
    if (!mFile || !mFile.is_open()) throw std::runtime_error("Failed to open file.");
    mFileName = getBasename(filename);
    
    std::transform(mFileName.begin(), mFileName.end(), mFileName.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    mFile.seekg(0, std::ios::end);
    if(mFile.tellg() < 20) throw std::runtime_error("Too small to be a FOX5 file.");
    
    mFile.seekg(-20, std::ios::end);
    
    uint8_t uint8read;
    mFile.read(reinterpret_cast<char*>(&uint8read), sizeof(uint8read));
    if (!mFile) throw std::runtime_error("Failed to read compression type.");
    mCompressionType = static_cast<CompressionType>(uint8read);
    
    mFile.read(reinterpret_cast<char*>(&uint8read), sizeof(uint8read));
    if (!mFile) throw std::runtime_error("Failed to read encryption type.");
    mEncryptionType = static_cast<EncryptionType>(uint8read);

    
    mFile.seekg(2, std::ios::cur); //Skip two reserved
    
    uint32_t dbCompressedSize = readUint32(mFile);
    uint32_t dbUncompressedSize = readUint32(mFile);
    
    char magic[8] = {0};
    mFile.read(magic, 8);
    if (!mFile) throw std::runtime_error("Failed to read magic from FOX5.");
    
    if(std::string(magic, sizeof(magic)) != "FOX5.1.1")
        throw std::runtime_error("Not a FOX5 file.");
    
    
    if(mEncryptionType == EncryptionType::ENCRYPTED)
    {
        mFile.seekg(-36, std::ios::end);
        mFile.read(reinterpret_cast<char*>(mSeed), sizeof(mSeed));
        if (!mFile) throw std::runtime_error("Failed to read seed from FOX5.");
    }
    
    mFile.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> commandBlock(dbUncompressedSize);
    mFile.read(reinterpret_cast<char*>(commandBlock.data()), dbCompressedSize);
    
    if(mEncryptionType == EncryptionType::ENCRYPTED)
    {
#ifdef HAS_CIPHER
        Fox5Cipher(commandBlock, dbCompressedSize, dbUncompressedSize, mSeed);
#else
        throw std::runtime_error("Can't decrypt without fox5 cipher library");
#endif
    }
    
    if(mCompressionType == FOX5::CompressionType::LZMA)
    {
        commandBlock = decompressLZMA(commandBlock, dbUncompressedSize);
    }
    else if(mCompressionType != FOX5::CompressionType::NOT)
    {
        throw std::runtime_error("Unknown compression type");
    }
    
    mImageStart = dbCompressedSize;
    
    uint8_t* pointer = commandBlock.data();
    uint8_t* dataEnd = pointer + commandBlock.size();
    pointer += 4;
    
    FOX5Command cmd = FOX5Command(&pointer, dataEnd);
    if(cmd.mCommand != FOX5Command::Command::LIST_START)
        throw std::runtime_error("Expected list start as first entry");
    
    if(cmd.getValue<uint8_t>() != 0)
        throw std::runtime_error("Expected file level 0 at start");
    
    if(cmd.getValue<uint32_t>() != 1)
        throw std::runtime_error("File level list should always have 1 entry");
    
    parseData(&pointer, dataEnd);
    printf("Fox5 loaded\n");
}

FOX5::~FOX5()
{
    if(mFile && mFile.is_open())
    {
        mFile.close();
    }
}