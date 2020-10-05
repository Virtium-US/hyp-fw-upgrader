/**
*
* @section LICENSE
* Copyright 2018 Virtium Technology
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http ://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
#ifndef AlignedBuffer_h
#define AlignedBuffer_h

#include "defines.h"

/**
*
* @section DESCRIPTION
*
* An object is responsible to aligned by the specified aligment for size in bytes.
*
*/
class STORAGEAPI_DLLSPEC SKAlignedBuffer
{
  private:
    static const U8 CACHE_ALIGN_BYTES;
    static const U32 CACHE_ALIGN_BITMASK;
    static const U16 LEFT_SPARE_BYTES;

  private:
    U64 actualBufferSizeInByte;
    U64 actualBufferSizeInSector;
    U64 currentBufferSizeInByte;
    U64 currentBufferSizeInSector;
    U8 *offsetBuffer;
    U8 *alignedBuffer;
    U8 *unalignedBuffer;

  public:
    /**
    * @brief Creates an SKAlignedBuffer object with the size in bytes and the allocate size in byte.
    *
    * @param sizeInByte An U64 variable to allocate memory via size in bytes.
    */
    SKAlignedBuffer(U64 sizeInByte);

    /**
    * @brief Destructor that frees up memory.
    */
    virtual ~SKAlignedBuffer();

  public:
    /**
    * @brief Gets data buffer.
    *
    * @return An U8 pointer of data buffer.
    */
    U8 *ToDataBuffer();

    /**
    * @brief (only for windows) Gets data buffer with left spare.
    *
    * @param leftSpare left spare memory for data buffer. If this value greater than LEFT_SPARE_BYTES, return result is nullptr
    * @return An U8 pointer of data buffer with left spare.
    */
    U8 *ToDataBufferWithLeftSpare(U16 leftSpare);

    /**
    * @brief Gets size in byte of allocated data buffer.
    *
    * @return An U32 of size in byte.
    */
    U32 GetSizeInByte();

    /**
    * @brief Set data buffer size. Use for trim right data buffer memory.
    * @param sizeInByte Size of data buffer. This method will does nothing if sizeInByte greater than or equal with sizeInByte of Constructor
    */
    void SetDataBufferSize(U32 sizeInByte);
};

#endif
