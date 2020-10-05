#include "StorageKitStorageDevice.h"

using namespace std;

SKStorageDevice::SKStorageDevice(const SKDeviceType &type, const std::string &devicePath)
{
    this->deviceType = type;
    this->devicePath = devicePath;
}

SKStorageDevice::~SKStorageDevice()
{
}

SKDeviceType SKStorageDevice::getDeviceType() const
{
    return this->deviceType;
}

string SKStorageDevice::getDevicePath() const
{
    return this->devicePath;
}

SKReturnCode SKStorageDevice::issueCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    return SKErrorInvalidCommand;
}

SKDeviceSpaceInfo* SKStorageDevice::queryDeviceSpaceInfo()
{
    // implement in inheritance
    return nullptr;
}

SKReturnCode SKStorageDevice::queryBootableStatus(bool &isBootable)
{
    // implement in inheritance
    return SKReturnCode::SKErrorUnknown;
}
