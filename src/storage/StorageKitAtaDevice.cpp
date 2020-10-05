#include <memory.h>

#include "StorageKitAtaDevice.h"
#include "StorageKitAtaCommandDesc.h"
#include "StorageKitAtaProtocol.h"

using namespace std;

SKAtaDevice::SKAtaDevice(const std::string &devicePath, const DeviceHandle &deviceHandle) :
    SKAtaDevice(ATA, devicePath)
{
    this->createProtocol(devicePath, deviceHandle);
}

SKAtaDevice::SKAtaDevice(const SKDeviceType &type, const std::string &devicePath) :
    SKStorageDevice(type, devicePath), protocol(nullptr)
{
    // Do nothing
}

SKAtaDevice::~SKAtaDevice()
{
    delete this->protocol;
    this->protocol = nullptr;
}

SKDeviceSpaceInfo* SKAtaDevice::queryDeviceSpaceInfo()
{
    return this->protocol->queryDeviceSpaceInfo();
}

SKReturnCode SKAtaDevice::queryBootableStatus(bool &isBootable)
{
    return this->protocol->queryBootableStatus(isBootable);
}

SKAtaOutputTaskFileRegister SKAtaDevice::getLastOutputTaskFileRegister() const
{
    return this->protocol->getLastOutputTaskFileRegister();
}

void SKAtaDevice::createProtocol(const std::string &devicePath, const DeviceHandle &deviceHandle)
{
    this->protocol = new SKAtaProtocol(devicePath, deviceHandle);
}

SKReturnCode SKAtaDevice::issueCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    if (ATA_COMMAND != cmdDesc->getCommandType())
    {
        return SKErrorInvalidCommand;
    }

    const SKAtaCommandDesc *ataCmdDesc = (const SKAtaCommandDesc *)cmdDesc;
    return this->protocol->issueCommand(ataCmdDesc, buffer);
}

SKReturnCode SKAtaDevice::ataIdentifyDevice(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createIdentifyDeviceDesc());
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataReadSmart(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKAtaCommandDesc::createReadSmartData();
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataReadSmartThreshold(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKAtaCommandDesc::createReadSmartThreshold();
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataReadSector(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createReadSectorDesc(lba, sectorCount));
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataReadSectorExt(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createReadSectorExtDesc(lba, sectorCount));
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataReadDma(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createReadDmaDesc(lba, sectorCount));
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataReadDmaExt(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createReadDmaExtDesc(lba, sectorCount));
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataWriteSector(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createWriteSectorDesc(lba, sectorCount));
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataWriteSectorExt(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createWriteSectorExtDesc(lba, sectorCount));
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataWriteDma(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createWriteDmaDesc(lba, sectorCount));
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataWriteDmaExt(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createWriteDmaExtDesc(lba, sectorCount));
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataSecurityErasePrepare()
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createSecurityErasePrepareDesc());
    SKReturnCode result = this->issueCommand(cmdDesc, nullptr);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataSecurityEraseUnit(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createSecurityEraseUnitDesc());
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataSecurityDisablePassword(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createSecurityDisablePasswordDesc());
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataSecuritySetPassword(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createSecuritySetPasswordDesc());
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataSecurityUnlock(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createSecurityUnlockDesc());
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataSecurityFreezeLock()
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createSecurityFrezzeLockDesc());
    SKReturnCode result = this->issueCommand(cmdDesc, nullptr);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataSanitizeCryptoScrambleExt()
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createSanitizeCryptoScrambleExtDesc());
    SKReturnCode result = this->issueCommand(cmdDesc, nullptr);
    delete cmdDesc;
    return result;
}

SKReturnCode SKAtaDevice::ataSanitizeOverwriteExt()
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createSanitizeOverwriteExtDesc());
    SKReturnCode result = this->issueCommand(cmdDesc, nullptr);
    delete cmdDesc;
    return result;
}

SKReturnCode SKAtaDevice::ataSanitizeBlockEraseExt()
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createSanitizeBlockEraseExtDesc());
    SKReturnCode result = this->issueCommand(cmdDesc, nullptr);
    delete cmdDesc;
    return result;
}

SKReturnCode SKAtaDevice::ataSleep()
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createAtaSleepDesc());
    SKReturnCode result = this->issueCommand(cmdDesc, nullptr);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataSanitizeStatusExt()
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createSanitizeStatusExtDesc());
    SKReturnCode result = this->issueCommand(cmdDesc, nullptr);
    delete cmdDesc;
    return result;
}

SKReturnCode SKAtaDevice::ataTrustedSend(U8 protocol, U16 comID, U32 sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createTrustedSendDesc(protocol, comID, sectorCount));
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::ataTrustedReceive(U8 protocol, U16 comID, U32 sectorCount, SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createTrustedReceiveDesc(protocol, comID, sectorCount));
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::getQuickEraseMethod(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createGetQuickEraseMethodDesc());
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKAtaDevice::setQuickEraseMethod(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = static_cast<SKCommandDesc*>(SKAtaCommandDesc::createSetQuickEraseMethodDesc());
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}
