#ifndef BufferParser_h
#define BufferParser_h

#include <string>

#include "StorageKitTypes.h"

class SKBufferParser
{
  private:
    const U8 *buffer;

  public:
    SKBufferParser(const U8 *buffer);
    std::string trim(const std::string& str);

    U64 getQWord(U32 byteIndex);
    U32 getDWord(U32 byteIndex);
    U8 getByte(U32 byteIndex);
    U16 getWord(U32 index);
    U16 getBit(U32 index, U32 bitIndex);
    std::string getString(U32 wordPos, U32 wordCount);
    std::string getStringInLE(U32 wordPos, U32 wordCount);
};

#endif
