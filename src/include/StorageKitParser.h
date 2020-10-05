#ifndef CoreDelegateParser_h
#define CoreDelegateParser_h

#include "StorageKitStructures.h"
#include "StorageKitTypes.h"

enum SKVendorCode
{
    eUnknownVendor = 0,
    eRealtekDevice,
    eVirtiumDevice,
};

class SKParser
{
  public:
    static SKDeviceInfo* parseAtaIdentifyBuffer(const U8 *buffer, const std::string &devicePath, const SKDeviceType &deviceType);
    static SKDeviceInfo* parseNvmeIdentifyBuffer(const U8 *controllerBuffer, const U8 *namespaceBuffer, const U8 *firmwareSlotInfoLogBuffer,
                                                     const int &nsid, const std::string &devicePath, const SKDeviceType &deviceType);
    static SKSmartInfo* parseAtaSmartBuffer(const U8 *buffer, const SKDeviceType &deviceType);
    static SKSmartInfo* parseNvmeSmartBuffer(const U8 *buffer, const SKDeviceType &deviceType);
};

#endif
