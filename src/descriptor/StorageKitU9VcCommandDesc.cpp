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

SKScsiCommandDesc* SKU9VcCommandDesc::createTargetInfo()
{
    // Vendor Command Code – CS=16, CC=127, R/W=1
    SKU9VcCommandDesc* cmdDesc = new SKU9VcCommandDesc(READ_FROM_DEVICE, SKU9VcCommandDesc::COMMAND_U9_VC_TUNNEL, COMMAND_10);
    setCommandCode(cmdDesc->inputFields.Cdb, buildCommandCode(16, 0, 127, 1));
    return cmdDesc;
}

SKScsiCommandDesc* SKU9VcCommandDesc::createSetAddressExtension(const U16 extensionAddress)
{
    // Vendor Command Code - CS=0, SN=extensionAddress, CC=32, R/W=0
    SKU9VcCommandDesc* cmdDesc = new SKU9VcCommandDesc(WRITE_TO_DEVICE, SKU9VcCommandDesc::COMMAND_U9_VC_TUNNEL, COMMAND_10);
    setCommandCode(cmdDesc->inputFields.Cdb, buildCommandCode(0, extensionAddress & 0xFFFF, 32, 0));
    return cmdDesc;
}

SKScsiCommandDesc* SKU9VcCommandDesc::createSetBaseAddress()
{
    // Vendor Command Code – CS=16, CC=32, R/W=0
    SKU9VcCommandDesc* cmdDesc = new SKU9VcCommandDesc(WRITE_TO_DEVICE, SKU9VcCommandDesc::COMMAND_U9_VC_TUNNEL, COMMAND_10);
    setCommandCode(cmdDesc->inputFields.Cdb, buildCommandCode(16, 0, 32, 0));
    return cmdDesc;
}

SKScsiCommandDesc* SKU9VcCommandDesc::createReadFirmwareVersion()
{
    // Vendor Command Code - CS=0, CC=16, R/W=1
    SKScsiCommandDesc* cmdDesc = new SKU9VcCommandDesc(READ_FROM_DEVICE, SKU9VcCommandDesc::COMMAND_U9_VC_TUNNEL, COMMAND_10);
    setCommandCode(cmdDesc->inputFields.Cdb, buildCommandCode(0, 0, 16, 1));
    return cmdDesc;
}

SKScsiCommandDesc* SKU9VcCommandDesc::createFirmwareUpdatePrepare() 
{
    // Vendor Command Code - CS=2, CC=40, R/W=0
    SKU9VcCommandDesc* cmdDesc = new SKU9VcCommandDesc(WRITE_TO_DEVICE, SKU9VcCommandDesc::COMMAND_U9_VC_TUNNEL, COMMAND_10);
    setCommandCode(cmdDesc->inputFields.Cdb, buildCommandCode(2, 0, 40, 0));
    return cmdDesc;
}

SKScsiCommandDesc* SKU9VcCommandDesc::createFirmwareUpdateTransfer() 
{
    // Vendor Command Code - CS=2, CC=41, R/W=0
    SKU9VcCommandDesc* cmdDesc = new SKU9VcCommandDesc(WRITE_TO_DEVICE, SKU9VcCommandDesc::COMMAND_U9_VC_TUNNEL, COMMAND_10);
    setCommandCode(cmdDesc->inputFields.Cdb, buildCommandCode(2, 0, 41, 0));
    return cmdDesc;
}

SKScsiCommandDesc* SKU9VcCommandDesc::createFirmwareUpdateExecute() 
{
    // Vendor Command Code - CS=2, CC=42, R/W=1
    SKScsiCommandDesc* cmdDesc = new SKU9VcCommandDesc(READ_FROM_DEVICE, SKU9VcCommandDesc::COMMAND_U9_VC_TUNNEL, COMMAND_10);
    setCommandCode(cmdDesc->inputFields.Cdb, buildCommandCode(2, 0, 42, 1));
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

// Creates a command code with the following format: CS = [31..2] | SN = [23..8] | CC = [7..1] | R/W = [0]
U32 SKU9VcCommandDesc::buildCommandCode(const U32 CS, const U32 SN, const U32 CC, const U32 RW) 
{
    return CS << 24 | SN << 8 | CC << 1 | RW;
}

// Sets the values for the command code in a CDB. NOTE: The command code is stored at offsets 2...5 in a given CDB
void SKU9VcCommandDesc::setCommandCode(U8* cdb, U32 cmdCode) 
{
    cdb[2] = cmdCode >> 24; // CS
    cdb[3] = cmdCode >> 16; // SN
    cdb[4] = cmdCode >> 8;  // CC
    cdb[5] = cmdCode;       // RW
}
