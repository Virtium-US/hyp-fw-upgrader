#include <memory.h>

#include "StorageKitScsiCommandDesc.h"
#include "StorageKitCommandDesc.h"

const U8 SKScsiCommandDesc::COMMAND_INQUIRY = 0x12;
const U8 SKScsiCommandDesc::COMMAND_READ_6 = 0x08;
const U8 SKScsiCommandDesc::COMMAND_READ_10 = 0x28;
const U8 SKScsiCommandDesc::COMMAND_READ_12 = 0xA8;
const U8 SKScsiCommandDesc::COMMAND_READ_16 = 0x88;
const U8 SKScsiCommandDesc::COMMAND_WRITE_6 = 0x0A;
const U8 SKScsiCommandDesc::COMMAND_WRITE_10 = 0x2A;
const U8 SKScsiCommandDesc::COMMAND_WRITE_12 = 0xAA;
const U8 SKScsiCommandDesc::COMMAND_WRITE_16 = 0x8A;
const U8 SKScsiCommandDesc::COMMAND_READ_CAPACITY = 0x25;

U8 getCdbLength(const SKScsiFieldFormatting &fieldFormatting)
{
    return (fieldFormatting >> 4);
}

void setAddress6(SKScsiCommandDesc *cmdDesc, const U32 &lba, const U16 &sectorCount)
{
    cmdDesc->inputFields.Command6.LbaHigh_16_21 = (lba >> 16) & 0x1F;
    cmdDesc->inputFields.Command6.LbaLow[0] = (lba >> 8) & 0xFF;
    cmdDesc->inputFields.Command6.LbaLow[1] = lba & 0xFF;

    cmdDesc->inputFields.Command6.Length = (sectorCount & 0xFF);

    cmdDesc->dataTransferLengthInSectors = sectorCount;
}

void setAddress10(SKScsiCommandDesc *cmdDesc, const U32 &lba, const U16 &sectorCount)
{
    cmdDesc->inputFields.Command10.Lba[0] = (lba >> 24) & 0xFF;
    cmdDesc->inputFields.Command10.Lba[1] = (lba >> 16) & 0xFF;
    cmdDesc->inputFields.Command10.Lba[2] = (lba >> 8) & 0xFF;
    cmdDesc->inputFields.Command10.Lba[3] = lba & 0xFF;

    cmdDesc->inputFields.Command10.Length[0] = (sectorCount >> 8) & 0xFF;
    cmdDesc->inputFields.Command10.Length[1] = sectorCount & 0xFF;

    cmdDesc->dataTransferLengthInSectors = sectorCount;
}

void setAddress12(SKScsiCommandDesc *cmdDesc, const U32 &lba, const U32 &sectorCount)
{
    cmdDesc->inputFields.Command12.Lba[0] = (lba >> 24) & 0xFF;
    cmdDesc->inputFields.Command12.Lba[1] = (lba >> 16) & 0xFF;
    cmdDesc->inputFields.Command12.Lba[2] = (lba >> 8) & 0xFF;
    cmdDesc->inputFields.Command12.Lba[3] = lba & 0xFF;

    cmdDesc->inputFields.Command12.Length[0] = (sectorCount >> 24) & 0xFF;
    cmdDesc->inputFields.Command12.Length[1] = (sectorCount >> 16) & 0xFF;
    cmdDesc->inputFields.Command12.Length[2] = (sectorCount >> 8) & 0xFF;
    cmdDesc->inputFields.Command12.Length[3] = sectorCount & 0xFF;

    cmdDesc->dataTransferLengthInSectors = sectorCount;
}

void setAddress16(SKScsiCommandDesc *cmdDesc, const U64 &lba, const U32 &sectorCount)
{
    cmdDesc->inputFields.Command16.Lba[0] = (lba >> 56) & 0xFF;
    cmdDesc->inputFields.Command16.Lba[1] = (lba >> 48) & 0xFF;
    cmdDesc->inputFields.Command16.Lba[2] = (lba >> 40) & 0xFF;
    cmdDesc->inputFields.Command16.Lba[3] = (lba >> 32) & 0xFF;
    cmdDesc->inputFields.Command16.Lba[4] = (lba >> 24) & 0xFF;
    cmdDesc->inputFields.Command16.Lba[5] = (lba >> 16) & 0xFF;
    cmdDesc->inputFields.Command16.Lba[6] = (lba >> 8) & 0xFF;
    cmdDesc->inputFields.Command16.Lba[7] = lba & 0xFF;

    cmdDesc->inputFields.Command16.Length[0] = (sectorCount >> 24) & 0xFF;
    cmdDesc->inputFields.Command16.Length[1] = (sectorCount >> 16) & 0xFF;
    cmdDesc->inputFields.Command16.Length[2] = (sectorCount >> 8) & 0xFF;
    cmdDesc->inputFields.Command16.Length[3] = sectorCount & 0xFF;

    cmdDesc->dataTransferLengthInSectors = sectorCount;
}

SKScsiCommandDesc::SKScsiCommandDesc(const SKDataAccess &dataAccess, const U8 &commandCode,
                               const SKScsiFieldFormatting &fieldFormatting = COMMAND_16) :
    SKCommandDesc(SCSI_COMMAND)
{
    this->dataAccess = dataAccess;
    this->fieldFormatting = fieldFormatting;

    memset(this->inputFields.Cdb, 0, sizeof(SKCommandInputFields));
    this->inputFields.OpCode = commandCode;
    this->dataTransferLengthInSectors = dataAccess == NONE ? 0 : 1;
    this->cdbLength = getCdbLength(fieldFormatting);
}

SKScsiCommandDesc* SKScsiCommandDesc::createInquiryDesc(const SKInquiryPageCode &pageCode)
{
    SKScsiCommandDesc* cmdDesc = new SKScsiCommandDesc(READ_FROM_DEVICE, SKScsiCommandDesc::COMMAND_INQUIRY, COMMAND_6);
    cmdDesc->inputFields.CommandInquiry.Evpd = (SKInquiryPageCode::SKStandard == pageCode ? 0 : 1);
    if (SKInquiryPageCode::SKStandard != pageCode)
    {
        cmdDesc->inputFields.CommandInquiry.PageCode = pageCode;
    }
    cmdDesc->inputFields.CommandInquiry.Length = 0xFF00;
    return cmdDesc;
}

SKScsiCommandDesc* SKScsiCommandDesc::createRead6Desc(const U32 &lba, const U16 &sectorCount)
{
    if ((lba & 0xFFE00000) != 0 || sectorCount > 256 || sectorCount == 0)
    {
        // The lba or sectorCount doesn't fit to the command
        return nullptr;
    }

    SKScsiCommandDesc* cmdDesc = new SKScsiCommandDesc(READ_FROM_DEVICE, SKScsiCommandDesc::COMMAND_READ_6, COMMAND_6);
    setAddress6(cmdDesc, lba, sectorCount);
    return cmdDesc;
}

SKScsiCommandDesc* SKScsiCommandDesc::createRead10Desc(const U32 &lba, const U16 &sectorCount)
{
    SKScsiCommandDesc* cmdDesc = new SKScsiCommandDesc(READ_FROM_DEVICE, SKScsiCommandDesc::COMMAND_READ_10, COMMAND_10);
    setAddress10(cmdDesc, lba, sectorCount);
    return cmdDesc;
}

SKScsiCommandDesc* SKScsiCommandDesc::createRead12Desc(const U32 &lba, const U32 &sectorCount)
{
    SKScsiCommandDesc* cmdDesc = new SKScsiCommandDesc(READ_FROM_DEVICE, SKScsiCommandDesc::COMMAND_READ_12, COMMAND_12);
    setAddress12(cmdDesc, lba, sectorCount);
    return cmdDesc;
}

SKScsiCommandDesc* SKScsiCommandDesc::createRead16Desc(const U64 &lba, const U32 &sectorCount)
{
    SKScsiCommandDesc* cmdDesc = new SKScsiCommandDesc(READ_FROM_DEVICE, SKScsiCommandDesc::COMMAND_READ_16, COMMAND_16);
    setAddress16(cmdDesc, lba, sectorCount);
    return cmdDesc;
}

SKScsiCommandDesc* SKScsiCommandDesc::createWrite6Desc(const U32 &lba, const U16 &sectorCount)
{
    if ((lba & 0xFFE00000) != 0 || sectorCount > 256 || sectorCount == 0)
    {
        // The lba or sectorCount doesn't fit to the command
        return nullptr;
    }

    SKScsiCommandDesc* cmdDesc = new SKScsiCommandDesc(WRITE_TO_DEVICE, SKScsiCommandDesc::COMMAND_WRITE_6, COMMAND_6);
    setAddress6(cmdDesc, lba, sectorCount);
    return cmdDesc;
}

SKScsiCommandDesc* SKScsiCommandDesc::createWrite10Desc(const U32 &lba, const U16 &sectorCount)
{
    SKScsiCommandDesc* cmdDesc = new SKScsiCommandDesc(WRITE_TO_DEVICE, SKScsiCommandDesc::COMMAND_WRITE_10, COMMAND_10);
    setAddress10(cmdDesc, lba, sectorCount);
    return cmdDesc;
}

SKScsiCommandDesc* SKScsiCommandDesc::createWrite12Desc(const U32 &lba, const U32 &sectorCount)
{
    SKScsiCommandDesc* cmdDesc = new SKScsiCommandDesc(WRITE_TO_DEVICE, SKScsiCommandDesc::COMMAND_WRITE_12, COMMAND_12);
    setAddress12(cmdDesc, lba, sectorCount);
    return cmdDesc;
}

SKScsiCommandDesc* SKScsiCommandDesc::createWrite16Desc(const U64 &lba, const U32 &sectorCount)
{
    SKScsiCommandDesc* cmdDesc = new SKScsiCommandDesc(WRITE_TO_DEVICE, SKScsiCommandDesc::COMMAND_WRITE_16, COMMAND_16);
    setAddress16(cmdDesc, lba, sectorCount);
    return cmdDesc;
}

SKScsiCommandDesc* SKScsiCommandDesc::createReadCapacityDesc()
{
    SKScsiCommandDesc* cmdDesc = new SKScsiCommandDesc(READ_FROM_DEVICE, SKScsiCommandDesc::COMMAND_READ_CAPACITY, COMMAND_10);
    return cmdDesc;
}
