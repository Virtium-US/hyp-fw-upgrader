#include "StorageKitU9VcCommandDesc.h"

const U8 SKU9VcCommandDesc::COMMAND_U9_VC_TUNNEL = 0xDD;

const U8 SKU9VcCommandDesc::U9_VSC = 16;
const U8 SKU9VcCommandDesc::VC_TRIM_COMMAND_CODE = 36;

SKU9VcCommandDesc::SKU9VcCommandDesc(const SKDataAccess &dataAccess, const U8 &commandCode,
                               const SKScsiFieldFormatting &fieldFormatting = COMMAND_16) :
    SKScsiCommandDesc(dataAccess, commandCode, fieldFormatting)
{
}

SKScsiCommandDesc* SKU9VcCommandDesc::createTrimAddressRangeDesc()
{
    // Vendor Command Code – CS=16, CC=36, R/W=0
    SKU9VcCommandDesc* cmdDesc = new SKU9VcCommandDesc(WRITE_TO_DEVICE, SKU9VcCommandDesc::COMMAND_U9_VC_TUNNEL, COMMAND_HYP_VC);
    cmdDesc->prepareCommandHypVc(U9_VSC, VC_TRIM_COMMAND_CODE, 0x0000, SKHypVcDirection::WRITE);
    cmdDesc->dataTransferLengthInSectors = 1;
    return cmdDesc;
}

SKScsiCommandDesc* SKU9VcCommandDesc::createReadFirmwareVersion()
{
    // Vendor Command Code = CS=0, CC=16, R/W=1
    SKScsiCommandDesc* cmdDesc = new SKU9VcCommandDesc(READ_FROM_DEVICE, SKU9VcCommandDesc::COMMAND_U9_VC_TUNNEL, COMMAND_10);
    cmdDesc->inputFields.Cdb[5] = 0x21;
    return cmdDesc;
}

void SKU9VcCommandDesc::prepareCommandHypVc(const U8 &cmdSet, const U8 &cmdCode, const U16 &sectorNumber, const SKHypVcDirection &direction)
{
    this->inputFields.CommandHypVc.CmdSet = cmdSet;
    this->inputFields.CommandHypVc.HighSectorNumber = ((sectorNumber & 0xFF00) >> 8);
    this->inputFields.CommandHypVc.LowSectorNumber = sectorNumber & 0xFF;
    this->inputFields.CommandHypVc.CmdCode = cmdCode;
    this->inputFields.CommandHypVc.Direction = direction;
}
