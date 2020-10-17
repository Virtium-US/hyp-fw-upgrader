#ifndef U9VcCommandDesc_h
#define U9VcCommandDesc_h

#include "StorageKitScsiCommandDesc.h"

class SKU9VcCommandDesc : public SKScsiCommandDesc
{
  public:
    static const U8 COMMAND_U9_VC_TUNNEL;

    static const U8 U9_VSC;
    static const U8 VC_TRIM_COMMAND_CODE;

  private:
    SKU9VcCommandDesc(const SKDataAccess &dataAccess, const U8 &commandCode,
                 const SKScsiFieldFormatting &fieldFormatting);

  public:
    static SKScsiCommandDesc* createTrimAddressRangeDesc();
    static SKScsiCommandDesc* createTargetInfo();
    static SKScsiCommandDesc* createSetAddressExtension(const U16 extensionAddress);
    static SKScsiCommandDesc* createSetBaseAddress();
    static SKScsiCommandDesc* createReadFirmwareVersion();
    static SKScsiCommandDesc* createFirmwareUpdatePrepare();
    static SKScsiCommandDesc* createFirmwareUpdateTransfer(const U16 SN);
    static SKScsiCommandDesc* createFirmwareUpdateExecute();

  public:
    void prepareCommandHypVc(const U8 &cmdSet, const U8 &cmdCode, const U16 &sectorNumber, const SKHypVcDirection &direction);

  private:
    static U32 buildCommandCode(const U32 CS, const U32 SN, const U32 CC, const U32 RW);
    static void setCommandCode(U8* cdb, U32 cmdCode);
};

#endif
