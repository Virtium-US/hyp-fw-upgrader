#include "StorageKitBufferParser.h"
#include <algorithm>

using namespace std;

SKBufferParser::SKBufferParser(const U8 *buffer)
{
    this->buffer = buffer;
}

U64 SKBufferParser::getQWord(U32 byteIndex)
{
    return static_cast<U64>(this->buffer[byteIndex])            |
          (static_cast<U64>(this->buffer[byteIndex + 1]) << 8)  |
          (static_cast<U64>(this->buffer[byteIndex + 2]) << 16) |
          (static_cast<U64>(this->buffer[byteIndex + 3]) << 24) |
          (static_cast<U64>(this->buffer[byteIndex + 4]) << 32) |
          (static_cast<U64>(this->buffer[byteIndex + 5]) << 40) |
          (static_cast<U64>(this->buffer[byteIndex + 6]) << 48) |
          (static_cast<U64>(this->buffer[byteIndex + 7]) << 56);
}

U32 SKBufferParser::getDWord(U32 byteIndex)
{
    return static_cast<U32>(this->buffer[byteIndex])            |
          (static_cast<U32>(this->buffer[byteIndex + 1]) << 8)  |
          (static_cast<U32>(this->buffer[byteIndex + 2]) << 16) |
          (static_cast<U32>(this->buffer[byteIndex + 3]) << 24);
}

U8 SKBufferParser::getByte(U32 byteIndex)
{
    return (this->buffer[byteIndex]);
}

U16 SKBufferParser::getWord(U32 index)
{
    return static_cast<U16>((this->buffer[(index * 2) + 1] << 8) | this->buffer[(index) * 2]);
}

U16 SKBufferParser::getBit(U32 index, U32 bitIndex)
{
    return ((this->getWord(index) >> (bitIndex)) & 0x1);
}

std::string SKBufferParser::trim(const string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::string SKBufferParser::getString(U32 wordPos, U32 wordCount)
{
    string value = "";
    U32 byteOffset = wordPos * 2;

    for (U32 i = 0; i < wordCount; ++i)
    {
        value.push_back(static_cast<S8>(this->buffer[byteOffset + 1]));
        value.push_back(static_cast<S8>(this->buffer[byteOffset]));
        byteOffset += 2;
    }

    return trim(value);
}

std::string SKBufferParser::getStringInLE(U32 wordPos, U32 wordCount)
{
    string value = "";
    U32 byteOffset = wordPos * 2;
    for (U32 i = 0; i < wordCount; ++i)
    {
        value.push_back(static_cast<S8>(this->buffer[byteOffset]));
        value.push_back(static_cast<S8>(this->buffer[byteOffset + 1]));
        byteOffset += 2;
    }
    return trim(value);
}
