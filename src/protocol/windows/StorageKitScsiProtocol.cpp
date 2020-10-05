#include <fileapi.h>
#include <ntddscsi.h>
#include <string.h>
#include "StorageKitScsiProtocol.h"

using namespace std;

struct SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER
{
    SCSI_PASS_THROUGH_DIRECT sptd;
    ULONG filler;
    U8 sense[SENSE_BUFFER_LENGTH];
};

SKReturnCode SKScsiProtocol::issueAtaCommand(const DeviceHandle &handle, const SKAtaCommandDesc *cmdDesc,
                                     SKAlignedBuffer *buffer, U16 &lastError, SKAtaOutputTaskFileRegister &lastOutputTaskFileRegister,
                                     const U32 &timeoutInSeconds)
{
    SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER scsiCmd;
    memset((void*)&scsiCmd, 0, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER));

    if (nullptr != buffer)
        scsiCmd.sptd.DataBuffer = buffer->ToDataBuffer();

    scsiCmd.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    scsiCmd.sptd.ScsiStatus = 0;
    scsiCmd.sptd.PathId = 0;
    scsiCmd.sptd.TargetId = 1;
    scsiCmd.sptd.Lun = 0;
    scsiCmd.sptd.DataTransferLength = cmdDesc->dataTransferLengthInSectors * 512;
    scsiCmd.sptd.TimeOutValue = timeoutInSeconds;

    scsiCmd.sptd.SenseInfoLength = sizeof(scsiCmd.sense);
    scsiCmd.sptd.SenseInfoOffset = sizeof(scsiCmd.sptd) + sizeof(scsiCmd.filler);

    scsiCmd.sptd.CdbLength = buildCdb(cmdDesc, scsiCmd.sptd.Cdb);
    if (cmdDesc->dataAccess == READ_FROM_DEVICE)
        scsiCmd.sptd.DataIn = SCSI_IOCTL_DATA_IN;
    else if(cmdDesc->dataAccess == WRITE_TO_DEVICE)
        scsiCmd.sptd.DataIn = SCSI_IOCTL_DATA_OUT;
    else
        scsiCmd.sptd.DataIn = SCSI_IOCTL_DATA_UNSPECIFIED;

    unsigned long returnBytes;
    bool success = DeviceIoControl(
                handle,
                IOCTL_SCSI_PASS_THROUGH_DIRECT,
                &scsiCmd,
                sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER),
                &scsiCmd,
                sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER),
                &returnBytes,
                NULL);

    // update outputTaskFileRegister
    extractAtaOutput(scsiCmd.sense, lastOutputTaskFileRegister, lastError);

    if (false == success)
    {
        return SKErrorIO;
    }

    return convertAtaStatus(scsiCmd.sptd.ScsiStatus, scsiCmd.sense);
}

SKReturnCode SKScsiProtocol::issueScsiCommand(const DeviceHandle &handle, const SKScsiCommandDesc *cmdDesc,
                                     SKAlignedBuffer *buffer, U16 &lastError, SKScsiSense &lastSense, const U32 &timeoutInSeconds)
{
    SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER scsiCmd;
    memset((void*)&scsiCmd, 0, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER));

    if (nullptr != buffer)
        scsiCmd.sptd.DataBuffer = buffer->ToDataBuffer();

    scsiCmd.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    scsiCmd.sptd.ScsiStatus = 0;
    scsiCmd.sptd.PathId = 0;
    scsiCmd.sptd.TargetId = 1;
    scsiCmd.sptd.Lun = 0;
    scsiCmd.sptd.DataTransferLength = cmdDesc->dataTransferLengthInSectors * SECTOR_SIZE_IN_BYTES;
    scsiCmd.sptd.TimeOutValue = timeoutInSeconds;

    scsiCmd.sptd.SenseInfoLength = sizeof(scsiCmd.sense);
    scsiCmd.sptd.SenseInfoOffset = sizeof(scsiCmd.sptd) + sizeof(scsiCmd.filler);

    // build cdb
    memcpy(scsiCmd.sptd.Cdb, cmdDesc->inputFields.Cdb, CDB_MAX_LENGTH);
    scsiCmd.sptd.CdbLength = cmdDesc->cdbLength;

    if (cmdDesc->dataAccess == READ_FROM_DEVICE)
        scsiCmd.sptd.DataIn = SCSI_IOCTL_DATA_IN;
    else if(cmdDesc->dataAccess == WRITE_TO_DEVICE)
        scsiCmd.sptd.DataIn = SCSI_IOCTL_DATA_OUT;
    else
        scsiCmd.sptd.DataIn = SCSI_IOCTL_DATA_UNSPECIFIED;

    unsigned long returnBytes;
    bool success = DeviceIoControl(
                handle,
                IOCTL_SCSI_PASS_THROUGH_DIRECT,
                &scsiCmd,
                sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER),
                &scsiCmd,
                sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER),
                &returnBytes,
                NULL);

    lastError = scsiCmd.sptd.ScsiStatus;

    // update outSense
    extractScsiOutput(scsiCmd.sense, lastSense);

    if (false == success)
    {
        return SKErrorIO;
    }

    // TODO: check condition if needed
    return convertScsiStatus(scsiCmd.sptd.ScsiStatus, scsiCmd.sense);
}

SKReturnCode SKScsiProtocol::issueAtaCommand(const SKAtaCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    return issueAtaCommand(this->handle, cmdDesc, buffer, this->lastError, this->lastOutputTaskFileRegister);
}

SKReturnCode SKScsiProtocol::issueScsiCommand(const SKScsiCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    return issueScsiCommand(this->handle, cmdDesc, buffer, this->lastError, this->lastSense);
}
