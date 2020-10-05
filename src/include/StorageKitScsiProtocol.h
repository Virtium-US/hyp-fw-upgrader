#ifndef SatProtocol_h
#define SatProtocol_h

#include "StorageKitAlignedBuffer.h"
#include "StorageKitAtaCommandDesc.h"
#include "StorageKitScsiCommandDesc.h"
#include "StorageKitStorageProtocol.h"

class SKScsiProtocol : public SKStorageProtocol
{
  private:
    static const U8 PROTOCOL_NO_DATA;
    static const U8 PROTOCOL_PIO_IN;
    static const U8 PROTOCOL_PIO_OUT;
    static const U8 PROTOCOL_DMA;

    static const U8 TLENGTH_IN_SECTOR;

    static const U8 TRANSFER_BLOCK;

    static const U8 WRITE_TO_DEV;
    static const U8 READ_FROM_DEV;
    static const U8 CHECK_CONDITION;

    static const U8 SCSI_STATUS_GOOD;
    static const U8 SCSI_STATUS_CHECK_CONDITION;
    static const U8 SCSI_STATUS_CONDITION_MET;
    static const U8 SCSI_STATUS_BUSY;
    static const U8 SCSI_STATUS_TASK_SET_FULL;
    static const U8 SCSI_STATUS_ACA_ACTIVE;
    static const U8 SCSI_STATUS_TASK_ABORTED;

  private:
    SKAtaOutputTaskFileRegister lastOutputTaskFileRegister;
    SKScsiSense lastSense;

  public:
    static SKReturnCode issueAtaCommand(const DeviceHandle &handle, const SKAtaCommandDesc *cmdDesc,
                                     SKAlignedBuffer *buffer, U16 &lastError, SKAtaOutputTaskFileRegister &lastOutputTaskFileRegister,
                                     const U32 &timeoutInSeconds = 10);

    static SKReturnCode issueScsiCommand(const DeviceHandle &handle, const SKScsiCommandDesc *cmdDesc,
                                     SKAlignedBuffer *buffer, U16 &lastError, SKScsiSense &lastSense, const U32 &timeoutInSeconds = 10);

  private:
    static U8 buildCdb(const SKAtaCommandDesc *cmdDesc, U8 *cdb);

    static SKReturnCode convertAtaStatus(const U8 &scsiStatus, const U8 *sense);
    static SKReturnCode convertScsiStatus(const U8 &status, const U8 *sense);
    static SKReturnCode convertSenseKey(const U8 &senseType, const U8 &senseKey);

    static void extractAtaOutput(const U8 *sense, SKAtaOutputTaskFileRegister &output, U16 &lastError);
    static void extractScsiOutput(const U8 *sense, SKScsiSense &output);

  public:
    SKScsiProtocol(const std::string &devicePath, const DeviceHandle &handle);
    virtual ~SKScsiProtocol() override;

    SKReturnCode issueAtaCommand(const SKAtaCommandDesc *cmdDesc, SKAlignedBuffer *buffer);
    SKReturnCode issueScsiCommand(const SKScsiCommandDesc *cmdDesc, SKAlignedBuffer *buffer);
    SKAtaOutputTaskFileRegister getLastOutputTaskFileRegister() const;
    SKScsiSense getLastSense() const;
};

#endif
