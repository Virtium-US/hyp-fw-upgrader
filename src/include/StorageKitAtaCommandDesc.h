#ifndef AtaCommandDesc_h
#define AtaCommandDesc_h

#include "StorageKitCommandDesc.h"

struct SKAtaCommandInputFields
{
    U8 Feature;
    U8 Count;
    U8 LbaLow;
    U8 LbaMid;
    U8 LbaHigh;
    U8 Device;
    U8 Command;
    U8 Reserved;
};

struct SKAtaCommandOutputFields
{
    U8 Error;
    U8 Count;
    U8 LbaLow;
    U8 LbaMid;
    U8 LbaHigh;
    U8 Device;
    U8 Status;
    U8 Reserved;
};

enum SKAtaFieldFormatting
{
    COMMAND_28_BIT,
    COMMAND_48_BIT
};

enum SKTransferMode
{
    DMA,
    PIO
};

class SKAtaCommandDesc : public SKCommandDesc
{
  public:
    static const U8 COMMAND_IDENTIFY_DEVICE;
    static const U8 COMMAND_SMART;
    static const U8 COMMAND_READ_SECTOR;
    static const U8 COMMAND_READ_SECTOR_EXT;
    static const U8 COMMAND_READ_DMA;
    static const U8 COMMAND_READ_DMA_EXT;
    static const U8 COMMAND_WRITE_DMA;
    static const U8 COMMAND_WRITE_DMA_EXT;
    static const U8 COMMAND_WRITE_SECTOR;
    static const U8 COMMAND_WRITE_SECTOR_EXT;
    static const U8 COMMAND_SECURITY_DISABLE_PASSWORD;
    static const U8 COMMAND_SECURITY_SET_PASSWORD;
    static const U8 COMMAND_SECURITY_UNLOCK;
    static const U8 COMMAND_SECURITY_FREEZE_LOCK;
    static const U8 COMMAND_SECURITY_ERASE_PREPARE;
    static const U8 COMMAND_SECURITY_ERASE_UNIT;
    static const U8 COMMAND_SANITIZE;
    static const U8 COMMAND_SLEEP;
    static const U8 COMMAND_TRUSTED_SEND;
    static const U8 COMMAND_TRUSTED_RECEIVE;
    static const U8 COMMAND_QUICK_ERASE_SET_METHOD;
    static const U8 COMMAND_QUICK_ERASE_GET_METHOD;

  private:
    //Smart feature
    static const U8 FEATURE_SMART_DISABLE;
    static const U8 FEATURE_SMART_ENABLE;
    static const U8 FEATURE_SMART_READ_DATA;
    static const U8 FEATURE_SMART_READ_THRESHOLD;
    static const U8 FEATURE_QUICK_ERASE_GET_METHOD;
    static const U8 FEATURE_QUICK_ERASE_SET_METHOD;

    //Santitize feature
    static const U8 FEATURE_SANITIZE_CRYPTO_SCRAMBLE_EXT;
    static const U8 FEATURE_SANITIZE_OVERWRITE_EXT;
    static const U8 FEATURE_SANITIZE_BLOCK_ERASE_EXT;
    static const U8 FEATURE_SANITIZE_STATUS_EXT;

    static const U64 LBA_SANITIZE_CRYPTO_SCRAMBLE_EXT;
    static const U64 LBA_SANITIZE_OVERWRITE_EXT;
    static const U64 LBA_SANITIZE_BLOCK_ERASE_EXT;

    static const U8 DEFAULT_DEVICE_VALUE;

  public:
    SKAtaCommandInputFields inputFields;
    SKAtaCommandInputFields inputFieldsExt;
    SKDataAccess dataAccess;
    SKTransferMode transferMode;
    U32 dataTransferLengthInSectors;
    bool isExtCommand;

  public:
    SKAtaCommandDesc(const SKAtaCommandDesc &desc);

  private:
    SKAtaCommandDesc(const SKDataAccess &dataAccess, const U8 &commandCode,
                   const SKAtaFieldFormatting &fieldFormatting, const SKTransferMode &transferMode);

  public:
    // Create SKCommandDesc functions
    static SKAtaCommandDesc* createIdentifyDeviceDesc();
    static SKAtaCommandDesc* createReadSmartData();
    static SKAtaCommandDesc* createReadSmartThreshold();
    static SKAtaCommandDesc* createReadSectorDesc(U64 lba, U32 sectorCount);
    static SKAtaCommandDesc* createReadSectorExtDesc(U64 lba, U32 sectorCount);
    static SKAtaCommandDesc* createReadDmaDesc(U64 lba, U32 sectorCount);
    static SKAtaCommandDesc* createReadDmaExtDesc(U64 lab, U32 sectorCount);
    static SKAtaCommandDesc* createWriteSectorDesc(U64 lba, U32 sectorCount);
    static SKAtaCommandDesc* createWriteSectorExtDesc(U64 lba, U32 sectorCount);
    static SKAtaCommandDesc* createWriteDmaDesc(U64 lba, U32 sectorCount);
    static SKAtaCommandDesc* createWriteDmaExtDesc(U64 lba, U32 sectorCount);
    static SKAtaCommandDesc* createSecurityErasePrepareDesc();
    static SKAtaCommandDesc* createSecurityEraseUnitDesc();
    static SKAtaCommandDesc* createSecurityUnlockDesc();
    static SKAtaCommandDesc* createSecurityFrezzeLockDesc();
    static SKAtaCommandDesc* createSanitizeCryptoScrambleExtDesc();
    static SKAtaCommandDesc* createSanitizeOverwriteExtDesc();
    static SKAtaCommandDesc* createSanitizeBlockEraseExtDesc();
    static SKAtaCommandDesc* createSanitizeStatusExtDesc();
    static SKAtaCommandDesc* createAtaSleepDesc();
    static SKAtaCommandDesc* createTrustedSendDesc(U8 protocol, U16 comID, U32 sectorCount);
    static SKAtaCommandDesc* createTrustedReceiveDesc(U8 protocol, U16 comID, U32 sectorCount);
    static SKAtaCommandDesc* createSecuritySetPasswordDesc();
    static SKAtaCommandDesc* createSecurityDisablePasswordDesc();
    static SKAtaCommandDesc* createGetQuickEraseMethodDesc();
    static SKAtaCommandDesc* createSetQuickEraseMethodDesc();

  public:
    void prepareLBASectorCount(U64 lba, U32 sectorCount);
};

#endif
