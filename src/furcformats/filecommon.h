#ifndef FILECOMMON_H
#define FILECOMMON_H
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include "LzmaDec.h"

std::vector<uint8_t> decompressLZMA(const std::vector<uint8_t>& compressedData, SizeT uncompressedSize);
uint32_t readUint32(uint8_t** dataPtr, uint8_t* dataEnd);
int32_t readInt32(uint8_t** dataPtr, uint8_t* dataEnd);
uint16_t readUint16(uint8_t** dataPtr, uint8_t* dataEnd);
int16_t readInt16(uint8_t** dataPtr, uint8_t* dataEnd);
uint8_t readUint8(uint8_t** dataPtr, uint8_t* dataEnd);
int8_t readInt8(uint8_t** dataPtr, uint8_t* dataEnd);
std::string readString(uint8_t** dataPtr, uint8_t* dataEnd, size_t length);
std::vector<uint8_t> readUInt8Array(uint8_t** dataPtr, uint8_t* dataEnd, size_t length);
uint32_t readUint32(std::ifstream& file);
uint16_t readUint16(std::ifstream& file);
std::string getBasename(const std::string& path);

#endif