#ifndef NvmeProtocol_h
#define NvmeProtocol_h

#include "StorageKitAlignedBuffer.h"
#include "StorageKitNvmeCommandDesc.h"
#include "StorageKitStorageProtocol.h"
#include "StorageKitScsiProtocol.h"

class SKNvmeProtocol : public SKStorageProtocol
{
private:
    static const U8 NVME_STATUS_SUCCESS_COMPLETION;
    static const U8 NVME_STATUS_INVALID_FIRMWARE_IMAGE;
    static const U8 NVME_STATUS_INVALID_FIRMWARE_SLOT;
    static const U8 NVME_STATUS_FIRMWARE_ACTIVATION_REQUIRES_CONVENTIONAL_RESET;
    static const U8 NVME_STATUS_FIRMWARE_ACTIVATION_REQUIRES_NVM_SUBSYSTEM_RESET;
    static const U8 NVME_STATUS_FIRMWARE_ACTIVATION_REQUIRES_RESET;
    static const U8 NVME_STATUS_FIRMWARE_ACTIVATION_REQUIRES_MAX_TIME_VIOLATION;
    static const U8 NVME_STATUS_FIRMWARE_ACTIVATION_PROHIBITED;
    static const U8 NVME_STATUS_OVERLAPPING_RANGE;
    static const U8 NVME_SCSI_OPERATION_CODE_READ;
    static const U8 NVME_SCSI_OPERATION_CODE_WRITE;

public:    
    static int getNsid(const std::string& devicePath, const DeviceHandle &handle);    
    static SKReturnCode issueNvmeCommand(const DeviceHandle &handle, const SKNvmeCommandDesc *cmdDesc, SKAlignedBuffer *buffer, U16 &lastError);
    static bool issueScsiCommand(const DeviceHandle &handle, const SKNvmeCommandDesc *cmdDesc, SKAlignedBuffer *dataBuffer, const U32 &timeoutInSeconds);
public:
    SKNvmeProtocol(const std::string &devicePath, const DeviceHandle &handle);
    virtual ~SKNvmeProtocol() override;

    SKReturnCode issueCommand(const SKNvmeCommandDesc *cmdDesc, SKAlignedBuffer *buffer);

private:
    static SKReturnCode convertNvmeStatus(const S32 &status);
    static U8 buildCdb(const SKNvmeCommandDesc *cmdDesc, U8 *cdb);

};

#endif
