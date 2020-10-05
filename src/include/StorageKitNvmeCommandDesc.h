#ifndef NvmeCommandDesc_h
#define NvmeCommandDesc_h

#include "StorageKitCommandDesc.h"

class SKNvmeCommandDesc : public SKCommandDesc
{
  public:
    struct SKAdminCommandInputFields
    {
        U8 OpCode;
        U32 Nsid;
        U32 Cdw10;
        U32 Cdw11;
        U32 Cdw12;
        U32 Cdw13;
        U32 TimeoutInMs;
    };

    struct SKIOCommandInputFields
    {
        U8 OpCode;
        U64 Lba;
        U32 SectorCount;
    };

    union SKCommandInputFields
    {
        U8 OpCode;
        SKAdminCommandInputFields AdminCommand;
        SKIOCommandInputFields IOCommand;
    };

    enum SKNvmeFieldFormatting
    {
        COMMAND_ADMIN,
        COMMAND_IO
    };

  public:
    static const U8 COMMAND_IDENTIFY_DEVICE;
    static const U8 COMMAND_GET_LOG_PAGE;
    static const U8 COMMAND_DOWNLOAD_FIRMWARE;
    static const U8 COMMAND_ACTIVE_FIRMWARE;
    static const U8 COMMAND_SANITIZE;
    static const U8 COMMAND_FORMAT;
    static const U8 COMMAND_SMART;
    static const U8 COMMAND_WRITE;
    static const U8 COMMAND_READ;

    // identify namespace
    static const U8 IDENTIFY_NAMESPACE_CNS;
    static const U8 IDENTIFY_NAMESPACE_CNS_PRESENT;

    // identify controller
    static const U8 IDENTIFY_CONTROLLER_NSID;
    static const U8 IDENTIFY_CONTROLLER_CNS;

    // log page
    static const U8 LOG_PAGE_ID_HEALTH_INFO;
    static const U8 LOG_PAGE_ID_FIRMWARE_SLOT_INFO;
    static const U8 LOG_PAGE_ID_SANITIZE_INFO;

    static const U32 IDENTIFY_BUFFER_SIZE_IN_SECTORS;
    static const U32 NSID_ALL;

  public:
    SKCommandInputFields inputFields;
    SKDataAccess dataAccess;
    SKNvmeFieldFormatting fieldFormatting;
    U32 dataTransferLengthInSectors;
    bool isExtCommand;

  protected:
    SKNvmeCommandDesc(const SKDataAccess &dataAccess, const U8 &commandCode,
                   const SKNvmeFieldFormatting &fieldFormatting);

  public:
    // Create SKCommandDesc functions
    static SKNvmeCommandDesc* createIdentifyControllerDesc();
    static SKNvmeCommandDesc* createIdentifyNamespaceDesc(const U32 &nsid, bool present);
    static SKNvmeCommandDesc* createGetFirmwareSlotInformationLogDesc();
    static SKNvmeCommandDesc* createGetSanitizeInformationLogDesc();
    static SKNvmeCommandDesc* createFirmwareDownloadDesc(const U32 &offset, const U32 &dataInBytes);
    static SKNvmeCommandDesc* createFirmwareCommitDesc(const U8 &slot, const U8 &action, const U8 &bpid);
    static SKNvmeCommandDesc* createSanitizeDesc(const U8 &sanact, const U8 &ause, const U8 &owpass,
                                                 const U8 &oipbp, const U8 &noDealloc, const U32 &ovrpat);
    static SKNvmeCommandDesc* createFormatDesc(const U32 &nsid, const U8 &lbaf, const U8 &ses,
                                                 const U8 &pi, const U8 &pil, const U32 &ms, const U32 &timeoutInMs);
    static SKNvmeCommandDesc* createSmartDesc(const U32 &nsid);
    static SKNvmeCommandDesc* createWriteDesc(const U64 &lba, const U32 &sectorCount);
    static SKNvmeCommandDesc* createWriteExtDesc(const U64 &lba, const U32 &sectorCount);
    static SKNvmeCommandDesc* createReadDesc(const U64 &lba, const U32 &sectorCount);
    static SKNvmeCommandDesc* createReadExtDesc(const U64 &lba, const U32 &sectorCount);
};

#endif
