#include <memory.h>

#include "StorageKitAtaCommandDesc.h"
#include "StorageKitCommandDesc.h"

const U8 SKAtaCommandDesc::COMMAND_SMART                  	= 0xB0;
const U8 SKAtaCommandDesc::DEFAULT_DEVICE_VALUE           	= 0xE0;
const U8 SKAtaCommandDesc::COMMAND_IDENTIFY_DEVICE        	= 0xEC;
const U8 SKAtaCommandDesc::COMMAND_READ_SECTOR            	= 0x20;
const U8 SKAtaCommandDesc::COMMAND_READ_SECTOR_EXT        	= 0x24;
const U8 SKAtaCommandDesc::COMMAND_READ_DMA               	= 0xC8;
const U8 SKAtaCommandDesc::COMMAND_READ_DMA_EXT           	= 0x25;
const U8 SKAtaCommandDesc::COMMAND_WRITE_DMA              	= 0xCA;
const U8 SKAtaCommandDesc::COMMAND_WRITE_DMA_EXT          	= 0x35;
const U8 SKAtaCommandDesc::COMMAND_WRITE_SECTOR           	= 0x30;
const U8 SKAtaCommandDesc::COMMAND_WRITE_SECTOR_EXT       	= 0x34;
const U8 SKAtaCommandDesc::COMMAND_SECURITY_DISABLE_PASSWORD 	= 0xF6;
const U8 SKAtaCommandDesc::COMMAND_SECURITY_SET_PASSWORD 		= 0xF1;
const U8 SKAtaCommandDesc::COMMAND_SECURITY_UNLOCK 			= 0xF2;
const U8 SKAtaCommandDesc::COMMAND_SECURITY_FREEZE_LOCK 		= 0xF5;
const U8 SKAtaCommandDesc::COMMAND_SECURITY_ERASE_PREPARE 	= 0xF3;
const U8 SKAtaCommandDesc::COMMAND_SECURITY_ERASE_UNIT 		= 0xF4;
const U8 SKAtaCommandDesc::COMMAND_SANITIZE               	= 0xB4;
const U8 SKAtaCommandDesc::COMMAND_SLEEP                    = 0xE6;
const U8 SKAtaCommandDesc::COMMAND_TRUSTED_SEND           	= 0x5E;
const U8 SKAtaCommandDesc::COMMAND_TRUSTED_RECEIVE        	= 0x5C;
const U8 SKAtaCommandDesc::COMMAND_QUICK_ERASE_GET_METHOD      = 0xFE;
const U8 SKAtaCommandDesc::COMMAND_QUICK_ERASE_SET_METHOD      = 0xFE;

const U8 SKAtaCommandDesc::FEATURE_SMART_DISABLE          = 0xD9;
const U8 SKAtaCommandDesc::FEATURE_SMART_ENABLE           = 0xD8;
const U8 SKAtaCommandDesc::FEATURE_SMART_READ_DATA        = 0xD0;
const U8 SKAtaCommandDesc::FEATURE_SMART_READ_THRESHOLD   = 0xD1;
const U8 SKAtaCommandDesc::FEATURE_QUICK_ERASE_GET_METHOD = 0x01;
const U8 SKAtaCommandDesc::FEATURE_QUICK_ERASE_SET_METHOD = 0x00;

const U8 SKAtaCommandDesc::FEATURE_SANITIZE_CRYPTO_SCRAMBLE_EXT   = 0x11;
const U8 SKAtaCommandDesc::FEATURE_SANITIZE_OVERWRITE_EXT         = 0x14;
const U8 SKAtaCommandDesc::FEATURE_SANITIZE_BLOCK_ERASE_EXT       = 0x12;
const U8 SKAtaCommandDesc::FEATURE_SANITIZE_STATUS_EXT            = 0x00;

const U64 SKAtaCommandDesc::LBA_SANITIZE_CRYPTO_SCRAMBLE_EXT      = 0x000043727970;
const U64 SKAtaCommandDesc::LBA_SANITIZE_OVERWRITE_EXT            = 0x4F5700000000;
const U64 SKAtaCommandDesc::LBA_SANITIZE_BLOCK_ERASE_EXT          = 0x0000426B4572;

SKAtaCommandDesc::SKAtaCommandDesc(const SKAtaCommandDesc &desc)
{
    this->commandType = desc.commandType;

    this->inputFields = desc.inputFields;
    this->inputFieldsExt = desc.inputFieldsExt;
    this->dataAccess = desc.dataAccess;
    this->transferMode = desc.transferMode;
    this->dataTransferLengthInSectors = desc.dataTransferLengthInSectors;
    this->isExtCommand = desc.isExtCommand;
}

SKAtaCommandDesc::SKAtaCommandDesc(const SKDataAccess &dataAccess, const U8 &commandCode,
                               const SKAtaFieldFormatting &fieldFormatting = COMMAND_28_BIT,
                               const SKTransferMode &transferMode = PIO) :
    SKCommandDesc(ATA_COMMAND)
{
    this->dataAccess = dataAccess;
    this->transferMode = transferMode;
    this->isExtCommand = (COMMAND_48_BIT == fieldFormatting);
    this->dataTransferLengthInSectors = dataAccess == NONE ? 0 : 1;

    memset((U8*)(void*)&this->inputFields, 0, sizeof(SKAtaCommandInputFields));
    this->inputFields.Command = commandCode;
    this->inputFields.Device = DEFAULT_DEVICE_VALUE;
    this->inputFields.Reserved = 0;
    this->inputFields.Feature = 0;

    this->inputFieldsExt.Command = commandCode;
    this->inputFieldsExt.Device = DEFAULT_DEVICE_VALUE;
    this->inputFieldsExt.Reserved = 0;
    this->inputFieldsExt.Feature = 0;
}

SKAtaCommandDesc* SKAtaCommandDesc::createIdentifyDeviceDesc()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(READ_FROM_DEVICE, SKAtaCommandDesc::COMMAND_IDENTIFY_DEVICE);
    cmdDesc->inputFields.Count = 1;
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createReadSmartData()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(READ_FROM_DEVICE, SKAtaCommandDesc::COMMAND_SMART);
    cmdDesc->inputFields.Feature = SKAtaCommandDesc::FEATURE_SMART_READ_DATA;
    cmdDesc->inputFields.Count = 1;
    cmdDesc->inputFields.LbaMid = 0x4F;
    cmdDesc->inputFields.LbaHigh = 0xC2;
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createReadSmartThreshold()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(READ_FROM_DEVICE, SKAtaCommandDesc::COMMAND_SMART);
    cmdDesc->inputFields.Feature = SKAtaCommandDesc::FEATURE_SMART_READ_THRESHOLD;
    cmdDesc->inputFields.Count = 1;
    cmdDesc->inputFields.LbaMid = 0x4F;
    cmdDesc->inputFields.LbaHigh = 0xC2;
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createReadSectorDesc(U64 lba, U32 sectorCount)
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(READ_FROM_DEVICE, SKAtaCommandDesc::COMMAND_READ_SECTOR);
    cmdDesc->prepareLBASectorCount(lba, sectorCount);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createReadSectorExtDesc(U64 lba, U32 sectorCount)
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(READ_FROM_DEVICE, SKAtaCommandDesc::COMMAND_READ_SECTOR_EXT, COMMAND_48_BIT);
    cmdDesc->prepareLBASectorCount(lba, sectorCount);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createReadDmaDesc(U64 lba, U32 sectorCount)
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(READ_FROM_DEVICE, SKAtaCommandDesc::COMMAND_READ_DMA);
    cmdDesc->prepareLBASectorCount(lba, sectorCount);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createReadDmaExtDesc(U64 lba, U32 sectorCount)
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(READ_FROM_DEVICE, SKAtaCommandDesc::COMMAND_READ_DMA_EXT, COMMAND_48_BIT);
    cmdDesc->prepareLBASectorCount(lba, sectorCount);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createWriteDmaDesc(U64 lba, U32 sectorCount)
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(WRITE_TO_DEVICE, SKAtaCommandDesc::COMMAND_WRITE_DMA);
    cmdDesc->prepareLBASectorCount(lba, sectorCount);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createWriteDmaExtDesc(U64 lba, U32 sectorCount)
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(WRITE_TO_DEVICE, SKAtaCommandDesc::COMMAND_WRITE_DMA_EXT, COMMAND_48_BIT);
    cmdDesc->prepareLBASectorCount(lba, sectorCount);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createWriteSectorDesc(U64 lba, U32 sectorCount)
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(WRITE_TO_DEVICE, SKAtaCommandDesc::COMMAND_WRITE_SECTOR);
    cmdDesc->prepareLBASectorCount(lba, sectorCount);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createWriteSectorExtDesc(U64 lba, U32 sectorCount)
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(WRITE_TO_DEVICE, SKAtaCommandDesc::COMMAND_WRITE_SECTOR_EXT, COMMAND_48_BIT);
    cmdDesc->prepareLBASectorCount(lba, sectorCount);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createSecurityErasePrepareDesc()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(NONE, SKAtaCommandDesc::COMMAND_SECURITY_ERASE_PREPARE);
    cmdDesc->prepareLBASectorCount(0, 0);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createSecurityEraseUnitDesc()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(WRITE_TO_DEVICE, SKAtaCommandDesc::COMMAND_SECURITY_ERASE_UNIT);
    cmdDesc->prepareLBASectorCount(0, 1);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createSecurityUnlockDesc()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(WRITE_TO_DEVICE, SKAtaCommandDesc::COMMAND_SECURITY_UNLOCK);
    cmdDesc->prepareLBASectorCount(0, 1);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createSecurityFrezzeLockDesc()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(NONE, SKAtaCommandDesc::COMMAND_SECURITY_FREEZE_LOCK);
    cmdDesc->prepareLBASectorCount(0, 0);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createSanitizeCryptoScrambleExtDesc()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(NONE, SKAtaCommandDesc::COMMAND_SANITIZE, COMMAND_48_BIT);
    cmdDesc->inputFields.Feature = SKAtaCommandDesc::FEATURE_SANITIZE_CRYPTO_SCRAMBLE_EXT;
    cmdDesc->prepareLBASectorCount(SKAtaCommandDesc::LBA_SANITIZE_CRYPTO_SCRAMBLE_EXT, 0);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createSanitizeOverwriteExtDesc()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(NONE, SKAtaCommandDesc::COMMAND_SANITIZE, COMMAND_48_BIT);
    cmdDesc->inputFields.Feature = SKAtaCommandDesc::FEATURE_SANITIZE_OVERWRITE_EXT;
    cmdDesc->prepareLBASectorCount(SKAtaCommandDesc::LBA_SANITIZE_OVERWRITE_EXT, 0);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createSanitizeBlockEraseExtDesc()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(NONE, SKAtaCommandDesc::COMMAND_SANITIZE, COMMAND_48_BIT);
    cmdDesc->inputFields.Feature = SKAtaCommandDesc::FEATURE_SANITIZE_BLOCK_ERASE_EXT;
    cmdDesc->prepareLBASectorCount(SKAtaCommandDesc::LBA_SANITIZE_BLOCK_ERASE_EXT, 0);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createSanitizeStatusExtDesc()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(NONE, SKAtaCommandDesc::COMMAND_SANITIZE, COMMAND_48_BIT);
    cmdDesc->inputFields.Feature = SKAtaCommandDesc::FEATURE_SANITIZE_STATUS_EXT;
    cmdDesc->prepareLBASectorCount(0, 0);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createAtaSleepDesc()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(NONE, SKAtaCommandDesc::COMMAND_SLEEP);
    cmdDesc->prepareLBASectorCount(0, 0);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createTrustedSendDesc(U8 protocol, U16 comID, U32 sectorCount)
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(WRITE_TO_DEVICE, SKAtaCommandDesc::COMMAND_TRUSTED_SEND);
    cmdDesc->inputFields.Feature = protocol;
    U64 lba = ((((U32)comID) << 8) | (((sectorCount) >> 8) & 0xFF)) | (1 << 24);
    cmdDesc->prepareLBASectorCount(lba, sectorCount);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createTrustedReceiveDesc(U8 protocol, U16 comID, U32 sectorCount)
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(READ_FROM_DEVICE, SKAtaCommandDesc::COMMAND_TRUSTED_RECEIVE);
    cmdDesc->inputFields.Feature = protocol;
    U64 lba = (((U32)comID) << 8) | (((sectorCount) >> 8) & 0xFF);
    cmdDesc->prepareLBASectorCount(lba, sectorCount);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createSecuritySetPasswordDesc()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(WRITE_TO_DEVICE, SKAtaCommandDesc::COMMAND_SECURITY_SET_PASSWORD);
    cmdDesc->prepareLBASectorCount(0, 1);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createSecurityDisablePasswordDesc()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(WRITE_TO_DEVICE, SKAtaCommandDesc::COMMAND_SECURITY_DISABLE_PASSWORD);
    cmdDesc->prepareLBASectorCount(0, 1);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createGetQuickEraseMethodDesc()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(READ_FROM_DEVICE, SKAtaCommandDesc::COMMAND_QUICK_ERASE_GET_METHOD);
    cmdDesc->inputFields.Feature = SKAtaCommandDesc::FEATURE_QUICK_ERASE_GET_METHOD;
    cmdDesc->prepareLBASectorCount(0, 1);
    return cmdDesc;
}

SKAtaCommandDesc* SKAtaCommandDesc::createSetQuickEraseMethodDesc()
{
    SKAtaCommandDesc* cmdDesc = new SKAtaCommandDesc(WRITE_TO_DEVICE, SKAtaCommandDesc::COMMAND_QUICK_ERASE_SET_METHOD);
    cmdDesc->inputFields.Feature = SKAtaCommandDesc::FEATURE_QUICK_ERASE_SET_METHOD;
    cmdDesc->prepareLBASectorCount(0, 1);
    return cmdDesc;
}

void SKAtaCommandDesc::prepareLBASectorCount(U64 lba, U32 sectorCount)
{
    this->dataTransferLengthInSectors = sectorCount;
    if (false == this->isExtCommand)
    {
        this->inputFields.Count   = (U8)sectorCount;
        this->inputFields.LbaLow  = (lba      ) & 0x000000FF;
        this->inputFields.LbaMid  = (lba >> 8 ) & 0x000000FF;
        this->inputFields.LbaHigh = (lba >> 16) & 0x000000FF;
        this->inputFields.Device = (this->inputFields.Device & 0xF0) | ((lba >> 24) & 0x0F);
    }
    else
    {
        this->inputFields.Count      = (U8)sectorCount;
        this->inputFields.LbaLow     = (lba      ) & 0x000000FF;
        this->inputFields.LbaMid     = (lba >> 8 ) & 0x000000FF;
        this->inputFields.LbaHigh    = (lba >> 16) & 0x000000FF;

        this->inputFieldsExt.Count   = (sectorCount >> 8) & 0x000000FF;
        this->inputFieldsExt.LbaLow  = (lba >> 24 ) & 0x000000FF;
        this->inputFieldsExt.LbaMid  = (lba >> 32 ) & 0x000000FF;
        this->inputFieldsExt.LbaHigh = (lba >> 40 ) & 0x000000FF;
    }
}
