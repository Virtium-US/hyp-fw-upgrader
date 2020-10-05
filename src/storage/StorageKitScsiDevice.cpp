#include "StorageKitScsiDevice.h"
#include "StorageKitScsiCommandDesc.h"
#include "StorageKitScsiProtocol.h"

using namespace std;

SKScsiDevice::SKScsiDevice(const std::string &devicePath, const DeviceHandle &deviceHandle) :
    SKStorageDevice(SCSI, devicePath)
{
    this->createProtocol(devicePath, deviceHandle);
}

SKScsiDevice::~SKScsiDevice()
{
    delete this->protocol;
    this->protocol = nullptr;
}

SKDeviceSpaceInfo* SKScsiDevice::queryDeviceSpaceInfo()
{
    return this->protocol->queryDeviceSpaceInfo();
}

SKReturnCode SKScsiDevice::queryBootableStatus(bool &isBootable)
{
    return this->protocol->queryBootableStatus(isBootable);
}

void SKScsiDevice::createProtocol(const std::string &devicePath, const DeviceHandle &deviceHandle)
{
    this->protocol = new SKScsiProtocol(devicePath, deviceHandle);
}

SKReturnCode SKScsiDevice::issueCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    switch (cmdDesc->getCommandType())
    {
        case SCSI_COMMAND:
        {
            return this->handleScsiCommand(cmdDesc, buffer);
        }
        default:
        {
            break;
        }
    }
    return SKErrorInvalidCommand;
}

SKReturnCode SKScsiDevice::handleScsiCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    if (SCSI_COMMAND != cmdDesc->getCommandType())
    {
        return SKErrorInvalidCommand;
    }

    const SKScsiCommandDesc *scsiCmdDesc = (const SKScsiCommandDesc *)cmdDesc;
    return this->protocol->issueScsiCommand(scsiCmdDesc, buffer);
}

SKReturnCode SKScsiDevice::scsiInquiry(const SKInquiryPageCode &pageCode, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKScsiCommandDesc::createInquiryDesc(pageCode);
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKScsiDevice::scsiWrite6(const U32 &lba, const U16 &sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKScsiCommandDesc::createWrite6Desc(lba, sectorCount);
    if (cmdDesc == nullptr)
    {
        return SKErrorInvalidParameters;
    }

    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;
    return result;
}

SKReturnCode SKScsiDevice::scsiWrite10(const U32 &lba, const U16 &sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKScsiCommandDesc::createWrite10Desc(lba, sectorCount);
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKScsiDevice::scsiWrite12(const U32 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKScsiCommandDesc::createWrite12Desc(lba, sectorCount);
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKScsiDevice::scsiWrite16(const U64 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKScsiCommandDesc::createWrite16Desc(lba, sectorCount);
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKScsiDevice::scsiRead6(const U32 &lba, const U16 &sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKScsiCommandDesc::createRead6Desc(lba, sectorCount);
    if (cmdDesc == nullptr)
    {
        return SKErrorInvalidParameters;
    }

    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKScsiDevice::scsiRead10(const U32 &lba, const U16 &sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKScsiCommandDesc::createRead10Desc(lba, sectorCount);
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKScsiDevice::scsiRead12(const U32 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKScsiCommandDesc::createRead12Desc(lba, sectorCount);
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKScsiDevice::scsiRead16(const U64 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKScsiCommandDesc::createRead16Desc(lba, sectorCount);
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKScsiDevice::scsiReadCapacity(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKScsiCommandDesc::createReadCapacityDesc();
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}
