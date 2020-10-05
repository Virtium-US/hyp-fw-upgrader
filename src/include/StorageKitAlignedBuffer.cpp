#include "StorageKitAlignedBuffer.h"

const U8 SKAlignedBuffer::CACHE_ALIGN_BYTES = 64;
const U32 SKAlignedBuffer::CACHE_ALIGN_BITMASK = (SKAlignedBuffer::CACHE_ALIGN_BYTES - 1);

#ifdef os_win
const U16 SKAlignedBuffer::LEFT_SPARE_BYTES = 512;
#else
const U16 SKAlignedBuffer::LEFT_SPARE_BYTES = 0;
#endif

SKAlignedBuffer::SKAlignedBuffer(U64 sizeInByte)
{
    actualBufferSizeInByte = sizeInByte;
    actualBufferSizeInSector = (actualBufferSizeInByte / SECTOR_SIZE_IN_BYTES);
    currentBufferSizeInByte = actualBufferSizeInByte;
    currentBufferSizeInSector = actualBufferSizeInSector;

    // Allocate enough memory to ensure cache alignment
    unalignedBuffer = new U8[LEFT_SPARE_BYTES + sizeInByte + CACHE_ALIGN_BYTES];
    U8 *unsparedBuffer = unalignedBuffer + LEFT_SPARE_BYTES;
    U64 currentAddress = (U64)unsparedBuffer & 0x0000FFFF;
    U64 nextAlignAddress = ((currentAddress | CACHE_ALIGN_BITMASK) + 1);
    alignedBuffer = (unsparedBuffer + (nextAlignAddress - currentAddress));
    offsetBuffer = alignedBuffer;
}

SKAlignedBuffer::~SKAlignedBuffer()
{
    delete[] unalignedBuffer;
}

U8 *SKAlignedBuffer::ToDataBuffer()
{
    return offsetBuffer;
}

U8 *SKAlignedBuffer::ToDataBufferWithLeftSpare(U16 leftSpare)
{
    if (LEFT_SPARE_BYTES < leftSpare)
    {
        return nullptr;
    }
    return offsetBuffer - leftSpare;
}

U32 SKAlignedBuffer::GetSizeInByte()
{
    return currentBufferSizeInByte;
}

void SKAlignedBuffer::SetDataBufferSize(U32 sizeInByte)
{
    if (currentBufferSizeInByte <= sizeInByte)
    {
        return;
    }

    currentBufferSizeInByte = sizeInByte;
    currentBufferSizeInSector = currentBufferSizeInByte / SECTOR_SIZE_IN_BYTES;
}
