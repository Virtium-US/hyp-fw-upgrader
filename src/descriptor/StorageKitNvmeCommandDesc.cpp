#include "StorageKitNvmeCommandDesc.h"

const U8 SKNvmeCommandDesc::COMMAND_IDENTIFY_DEVICE         = 0x06;
const U8 SKNvmeCommandDesc::COMMAND_GET_LOG_PAGE            = 0x02;
const U8 SKNvmeCommandDesc::COMMAND_ACTIVE_FIRMWARE         = 0x10;
const U8 SKNvmeCommandDesc::COMMAND_DOWNLOAD_FIRMWARE       = 0x11;
const U8 SKNvmeCommandDesc::COMMAND_SANITIZE                = 0x84;
const U8 SKNvmeCommandDesc::COMMAND_FORMAT                  = 0x80;
const U8 SKNvmeCommandDesc::COMMAND_SMART                   = 0x00;
const U8 SKNvmeCommandDesc::COMMAND_WRITE                   = 0x01;
const U8 SKNvmeCommandDesc::COMMAND_READ                    = 0x02;

const U8 SKNvmeCommandDesc::IDENTIFY_NAMESPACE_CNS = 0x00;
const U8 SKNvmeCommandDesc::IDENTIFY_NAMESPACE_CNS_PRESENT = 0x11;
const U8 SKNvmeCommandDesc::IDENTIFY_CONTROLLER_NSID = 0x00;
const U8 SKNvmeCommandDesc::IDENTIFY_CONTROLLER_CNS = 0x01;
const U8 SKNvmeCommandDesc::LOG_PAGE_ID_HEALTH_INFO = 0x02;
const U8 SKNvmeCommandDesc::LOG_PAGE_ID_FIRMWARE_SLOT_INFO = 0x03;
const U8 SKNvmeCommandDesc::LOG_PAGE_ID_SANITIZE_INFO = 0x81;
const U32 SKNvmeCommandDesc::IDENTIFY_BUFFER_SIZE_IN_SECTORS = 8;
const U32 SKNvmeCommandDesc::NSID_ALL = 0xffffffff;

SKNvmeCommandDesc::SKNvmeCommandDesc(const SKDataAccess &dataAccess, const U8 &commandCode,
                               const SKNvmeFieldFormatting &fieldFormatting = COMMAND_ADMIN) :
    SKCommandDesc(NVME_COMMAND)
{
    this->dataAccess = dataAccess;
    this->fieldFormatting = fieldFormatting;

    this->inputFields.OpCode = commandCode;
    this->dataTransferLengthInSectors = dataAccess == NONE ? 0 : 1;
    this->isExtCommand = false;
}

SKNvmeCommandDesc* SKNvmeCommandDesc::createIdentifyControllerDesc()
{
    SKNvmeCommandDesc* cmdDesc = new SKNvmeCommandDesc(READ_FROM_DEVICE, SKNvmeCommandDesc::COMMAND_IDENTIFY_DEVICE);
    cmdDesc->inputFields.AdminCommand.Nsid = IDENTIFY_CONTROLLER_NSID;
    cmdDesc->inputFields.AdminCommand.Cdw10 = IDENTIFY_CONTROLLER_CNS;
    cmdDesc->dataTransferLengthInSectors = IDENTIFY_BUFFER_SIZE_IN_SECTORS;
    return cmdDesc;
}

SKNvmeCommandDesc* SKNvmeCommandDesc::createIdentifyNamespaceDesc(const U32 &nsid, bool present)
{
    U32 cns = (true == present ? IDENTIFY_NAMESPACE_CNS_PRESENT : IDENTIFY_NAMESPACE_CNS);

    SKNvmeCommandDesc* cmdDesc = new SKNvmeCommandDesc(READ_FROM_DEVICE, SKNvmeCommandDesc::COMMAND_IDENTIFY_DEVICE);
    cmdDesc->inputFields.AdminCommand.Nsid = nsid;
    cmdDesc->inputFields.AdminCommand.Cdw10 = cns;
    cmdDesc->dataTransferLengthInSectors = IDENTIFY_BUFFER_SIZE_IN_SECTORS;
    return cmdDesc;
}

SKNvmeCommandDesc* SKNvmeCommandDesc::createGetFirmwareSlotInformationLogDesc()
{
    SKNvmeCommandDesc* cmdDesc = new SKNvmeCommandDesc(READ_FROM_DEVICE, SKNvmeCommandDesc::COMMAND_GET_LOG_PAGE);
    cmdDesc->inputFields.AdminCommand.Nsid = NSID_ALL;

    U32 transferLengthInBytes = cmdDesc->dataTransferLengthInSectors * SECTOR_SIZE_IN_BYTES;
    U32 numd = (transferLengthInBytes >> 2) - 1;
    U32 numdu = (numd >> 16);
    U32 numdl = (numd & 0xFFFF);

    U8 logId = LOG_PAGE_ID_FIRMWARE_SLOT_INFO;
    U8 offset = 0;
    cmdDesc->inputFields.AdminCommand.Cdw10 = logId | (numdl << 16);
    cmdDesc->inputFields.AdminCommand.Cdw11 = numdu;
    cmdDesc->inputFields.AdminCommand.Cdw12 = offset;
    cmdDesc->inputFields.AdminCommand.Cdw13 = (offset >> 32);
    return cmdDesc;
}

SKNvmeCommandDesc* SKNvmeCommandDesc::createGetSanitizeInformationLogDesc()
{
    SKNvmeCommandDesc* cmdDesc = new SKNvmeCommandDesc(READ_FROM_DEVICE, SKNvmeCommandDesc::COMMAND_GET_LOG_PAGE);
    cmdDesc->inputFields.AdminCommand.Nsid = 0;

    U32 transferLengthInBytes = cmdDesc->dataTransferLengthInSectors * SECTOR_SIZE_IN_BYTES;
    U32 numd = (transferLengthInBytes >> 2) - 1;
    U32 numdu = (numd >> 16);
    U32 numdl = (numd & 0xFFFF);

    U8 logId = LOG_PAGE_ID_SANITIZE_INFO;
    cmdDesc->inputFields.AdminCommand.Cdw10 = logId | (numdl << 16);
    cmdDesc->inputFields.AdminCommand.Cdw11 = numdu;
    return cmdDesc;
}

SKNvmeCommandDesc* SKNvmeCommandDesc::createFirmwareDownloadDesc(const U32 &offset, const U32 &dataInBytes)
{
    SKNvmeCommandDesc* cmdDesc = new SKNvmeCommandDesc(WRITE_TO_DEVICE, SKNvmeCommandDesc::COMMAND_DOWNLOAD_FIRMWARE);
    cmdDesc->inputFields.AdminCommand.Cdw10 = (dataInBytes >> 2) - 1;
    cmdDesc->inputFields.AdminCommand.Cdw11 = (offset >> 2);
    cmdDesc->dataTransferLengthInSectors = (dataInBytes / SECTOR_SIZE_IN_BYTES);
    return cmdDesc;
}

SKNvmeCommandDesc* SKNvmeCommandDesc::createFirmwareCommitDesc(const U8 &slot, const U8 &action, const U8 &bpid)
{
    SKNvmeCommandDesc* cmdDesc = new SKNvmeCommandDesc(NONE, SKNvmeCommandDesc::COMMAND_ACTIVE_FIRMWARE);
    cmdDesc->inputFields.AdminCommand.Cdw10 = (bpid << 31) | (action << 3) | slot;
    return cmdDesc;
}

SKNvmeCommandDesc* SKNvmeCommandDesc::createSanitizeDesc(const U8 &sanact, const U8 &ause, const U8 &owpass,
                                                         const U8 &oipbp, const U8 &noDealloc, const U32 &ovrpat)
{
    SKNvmeCommandDesc* cmdDesc = new SKNvmeCommandDesc(WRITE_TO_DEVICE, SKNvmeCommandDesc::COMMAND_SANITIZE);
    cmdDesc->inputFields.AdminCommand.Cdw10 = (noDealloc << 9) | (oipbp << 8) | (owpass << 4) | (ause << 3) | sanact;
    cmdDesc->inputFields.AdminCommand.Cdw11 = ovrpat;
    cmdDesc->dataTransferLengthInSectors = SECTOR_SIZE_IN_BYTES;
    return cmdDesc;
}

SKNvmeCommandDesc* SKNvmeCommandDesc::createFormatDesc(const U32 &nsid, const U8 &lbaf, const U8 &ses,
                                                 const U8 &pi, const U8 &pil, const U32 &ms, const U32 &timeoutInMs)
{
    SKNvmeCommandDesc* cmdDesc = new SKNvmeCommandDesc(WRITE_TO_DEVICE, SKNvmeCommandDesc::COMMAND_FORMAT);
    cmdDesc->inputFields.AdminCommand.Nsid = nsid;

    U32 cdw10 = lbaf | (ms << 4) | (pi << 5) | (pil << 8) | (ses << 9);

    cmdDesc->inputFields.AdminCommand.Cdw10 = cdw10;
    cmdDesc->inputFields.AdminCommand.TimeoutInMs = timeoutInMs;
    cmdDesc->dataTransferLengthInSectors = SECTOR_SIZE_IN_BYTES;
    return cmdDesc;
}

SKNvmeCommandDesc* SKNvmeCommandDesc::createSmartDesc(const U32 &nsid)
{    
    SKNvmeCommandDesc* cmdDesc = new SKNvmeCommandDesc(READ_FROM_DEVICE, SKNvmeCommandDesc::COMMAND_SMART);
    cmdDesc->inputFields.AdminCommand.Cdw10 = LOG_PAGE_ID_HEALTH_INFO;
    cmdDesc->inputFields.AdminCommand.Nsid = nsid;
    return cmdDesc;
}

SKNvmeCommandDesc* SKNvmeCommandDesc::createWriteDesc(const U64 &lba, const U32 &sectorCount)
{
    SKNvmeCommandDesc* cmdDesc = new SKNvmeCommandDesc(WRITE_TO_DEVICE, SKNvmeCommandDesc::COMMAND_WRITE, COMMAND_IO);
    cmdDesc->inputFields.IOCommand.Lba = lba;
    cmdDesc->inputFields.IOCommand.SectorCount = sectorCount;
    return cmdDesc;
}

SKNvmeCommandDesc* SKNvmeCommandDesc::createWriteExtDesc(const U64 &lba, const U32 &sectorCount)
{
    SKNvmeCommandDesc* cmdDesc = new SKNvmeCommandDesc(WRITE_TO_DEVICE, SKNvmeCommandDesc::COMMAND_WRITE, COMMAND_IO);
    cmdDesc->inputFields.IOCommand.Lba = lba;
    cmdDesc->inputFields.IOCommand.SectorCount = sectorCount;
    cmdDesc->isExtCommand = true;
    return cmdDesc;
}

SKNvmeCommandDesc* SKNvmeCommandDesc::createReadDesc(const U64 &lba, const U32 &sectorCount)
{
    SKNvmeCommandDesc* cmdDesc = new SKNvmeCommandDesc(READ_FROM_DEVICE, SKNvmeCommandDesc::COMMAND_READ, COMMAND_IO);
    cmdDesc->inputFields.IOCommand.Lba = lba;
    cmdDesc->inputFields.IOCommand.SectorCount = sectorCount;
    return cmdDesc;
}

SKNvmeCommandDesc* SKNvmeCommandDesc::createReadExtDesc(const U64 &lba, const U32 &sectorCount)
{
    SKNvmeCommandDesc* cmdDesc = new SKNvmeCommandDesc(READ_FROM_DEVICE, SKNvmeCommandDesc::COMMAND_READ, COMMAND_IO);
    cmdDesc->inputFields.IOCommand.Lba = lba;
    cmdDesc->inputFields.IOCommand.SectorCount = sectorCount;
    cmdDesc->isExtCommand = true;
    return cmdDesc;
}
