#include <fcntl.h>
#include <scsi/sg.h>
#include <sys/ioctl.h>
#include <string.h>

#include "StorageKitScsiProtocol.h"

using namespace std;

#define RESERVED_REGISTER       10
#define ERROR_REGISTER          11
#define COUNT_EXT_REGISTER      12
#define COUNT_REGISTER          13
#define LBA_LOW_EXT_REGISTER    14
#define LBA_LOW_REGISTER        15
#define LBA_MID_EXT_REGISTER    16
#define LBA_MID_REGISTER        17
#define LBA_HIGH_EXT_REGISTER   18
#define LBA_HIGH_REGISTER       19
#define DEVICE_REGISTER         20
#define STATUS_REGISTER         21

SKReturnCode SKScsiProtocol::issueAtaCommand(const DeviceHandle &handle, const SKAtaCommandDesc *cmdDesc,
                                     SKAlignedBuffer *buffer, U16 &lastError, SKAtaOutputTaskFileRegister &lastOutputTaskFileRegister,
                                     const U32 &timeoutInSeconds)
{
    U8 sat[16]; // sat 16
    U8 sense[SENSE_BUFFER_LENGTH];

    buildCdb(cmdDesc, sat);

    sg_io_hdr hdr;

    // reset hdr
    memset((void*)&hdr, 0x00, sizeof(hdr));

    hdr.interface_id = 'S';

    hdr.cmdp = sat;
    hdr.cmd_len = sizeof(sat);

    hdr.sbp = sense;
    hdr.mx_sb_len = sizeof(sense);

    hdr.timeout = timeoutInSeconds*1000;     // seconds to milliseconds
    hdr.dxfer_len = cmdDesc->dataTransferLengthInSectors * SECTOR_SIZE_IN_BYTES;
    if (nullptr != buffer)
    {
        hdr.dxferp = buffer->ToDataBuffer();
    }

    hdr.dxfer_direction = SG_DXFER_NONE;
    if (READ_FROM_DEVICE == cmdDesc->dataAccess)
    {
        hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    }
    else if (WRITE_TO_DEVICE == cmdDesc->dataAccess)
    {
        hdr.dxfer_direction = SG_DXFER_TO_DEV;
    }

    int retCode = ioctl(handle, SG_IO, &hdr);

    // update outputTaskFileRegister
    extractAtaOutput(sense, lastOutputTaskFileRegister, lastError);

    if (0 != retCode)
    {
        return SKErrorIO;
    }

    return convertAtaStatus(hdr.status, sense);
}

SKReturnCode SKScsiProtocol::issueScsiCommand(const DeviceHandle &handle, const SKScsiCommandDesc *cmdDesc,
                                     SKAlignedBuffer *buffer, U16 &lastError, SKScsiSense &lastSense, const U32 &timeoutInSeconds)
{
    U8 sat[16]; // sat 16
    U8 sense[SENSE_BUFFER_LENGTH];

    // build cdb
    memcpy(sat, cmdDesc->inputFields.Cdb, 16);

    sg_io_hdr hdr;

    // reset hdr
    memset((void*)&hdr, 0x00, sizeof(hdr));

    hdr.interface_id = 'S';

    hdr.cmdp = sat;
    hdr.cmd_len = sizeof(sat);

    hdr.sbp = sense;
    hdr.mx_sb_len = sizeof(sense);

    hdr.timeout = timeoutInSeconds*1000;     // seconds to milliseconds
    hdr.dxfer_len = cmdDesc->dataTransferLengthInSectors * SECTOR_SIZE_IN_BYTES;
    if (nullptr != buffer)
    {
        hdr.dxferp = buffer->ToDataBuffer();
    }

    hdr.dxfer_direction = SG_DXFER_NONE;
    if (READ_FROM_DEVICE == cmdDesc->dataAccess)
    {
        hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    }
    else if (WRITE_TO_DEVICE == cmdDesc->dataAccess)
    {
        hdr.dxfer_direction = SG_DXFER_TO_DEV;
    }

    int retCode = ioctl(handle, SG_IO, &hdr);

    // update lastSense
    extractScsiOutput(sense, lastSense);

    if (0 != retCode)
    {
        return SKErrorIO;
    }

    return convertScsiStatus(hdr.status, sense);
}

SKReturnCode SKScsiProtocol::issueAtaCommand(const SKAtaCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    return issueAtaCommand(this->handle, cmdDesc, buffer, this->lastError, this->lastOutputTaskFileRegister);
}

SKReturnCode SKScsiProtocol::issueScsiCommand(const SKScsiCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    return issueScsiCommand(this->handle, cmdDesc, buffer, this->lastError, this->lastSense);
}
