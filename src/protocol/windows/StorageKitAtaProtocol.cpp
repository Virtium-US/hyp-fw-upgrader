#include <fileapi.h>
#include <ntddscsi.h>

#include "StorageKitAtaProtocol.h"
#include "StorageKitScsiProtocol.h"

using namespace std;

#define FEATURE_REGISTER  0
#define ERROR_REGISTER    0
#define COUNT_REGISTER    1
#define LBA_LOW_REGISTER  2
#define LBA_MID_REGISTER  3
#define LBA_HIGH_REGISTER 4
#define DEVICE_REGISTER   5
#define COMMAND_REGISTER  6
#define STATUS_REGISTER   6
#define RESERVED_REGISTER 7

const SKReturnCode ATA_ERROR_MAP[8] = {
    SKErrorRegisterCommandCompletionTimeOut,         // Error bit 0
    SKErrorRegisterEndOfMedia,          // Error bit 1
    SKErrorRegisterAbort,         // Error bit 2
    SKErrorUnknown,      // Error bit 3
    SKErrorRegisterIDNotFound,         // Error bit 4
    SKErrorUnknown,      // Error bit 5
    SKErrorRegisterUncorrectable,          // Error bit 6
    SKErrorRegisterInterfaceCRC          // Error bit 7
};

SKAtaProtocol::SKAtaProtocol(const std::string &devicePath, const DeviceHandle &handle) :
    SKStorageProtocol(devicePath, handle)
{
}

SKAtaProtocol::~SKAtaProtocol()
{
}

SKReturnCode SKAtaProtocol::issueCommand(const SKAtaCommandDesc *cmdDesc, SKAlignedBuffer *buffer, const U32 &timeoutInSeconds)
{
    if (cmdDesc->inputFields.Command == SKAtaCommandDesc::COMMAND_SANITIZE
            || cmdDesc->inputFields.Command == SKAtaCommandDesc::COMMAND_SECURITY_SET_PASSWORD
            || cmdDesc->inputFields.Command == SKAtaCommandDesc::COMMAND_SECURITY_DISABLE_PASSWORD)
    {
        U16 lastError;
        SKAtaOutputTaskFileRegister lastOutputTFR;

        // windows 10 does not support security commands on ATA protocol.
        // Workaround by sending this command through SAT protocol with extended command (SAT-cdb16).
        SKAtaCommandDesc cmdDescExt(*cmdDesc);
        cmdDescExt.isExtCommand = true;

        SKReturnCode result = SKScsiProtocol::issueAtaCommand(getDeviceHandle(), &cmdDescExt, buffer, lastError, lastOutputTFR);
        this->lastError = lastError;
        this->lastOutputTaskFileRegister = lastOutputTFR;
        return result;
    }

    ATA_PASS_THROUGH_DIRECT ataCmd;

    ataCmd.AtaFlags = 0;
    if (READ_FROM_DEVICE == cmdDesc->dataAccess)
    {
        ataCmd.AtaFlags |= (1 << 1);
    }
    else if (WRITE_TO_DEVICE == cmdDesc->dataAccess)
    {
        ataCmd.AtaFlags |= (1 << 2);
    }
    if (cmdDesc->isExtCommand)
    {
        ataCmd.AtaFlags |= (1 << 3);
    }

    if (nullptr != buffer)
    {
        ataCmd.DataBuffer = buffer->ToDataBuffer();
    }
    ataCmd.Length = sizeof(ATA_PASS_THROUGH_DIRECT);
    ataCmd.DataTransferLength = cmdDesc->dataTransferLengthInSectors * 512;
    ataCmd.TimeOutValue = timeoutInSeconds;

    if (false == cmdDesc->isExtCommand)
    {
        ataCmd.CurrentTaskFile[FEATURE_REGISTER]  = cmdDesc->inputFields.Feature;
        ataCmd.CurrentTaskFile[COUNT_REGISTER]    = cmdDesc->inputFields.Count;
        ataCmd.CurrentTaskFile[LBA_LOW_REGISTER]  = cmdDesc->inputFields.LbaLow;
        ataCmd.CurrentTaskFile[LBA_MID_REGISTER]  = cmdDesc->inputFields.LbaMid;
        ataCmd.CurrentTaskFile[LBA_HIGH_REGISTER] = cmdDesc->inputFields.LbaHigh;
        ataCmd.CurrentTaskFile[DEVICE_REGISTER]   = cmdDesc->inputFields.Device;
        ataCmd.CurrentTaskFile[COMMAND_REGISTER]  = cmdDesc->inputFields.Command;
        ataCmd.CurrentTaskFile[RESERVED_REGISTER] = cmdDesc->inputFields.Reserved;
    }
    else
    {
        ataCmd.CurrentTaskFile[FEATURE_REGISTER]   = cmdDesc->inputFields.Feature;
        ataCmd.CurrentTaskFile[COUNT_REGISTER]     = cmdDesc->inputFields.Count;
        ataCmd.CurrentTaskFile[LBA_LOW_REGISTER]   = cmdDesc->inputFields.LbaLow;
        ataCmd.CurrentTaskFile[LBA_MID_REGISTER]   = cmdDesc->inputFields.LbaMid;
        ataCmd.CurrentTaskFile[LBA_HIGH_REGISTER]  = cmdDesc->inputFields.LbaHigh;
        ataCmd.CurrentTaskFile[DEVICE_REGISTER]    = cmdDesc->inputFields.Device;
        ataCmd.CurrentTaskFile[COMMAND_REGISTER]   = cmdDesc->inputFields.Command;
        ataCmd.CurrentTaskFile[RESERVED_REGISTER]  = cmdDesc->inputFields.Reserved;

        ataCmd.PreviousTaskFile[FEATURE_REGISTER]  = cmdDesc->inputFieldsExt.Feature;
        ataCmd.PreviousTaskFile[COUNT_REGISTER]    = cmdDesc->inputFieldsExt.Count;
        ataCmd.PreviousTaskFile[LBA_LOW_REGISTER]  = cmdDesc->inputFieldsExt.LbaLow;
        ataCmd.PreviousTaskFile[LBA_MID_REGISTER]  = cmdDesc->inputFieldsExt.LbaMid;
        ataCmd.PreviousTaskFile[LBA_HIGH_REGISTER] = cmdDesc->inputFieldsExt.LbaHigh;
        ataCmd.PreviousTaskFile[DEVICE_REGISTER]   = cmdDesc->inputFieldsExt.Device;
        ataCmd.PreviousTaskFile[COMMAND_REGISTER]  = cmdDesc->inputFieldsExt.Command;
        ataCmd.PreviousTaskFile[RESERVED_REGISTER] = cmdDesc->inputFieldsExt.Reserved;
    }

    unsigned long returnBytes;
    bool success = DeviceIoControl(
                this->handle,
                IOCTL_ATA_PASS_THROUGH_DIRECT,
                &ataCmd,
                ataCmd.Length,
                &ataCmd,
                ataCmd.Length,
                &returnBytes,
                nullptr);

    this->lastError = ataCmd.CurrentTaskFile[ERROR_REGISTER];

    // update outputTaskFileRegister
    this->lastOutputTaskFileRegister.Error = ataCmd.CurrentTaskFile[ERROR_REGISTER];
    this->lastOutputTaskFileRegister.Count = (ataCmd.PreviousTaskFile[COUNT_REGISTER] << 8) + ataCmd.CurrentTaskFile[COUNT_REGISTER];
    this->lastOutputTaskFileRegister.LbaLow = (ataCmd.PreviousTaskFile[LBA_LOW_REGISTER] << 8) + ataCmd.CurrentTaskFile[LBA_LOW_REGISTER];
    this->lastOutputTaskFileRegister.LbaMid = (ataCmd.PreviousTaskFile[LBA_MID_REGISTER] << 8) + ataCmd.CurrentTaskFile[LBA_MID_REGISTER];
    this->lastOutputTaskFileRegister.LbaHigh = (ataCmd.PreviousTaskFile[LBA_HIGH_REGISTER] << 8) + ataCmd.CurrentTaskFile[LBA_HIGH_REGISTER];
    this->lastOutputTaskFileRegister.Device = ataCmd.CurrentTaskFile[DEVICE_REGISTER];
    this->lastOutputTaskFileRegister.Status = ataCmd.CurrentTaskFile[STATUS_REGISTER];
    this->lastOutputTaskFileRegister.Reserved = ataCmd.CurrentTaskFile[RESERVED_REGISTER];

    if (false == success)
    {
        return SKErrorIO;
    }

    if (ataCmd.CurrentTaskFile[STATUS_REGISTER] & 0x01)         // Error bit is set
    {
        for (U8 i = 0; i < 8; i++)
        {
            if (this->lastError == (1 << i))
            {
                return ATA_ERROR_MAP[i];
            }
        }
        return SKErrorUnknown;
    }

    return SKSucceeded;
}

SKAtaOutputTaskFileRegister SKAtaProtocol::getLastOutputTaskFileRegister() const
{
    return this->lastOutputTaskFileRegister;
}
