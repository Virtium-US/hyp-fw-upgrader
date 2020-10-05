#include <string.h>

#include "StorageKitHypDevice.h"
#include "StorageKitAtaCommandDesc.h"
#include "StorageKitScsiCommandDesc.h"
#include "StorageKitU9VcCommandDesc.h"
#include "StorageKitScsiProtocol.h"

using namespace std;

SKHypDevice::SKHypDevice(const std::string &devicePath, const DeviceHandle &deviceHandle) :
    SKScsiDevice(devicePath, deviceHandle)
{
    this->deviceType = HYP;
}

SKReturnCode SKHypDevice::issueCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    switch (cmdDesc->getCommandType())
    {
        case ATA_COMMAND:
        {
            return this->handleAtaCommand(cmdDesc, buffer);
        }
        case SCSI_COMMAND:
        {
            return this->handleScsiCommand(cmdDesc, buffer);
        }
        default:
        {
            break;
        }
    }
    return SKErrorInvalidCommand;
}

SKReturnCode SKHypDevice::handleAtaCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    if (ATA_COMMAND != cmdDesc->getCommandType())
    {
        return SKErrorInvalidCommand;
    }

    const SKAtaCommandDesc *ataCmdDesc = (const SKAtaCommandDesc *)cmdDesc;
    if (ataCmdDesc->inputFields.Command != SKAtaCommandDesc::COMMAND_IDENTIFY_DEVICE
            && ataCmdDesc->inputFields.Command != SKAtaCommandDesc::COMMAND_SMART)
    {
        return SKErrorInvalidCommand;
    }
    return this->protocol->issueAtaCommand(ataCmdDesc, buffer);
}

SKReturnCode SKHypDevice::handleScsiCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    if (SCSI_COMMAND != cmdDesc->getCommandType())
    {
        return SKErrorInvalidCommand;
    }

    const SKScsiCommandDesc *scsiCmdDesc = (const SKScsiCommandDesc *)cmdDesc;
    return this->protocol->issueScsiCommand(scsiCmdDesc, buffer);
}

SKReturnCode SKHypDevice::ataIdentifyDevice(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKAtaCommandDesc::createIdentifyDeviceDesc();
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKHypDevice::ataReadSmart(SKAlignedBuffer *buffer)
{
    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKAtaCommandDesc::createReadSmartData();
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;

    return result;
}

SKReturnCode SKHypDevice::scsiTrim(U64 startLba, U64 count)
{
    // Fill buffer
    SKAlignedBuffer *buffer = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    memset(buffer->ToDataBuffer(), 0x00, SECTOR_SIZE_IN_BYTES);

    // 0..3 - Signature word, fixed value 0x87654321
    U8 *data = buffer->ToDataBuffer();
    data[0] = 0x87;
    data[1] = 0x65;
    data[2] = 0x43;
    data[3] = 0x21;

    // 4..7 - Bit 0: Secure Erase (if enabled, data within trim range is erased on flash)
    data[7] = 0x01;

    // 8..15 - Trim range start LBA
    data[8] = (startLba >> 56) & 0xFF;
    data[9] = (startLba >> 48) & 0xFF;
    data[10] = (startLba >> 40) & 0xFF;
    data[11] = (startLba >> 32) & 0xFF;
    data[12] = (startLba >> 24) & 0xFF;
    data[13] = (startLba >> 16) & 0xFF;
    data[14] = (startLba >> 8) & 0xFF;
    data[15] = startLba & 0xFF;

    // 16..23 - Trim range LBA count
    data[16] = (count >> 56) & 0xFF;
    data[17] = (count >> 48) & 0xFF;
    data[18] = (count >> 40) & 0xFF;
    data[19] = (count >> 32) & 0xFF;
    data[20] = (count >> 24) & 0xFF;
    data[21] = (count >> 16) & 0xFF;
    data[22] = (count >> 8) & 0xFF;
    data[23] = count & 0xFF;

    SKCommandDesc *cmdDesc = (SKCommandDesc *)SKU9VcCommandDesc::createTrimAddressRangeDesc();
    SKReturnCode result = this->issueCommand(cmdDesc, buffer);
    delete cmdDesc;
    delete buffer;

    return result;
}
