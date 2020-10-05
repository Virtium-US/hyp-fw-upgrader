#ifndef StorageProtocol_h
#define StorageProtocol_h

#include <string>
#include <vector>

#include "defines.h"

#define BusTypeSata 0xB
#define BusTypeNvme 0x11

struct SKBaseDeviceInfo
{
    std::string devicePath;
    DeviceHandle deviceHandle;
    SKDeviceType type;
};

class SKStorageProtocol
{
  protected:
    std::string devicePath;
    DeviceHandle handle;
    U16 lastError;

  protected:
    SKStorageProtocol(const std::string &devicePath, const DeviceHandle &handle);
    virtual ~SKStorageProtocol();

  public:
    static void scan(std::vector<SKBaseDeviceInfo*> &devices);
    static SKBaseDeviceInfo* scan(const std::string &devicePath);
    SKDeviceSpaceInfo* queryDeviceSpaceInfo();
    SKReturnCode queryBootableStatus(bool &isBootable);
    U16 getLastError() const;
    DeviceHandle getDeviceHandle() const;
};

#endif
