#include <string.h>

#include "StorageKitNvmeDevice.h"
#include "StorageKitNvmeCommandDesc.h"
#include "StorageKitNvmeProtocol.h"

using namespace std;

const U32 SKNvmeDevice::IDENTIFY_BUFFER_SIZE_IN_SECTORS = 8;

SKNvmeDevice::SKNvmeDevice(const std::string &devicePath, const DeviceHandle &deviceHandle, const int &nsid) :
    SKStorageDevice(NVME, devicePath), nsid(nsid)
{
    this->createProtocol(devicePath, deviceHandle);
}

SKNvmeDevice::~SKNvmeDevice()
{
    delete this->protocol;
    this->protocol = nullptr;
}

SKDeviceSpaceInfo* SKNvmeDevice::queryDeviceSpaceInfo()
{
    return this->protocol->queryDeviceSpaceInfo();
}

SKReturnCode SKNvmeDevice::queryBootableStatus(bool &isBootable)
{
    return this->protocol->queryBootableStatus(isBootable);
}

void SKNvmeDevice::createProtocol(const std::string &devicePath, const DeviceHandle &deviceHandle)
{
    this->protocol = new SKNvmeProtocol(devicePath, deviceHandle);
}

SKReturnCode SKNvmeDevice::issueCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    if (NVME_COMMAND != cmdDesc->getCommandType())
    {
        return SKErrorInvalidCommand;
    }

    const SKNvmeCommandDesc *nvmeCmdDesc = (const SKNvmeCommandDesc *)cmdDesc;
    return this->protocol->issueCommand(nvmeCmdDesc, buffer);
}

int SKNvmeDevice::getNsid()
{
    return nsid;
}

SKReturnCode SKNvmeDevice::identifyNamespace(SKAlignedBuffer *buffer, bool present)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc*)SKNvmeCommandDesc::createIdentifyNamespaceDesc(nsid, present);
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKNvmeDevice::identifyController(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc*)SKNvmeCommandDesc::createIdentifyControllerDesc();
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKNvmeDevice::getFirmwareSlotInformationLog(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc*)SKNvmeCommandDesc::createGetFirmwareSlotInformationLogDesc();
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKNvmeDevice::getSanitizeInformationLog(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc*)SKNvmeCommandDesc::createGetSanitizeInformationLogDesc();
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKNvmeDevice::firmwareDownload(const U32 &offset, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc*)SKNvmeCommandDesc::createFirmwareDownloadDesc(offset, buffer->GetSizeInByte());
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKNvmeDevice::firmwareCommit(const SKNvmeFirmwareSlot &slot, const SKNvmeFirmwareAction &action, const SKNvmeFirmwareBpid &bpid)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc*)SKNvmeCommandDesc::createFirmwareCommitDesc((U8)slot, (U8)action, (U8)bpid);
    SKReturnCode result = this->issueCommand(cmdDesc, nullptr);
    delete cmdDesc;

    return result;
}

SKReturnCode SKNvmeDevice::sanitize(const SKNvmeSanitizeAction &action, const SKNvmeSanitizeAUSE &ause, const SKNvmeSanitizeOWPASS &owpass,
                      const SKNvmeSanitizeOIPBP &oipbp, const SKNvmeSanitizeNoDeallocate &noDealloc, const U32 &overwritePattern)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc*)SKNvmeCommandDesc::createSanitizeDesc((U8)action, (U8)ause, (U8)owpass, (U8)oipbp, (U8)noDealloc, overwritePattern);
    SKReturnCode result = this->issueCommand(cmdDesc, nullptr);
    delete cmdDesc;

    return result;
}

SKReturnCode SKNvmeDevice::format(const SKNvmeFormatLBAF &lbaf, const SKNvmeFormatSES &ses, const SKNvmeFormatPI &pi,
                      const SKNvmeFormatPIL &pil, const SKNvmeFormatMS &ms, const U32 &timeoutInMs)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc*)SKNvmeCommandDesc::createFormatDesc(nsid, (U8)lbaf, (U8)ses, (U8)pi, (U8)pil, (U8)ms, timeoutInMs);
    SKReturnCode result = this->issueCommand(cmdDesc, nullptr);
    delete cmdDesc;

    return result;
}

SKReturnCode SKNvmeDevice::getSmart(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc*)SKNvmeCommandDesc::createSmartDesc(nsid);
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKNvmeDevice::write(const U64 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer)
{    
    SKCommandDesc *cmdDesc = (SKCommandDesc*)SKNvmeCommandDesc::createWriteDesc(lba, sectorCount);
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKNvmeDevice::writeExt(const U64 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc*)SKNvmeCommandDesc::createWriteExtDesc(lba, sectorCount);
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKNvmeDevice::read(const U64 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc*)SKNvmeCommandDesc::createReadDesc(lba, sectorCount);
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKNvmeDevice::readExt(const U64 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc*)SKNvmeCommandDesc::createReadExtDesc(lba, sectorCount);
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}
