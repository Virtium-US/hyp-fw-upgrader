#include <iostream>
#include <stdio.h>
#include <string.h>

#include "FirmwareUpdater.h"

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
FirmwareVersionInfo FirmwareUpdater::readFirmwareVersion()
{
    // prepare command data
    SKAlignedBuffer* buffer = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    SKScsiCommandDesc* desc = SKU9VcCommandDesc::createReadFirmwareVersion();
    this->scsiInterface->issueScsiCommand(desc, buffer);

    // format returned data
    FirmwareVersionInfo info;
    memcpy(&info, buffer->ToDataBuffer(), sizeof(FirmwareVersionInfo));

    // clean up
    delete buffer;
    delete desc;

    return info;
}
