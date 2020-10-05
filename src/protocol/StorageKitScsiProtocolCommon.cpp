#include <string.h>
#include "StorageKitScsiProtocol.h"

using namespace std;

#define SENSE_TYPE_REGISTER 0
#define SENSE_KEY_REGISTER 2

#define DESCRIPTOR_CODE_REGISTER                8
#define ADDITIONAL_DESCRIPTOR_LENGTH_REGISTER   9

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

const SKReturnCode ATA_ERROR_MAP[8] = {
    SKErrorRegisterCommandCompletionTimeOut,    // Error bit 0
    SKErrorRegisterEndOfMedia,                  // Error bit 1
    SKErrorRegisterAbort,                       // Error bit 2
    SKErrorUnknown,                             // Error bit 3
    SKErrorRegisterIDNotFound,                  // Error bit 4
    SKErrorUnknown,                             // Error bit 5
    SKErrorRegisterUncorrectable,               // Error bit 6
    SKErrorRegisterInterfaceCRC                 // Error bit 7
};

const SKReturnCode SENSE_KEY_ERROR_MAP[16] = {
    SKErrorSenseNoSense,
    SKErrorSenseRecoveredError,
    SKErrorSenseNotReady,
    SKErrorSenseMediumError,
    SKErrorSenseHardwareError,
    SKErrorSenseIllegalRequest,
    SKErrorSenseUnitAttention,
    SKErrorSenseDataProtect,
    SKErrorSenseBlankCheck,
    SKErrorSenseVendorSpecific,
    SKErrorSenseCopyAborted,
    SKErrorSenseAbortedCommand,
    SKErrorSenseVolumeOverflow,
    SKErrorUnknown,
    SKErrorSenseMiscompare,
    SKErrorSenseCompleted
};

const U8 SKScsiProtocol::PROTOCOL_NO_DATA = 0x06;
const U8 SKScsiProtocol::PROTOCOL_PIO_IN = 0x08;
const U8 SKScsiProtocol::PROTOCOL_PIO_OUT = 0x0A;
const U8 SKScsiProtocol::PROTOCOL_DMA = 0x0C;

const U8 SKScsiProtocol::TLENGTH_IN_SECTOR = 0x02;

const U8 SKScsiProtocol::TRANSFER_BLOCK = (1 << 2);

const U8 SKScsiProtocol::WRITE_TO_DEV = (0 << 3);
const U8 SKScsiProtocol::READ_FROM_DEV = (1 << 3);
const U8 SKScsiProtocol::CHECK_CONDITION = (1 << 5);

const U8 SKScsiProtocol::SCSI_STATUS_GOOD = 0;
const U8 SKScsiProtocol::SCSI_STATUS_CHECK_CONDITION = 0x02;
const U8 SKScsiProtocol::SCSI_STATUS_CONDITION_MET = 0x04;
const U8 SKScsiProtocol::SCSI_STATUS_BUSY = 0x08;
const U8 SKScsiProtocol::SCSI_STATUS_TASK_SET_FULL = 0x28;
const U8 SKScsiProtocol::SCSI_STATUS_ACA_ACTIVE = 0x30;
const U8 SKScsiProtocol::SCSI_STATUS_TASK_ABORTED = 0x40;

SKScsiProtocol::SKScsiProtocol(const std::string &devicePath, const DeviceHandle &handle) :
    SKStorageProtocol(devicePath, handle)
{
}

SKScsiProtocol::~SKScsiProtocol()
{
}

SKAtaOutputTaskFileRegister SKScsiProtocol::getLastOutputTaskFileRegister() const
{
    return this->lastOutputTaskFileRegister;
}

SKScsiSense SKScsiProtocol::getLastSense() const
{
    return this->lastSense;
}

U8 SKScsiProtocol::buildCdb(const SKAtaCommandDesc *cmdDesc, U8 *cdb)
{
    memset(cdb, 0, 16);

    U8 transferMode = 0;
    if (NONE == cmdDesc->dataAccess)
        transferMode = PROTOCOL_NO_DATA;
    else
    {
        if (DMA == cmdDesc->transferMode)
            transferMode = PROTOCOL_DMA;
        else
            transferMode = READ_FROM_DEVICE == cmdDesc->dataAccess ? PROTOCOL_PIO_IN : PROTOCOL_PIO_OUT;
    }
    cdb[1] = transferMode; // MULTIPLE_COUNT | pROTOCOL | EXTEND

    // check condition
    cdb[2] = CHECK_CONDITION;
    if (NONE != cmdDesc->dataAccess)
    {
        cdb[2] |= TRANSFER_BLOCK;
        cdb[2] |= TLENGTH_IN_SECTOR;
        cdb[2] |= READ_FROM_DEVICE == cmdDesc->dataAccess ? READ_FROM_DEV : WRITE_TO_DEV;
    }

    U8 cdbLength = 0;
    if (true == cmdDesc->isExtCommand)
    {
        cdbLength = 16;
        cdb[0]    = 0x85;
        cdb[1]    |= 1;     // The extend bit
        cdb[3]    = cmdDesc->inputFieldsExt.Feature;
        cdb[4]    = cmdDesc->inputFields.Feature;
        cdb[5]    = cmdDesc->inputFieldsExt.Count;
        cdb[6]    = cmdDesc->inputFields.Count;
        cdb[8]    = cmdDesc->inputFields.LbaLow;
        cdb[10]   = cmdDesc->inputFields.LbaMid;
        cdb[12]   = cmdDesc->inputFields.LbaHigh;
        cdb[7]    = cmdDesc->inputFieldsExt.LbaLow;
        cdb[9]    = cmdDesc->inputFieldsExt.LbaMid;
        cdb[11]   = cmdDesc->inputFieldsExt.LbaHigh;
        cdb[13]   = cmdDesc->inputFields.Device;
        cdb[14]   = cmdDesc->inputFields.Command;
    }
    else
    {
        cdbLength = 12;
        cdb[0]    = 0xA1;
        cdb[3]    = cmdDesc->inputFields.Feature;
        cdb[4]    = cmdDesc->inputFields.Count;
        cdb[5]    = cmdDesc->inputFields.LbaLow;
        cdb[7]    = cmdDesc->inputFields.LbaHigh;
        cdb[6]    = cmdDesc->inputFields.LbaMid;
        cdb[8]    = cmdDesc->inputFields.Device;
        cdb[9]    = cmdDesc->inputFields.Command;
    }

    return cdbLength;
}

SKReturnCode SKScsiProtocol::convertAtaStatus(const U8 &scsiStatus, const U8 *sense)
{
    U8 senseType = (sense[SENSE_TYPE_REGISTER] & 0x7F);
    if (0x72 == senseType)
    {
        if(0x09 == sense[DESCRIPTOR_CODE_REGISTER]
            && 0x0C == sense[ADDITIONAL_DESCRIPTOR_LENGTH_REGISTER])
        {
            if (0x01 & sense[STATUS_REGISTER])         // Error bit is set
            {
                U8 error = sense[ERROR_REGISTER];
                for (U8 i = 0; i < 8; i++)
                {
                    if (error == (1 << i))
                        return ATA_ERROR_MAP[i];
                }
                return SKErrorUnknown;
            }
            else
            {
                return SKSucceeded;
            }
        }
    }

    return convertScsiStatus(scsiStatus, sense);
}

SKReturnCode SKScsiProtocol::convertScsiStatus(const U8 &status, const U8 *sense)
{
    switch(status)
    {
        case SCSI_STATUS_GOOD:
        case SCSI_STATUS_CONDITION_MET:
            return SKSucceeded;
        case SCSI_STATUS_CHECK_CONDITION:
            return convertSenseKey(sense[SENSE_TYPE_REGISTER], sense[SENSE_KEY_REGISTER]);
        case SCSI_STATUS_BUSY:
            return SKErrorScsiDeviceBusy;
        case SCSI_STATUS_TASK_SET_FULL:
            return SKErrorScsiTaskSetFull;
        case SCSI_STATUS_ACA_ACTIVE:
            return SKErrorScsiACAActive;
        case SCSI_STATUS_TASK_ABORTED:
            return SKErrorScsiTaskAborted;
        default:
            break;
    }

    return SKErrorUnknown;
}

SKReturnCode SKScsiProtocol::convertSenseKey(const U8 &senseType, const U8 &senseKey)
{
    if (0x70 == senseType)
    {
        if (sizeof(SENSE_KEY_ERROR_MAP) > senseKey)
        {
            SKReturnCode error = SENSE_KEY_ERROR_MAP[senseKey];
            if (SKReturnCode::SKErrorSenseRecoveredError == error)
            {
                return SKReturnCode::SKSucceeded;
            }
            else
            {
                return error;
            }
        }
    }

    return SKErrorScsiCheckCondition;
}

void SKScsiProtocol::extractAtaOutput(const U8 *sense, SKAtaOutputTaskFileRegister &output, U16 &lastError)
{
    lastError = sense[ERROR_REGISTER];

    output.Error = sense[ERROR_REGISTER];
    output.Count = (sense[COUNT_EXT_REGISTER] << 8) + sense[COUNT_REGISTER];
    output.LbaLow = (sense[LBA_LOW_EXT_REGISTER] << 8) + sense[LBA_LOW_REGISTER];
    output.LbaMid = (sense[LBA_MID_EXT_REGISTER] << 8) + sense[LBA_MID_REGISTER];
    output.LbaHigh = (sense[LBA_HIGH_EXT_REGISTER] << 8) + sense[LBA_HIGH_REGISTER];
    output.Device = sense[DEVICE_REGISTER];
    output.Status = sense[STATUS_REGISTER];
    output.Reserved = sense[RESERVED_REGISTER];
}

void SKScsiProtocol::extractScsiOutput(const U8 *sense, SKScsiSense &output)
{
    memcpy(output.Sense, sense, SENSE_BUFFER_LENGTH);
}
