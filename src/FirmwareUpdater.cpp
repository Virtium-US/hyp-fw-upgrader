#include <iostream>
#include <stdio.h>
#include <string.h>

#include "FirmwareUpdater.h"

using namespace updater;

FirmwareUpdater::FirmwareUpdater(char* devPath) 
{
    SKBaseDeviceInfo* devInfo = SKStorageProtocol::scan(devPath);
    this->scsiInterface = new SKScsiProtocol(devInfo->devicePath, devInfo->deviceHandle);
    
    delete devInfo;
}

FirmwareUpdater::~FirmwareUpdater()
{
    delete scsiInterface;
}

// Issues the VC to read firmware version information
FWVersionInfo_t FirmwareUpdater::readFirmwareVersion()
{
    // prepare and execute the command data
    SKAlignedBuffer* buffer = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    SKScsiCommandDesc* desc = SKU9VcCommandDesc::createReadFirmwareVersion();
    this->scsiInterface->issueScsiCommand(desc, buffer);

    // format returned data
    FWVersionInfo_t info;
    memcpy(&info, buffer->ToDataBuffer(), sizeof(FWVersionInfo_t));

    // clean up
    delete buffer;
    delete desc;

    return info;
}

// Issues the VC to inspect information about the target device
TargetInfo_t FirmwareUpdater::readTargetInfo()
{
    // prepare and execute the command
    SKAlignedBuffer* buffer = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    SKScsiCommandDesc* desc = SKU9VcCommandDesc::createTargetInfo();
    this->scsiInterface->issueScsiCommand(desc, buffer);

    // format returned data
    TargetInfo_t targetInfo;
    memcpy(&targetInfo, buffer->ToDataBuffer(), sizeof(TargetInfo_t));

    delete buffer;
    delete desc;

    return targetInfo;
}
