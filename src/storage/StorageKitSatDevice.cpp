#include "StorageKitSatDevice.h"
#include "StorageKitAtaCommandDesc.h"
#include "StorageKitScsiProtocol.h"

using namespace std;

SKSatDevice::SKSatDevice(const std::string &devicePath, const DeviceHandle &deviceHandle) :
    SKAtaDevice(SAT, devicePath)
{
    this->createProtocol(devicePath, deviceHandle);
}

SKSatDevice::~SKSatDevice()
{
    delete this->scsiProtocol;
    this->scsiProtocol = nullptr;
}

SKAtaOutputTaskFileRegister SKSatDevice::getLastOutputTaskFileRegister() const
{
    return this->scsiProtocol->getLastOutputTaskFileRegister();
}

SKDeviceSpaceInfo* SKSatDevice::queryDeviceSpaceInfo()
{
    return this->scsiProtocol->queryDeviceSpaceInfo();
}

SKReturnCode SKSatDevice::queryBootableStatus(bool &isBootable)
{
    return this->scsiProtocol->queryBootableStatus(isBootable);
}

void SKSatDevice::createProtocol(const std::string &devicePath, const DeviceHandle &deviceHandle)
{
    this->scsiProtocol = new SKScsiProtocol(devicePath, deviceHandle);
}

SKReturnCode SKSatDevice::issueCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    switch (cmdDesc->getCommandType())
    {
        case ATA_COMMAND:
        {
            return this->handleAtaCommand(cmdDesc, buffer);
        }
        default:
        {
            break;
        }
    }
    return SKErrorInvalidCommand;
}

SKReturnCode SKSatDevice::handleAtaCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    if (ATA_COMMAND != cmdDesc->getCommandType())
    {
        return SKErrorInvalidCommand;
    }

    const SKAtaCommandDesc *ataCmdDesc = (const SKAtaCommandDesc *)cmdDesc;
    return this->scsiProtocol->issueAtaCommand(ataCmdDesc, buffer);
}
