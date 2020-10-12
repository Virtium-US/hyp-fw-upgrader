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
    static SKScsiCommandDesc* createReadFirmwareVersion();

  public:
    void prepareCommandHypVc(const U8 &cmdSet, const U8 &cmdCode, const U16 &sectorNumber, const SKHypVcDirection &direction);
};

#endif
