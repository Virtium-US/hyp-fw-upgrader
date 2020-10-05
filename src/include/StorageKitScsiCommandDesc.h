#ifndef ScsiCommandDesc_h
#define ScsiCommandDesc_h

#include "StorageKitCommandDesc.h"

#define CDB_MAX_LENGTH 16

#pragma pack(push, 1) // exact fit - no padding
struct SKCommandInquiryInputFields
{
    U8 OpCode;
    U8 Evpd: 1, Reserved: 7;
    U8 PageCode;
    U16 Length;
    U8 Control;
};

struct SKCommand6InputFields
{
    U8 OpCode;
    U8 MiscCdb: 3, LbaHigh_16_21: 5;    // bit 16 - 21
    U8 LbaLow[2];                       // bit 0 - 15
    U8 Length;
    U8 Control;
};

struct SKCommand10InputFields
{
    U8 OpCode;
    U8 MiscCdb: 3, Service: 5;
    U8 Lba[4];
    U8 MiscCdb2;
    U8 Length[2];
    U8 Control;
};

struct SKCommandHypVcInputFields
{
    U8 OpCode;
    U8 Reserved1;
    U8 Direction: 1, CmdCode: 7;
    U8 LowSectorNumber;
    U8 HighSectorNumber;
    U8 CmdSet;
    U16 Reserved2;
};

struct SKCommand12InputFields
{
    U8 OpCode;
    U8 MiscCdb_Service;
    U8 Lba[4];
    U8 Length[4];
    U8 MiscCdb;
    U8 Control;
};

struct SKCommand16InputFields
{
    U8 OpCode;
    U8 MiscCdb: 3, Service: 5;
    U8 Lba[8];
    U8 Length[4];
    U8 MiscCdb2;
    U8 Control;
};
#pragma pack(pop)

union SKCommandInputFields
{
    U8 OpCode;
    SKCommandInquiryInputFields CommandInquiry;
    SKCommand6InputFields Command6;
    SKCommand10InputFields Command10;
    SKCommandHypVcInputFields CommandHypVc;
    SKCommand12InputFields Command12;
    SKCommand16InputFields Command16;
    U8 Cdb[CDB_MAX_LENGTH];
};

enum SKScsiFieldFormatting
{
    COMMAND_6 = 0x0060,
    COMMAND_10 = 0x00A0,
    COMMAND_HYP_VC = 0x00A1,
    COMMAND_12 = 0x00C0,
    COMMAND_16 = 0x0100
};

// HYP Vendor Specific Command Code Direction
enum SKHypVcDirection
{
    WRITE = 0,
    READ = 1
};

class SKScsiCommandDesc : public SKCommandDesc
{
  public:
    static const U8 COMMAND_INQUIRY;
    static const U8 COMMAND_READ_6;
    static const U8 COMMAND_READ_10;
    static const U8 COMMAND_READ_12;
    static const U8 COMMAND_READ_16;
    static const U8 COMMAND_WRITE_6;
    static const U8 COMMAND_WRITE_10;
    static const U8 COMMAND_WRITE_12;
    static const U8 COMMAND_WRITE_16;
    static const U8 COMMAND_READ_CAPACITY;

  public:
    SKCommandInputFields inputFields;
    SKDataAccess dataAccess;
    SKScsiFieldFormatting fieldFormatting;
    U32 dataTransferLengthInSectors;
    U8 cdbLength;

  protected:
    SKScsiCommandDesc(const SKDataAccess &dataAccess, const U8 &commandCode,
                   const SKScsiFieldFormatting &fieldFormatting);

  public:
    // Create SKCommandDesc functions
    static SKScsiCommandDesc* createInquiryDesc(const SKInquiryPageCode &pageCode);

    static SKScsiCommandDesc* createRead6Desc(const U32 &lba, const U16 &sectorCount);
    static SKScsiCommandDesc* createRead10Desc(const U32 &lba, const U16 &sectorCount);
    static SKScsiCommandDesc* createRead12Desc(const U32 &lba, const U32 &sectorCount);
    static SKScsiCommandDesc* createRead16Desc(const U64 &lba, const U32 &sectorCount);

    static SKScsiCommandDesc* createWrite6Desc(const U32 &lba, const U16 &sectorCount);
    static SKScsiCommandDesc* createWrite10Desc(const U32 &lba, const U16 &sectorCount);
    static SKScsiCommandDesc* createWrite12Desc(const U32 &lba, const U32 &sectorCount);
    static SKScsiCommandDesc* createWrite16Desc(const U64 &lba, const U32 &sectorCount);

    static SKScsiCommandDesc* createReadCapacityDesc();
};

#endif
