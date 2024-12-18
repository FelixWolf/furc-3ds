#include "filecommon.h"

#define LZMA_PROPS_SIZE 5
#define LZMA_ALONE_HEADER_SIZE 13 // LZMA Alone has 5 bytes properties + 8 bytes uncompressed size

// Implement memory allocation functions
void* SzAlloc(const ISzAlloc* /*p*/, size_t size)
{
    return std::malloc(size);
}

void SzFree(const ISzAlloc* /*p*/, void* address)
{
    std::free(address);
}

static ISzAlloc g_Alloc = { SzAlloc, SzFree };
std::vector<uint8_t> decompressLZMA(const std::vector<uint8_t>& compressedData, SizeT uncompressedSize)
{
    std::vector<uint8_t> decompressedData;

    // LZMA "Alone" format includes an 8-byte uncompressed size in the header (after the properties)
    if(uncompressedSize == 0)
    {
        memcpy(&uncompressedSize, compressedData.data() + LZMA_PROPS_SIZE, sizeof(uncompressedSize));
    }
    
    decompressedData.resize(uncompressedSize);

    SizeT destLen = uncompressedSize;
    SizeT srcLen = compressedData.size() - LZMA_ALONE_HEADER_SIZE; // Skip the 13-byte header
    
    ELzmaStatus status;

    int res = LzmaDecode(
        decompressedData.data(), &destLen,
        compressedData.data() + LZMA_ALONE_HEADER_SIZE, &srcLen,
        compressedData.data(), LZMA_PROPS_SIZE,
        LZMA_FINISH_ANY, &status, &g_Alloc);

    if(res != SZ_OK)
        throw std::runtime_error("LZMA decompression failed");

    return decompressedData;
}

// Utilities
uint32_t readUint32(uint8_t** dataPtr, uint8_t* dataEnd) {
    if (*dataPtr + 4 > dataEnd)
        throw std::runtime_error("Not enough data to read uint32_t.");

    uint32_t value = (static_cast<uint32_t>((*dataPtr)[0]) << 24) |
                     (static_cast<uint32_t>((*dataPtr)[1]) << 16) |
                     (static_cast<uint32_t>((*dataPtr)[2]) << 8) |
                     (static_cast<uint32_t>((*dataPtr)[3]));

    *dataPtr += 4; // Move the pointer forward by 4 bytes
    return value;
}

int32_t readInt32(uint8_t** dataPtr, uint8_t* dataEnd) {
    if (*dataPtr + 4 > dataEnd)
        throw std::runtime_error("Not enough data to read uint32_t.");

    int32_t value = (static_cast<int32_t>((*dataPtr)[0]) << 24) |
                     (static_cast<int32_t>((*dataPtr)[1]) << 16) |
                     (static_cast<int32_t>((*dataPtr)[2]) << 8) |
                     (static_cast<int32_t>((*dataPtr)[3]));

    *dataPtr += 4; // Move the pointer forward by 4 bytes
    return value;
}

uint16_t readUint16(uint8_t** dataPtr, uint8_t* dataEnd) {
    if (*dataPtr + 2 > dataEnd)
        throw std::runtime_error("Not enough data to read uint16_t.");
    
    uint16_t value = (static_cast<uint16_t>((*dataPtr)[0]) << 8) |
                     (static_cast<uint16_t>((*dataPtr)[1]));

    *dataPtr += 2; // Move the pointer forward by 2 bytes
    return value;
}

int16_t readInt16(uint8_t** dataPtr, uint8_t* dataEnd) {
    if (*dataPtr + 2 > dataEnd)
        throw std::runtime_error("Not enough data to read uint16_t.");
    
    int16_t value = (static_cast<int16_t>((*dataPtr)[0]) << 8) |
                    (static_cast<int16_t>((*dataPtr)[1]));

    *dataPtr += 2; // Move the pointer forward by 2 bytes
    return value;
}

uint8_t readUint8(uint8_t** dataPtr, uint8_t* dataEnd) {
    if (*dataPtr + 1 > dataEnd)
        throw std::runtime_error("Not enough data to read uint8_t.");

    uint8_t value = static_cast<uint8_t>((*dataPtr)[0]);

    *dataPtr += 1; // Move the pointer forward by 2 bytes
    return value;
}

int8_t readInt8(uint8_t** dataPtr, uint8_t* dataEnd) {
    if (*dataPtr + 1 > dataEnd)
        throw std::runtime_error("Not enough data to read uint8_t.");

    int8_t value = static_cast<int8_t>((*dataPtr)[0]);

    *dataPtr += 1; // Move the pointer forward by 2 bytes
    return value;
}

std::string readString(uint8_t** dataPtr, uint8_t* dataEnd, size_t length) {
    if (*dataPtr + length > dataEnd) {
        throw std::runtime_error("Not enough data to read std::string.");
    }

    std::string value(reinterpret_cast<char*>(*dataPtr), length);
    *dataPtr += length; // Move the pointer forward by the length of the string

    return value;
}

std::vector<uint8_t> readUInt8Array(uint8_t** dataPtr, uint8_t* dataEnd, size_t length) {
    if (*dataPtr + length > dataEnd) {
        throw std::runtime_error("Not enough data to read uint8_t array.");
    }

    std::vector<uint8_t> value(*dataPtr, *dataPtr + length);
    *dataPtr += length; // Move the pointer forward by the length of the array

    return value;
}

uint32_t readUint32(std::ifstream& file)
{
    uint8_t buffer[4];
    file.read(reinterpret_cast<char*>(buffer), 4);

    if (!file)
        throw std::runtime_error("Failed to read uint32_t from file.");

    uint32_t value = (static_cast<uint32_t>(buffer[0]) << 24) |
                     (static_cast<uint32_t>(buffer[1]) << 16) |
                     (static_cast<uint32_t>(buffer[2]) << 8) |
                     (static_cast<uint32_t>(buffer[3]));

    return value;
}

uint16_t readUint16(std::ifstream& file)
{
    uint8_t buffer[2];
    file.read(reinterpret_cast<char*>(buffer), 2);

    if (!file)
        throw std::runtime_error("Failed to read uint32_t from file.");

    uint16_t value = (static_cast<uint32_t>(buffer[0]) << 8) |
                     (static_cast<uint32_t>(buffer[1]));

    return value;
}

std::string getBasename(const std::string& path)
{
    // Find the last directory separator, considering both '/' and '\\'
    size_t lastSeparator = path.find_last_of("/\\");
    
    // If no separator is found, return the entire path as the basename
    if (lastSeparator == std::string::npos)
    {
        return path;
    }

    // Extract and return the substring after the last separator
    return path.substr(lastSeparator + 1);
}
