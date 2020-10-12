#include <iostream>
#include <signal.h>
#include <string.h>
#include <vector>
#include <StorageKitStorageProtocol.h>
#include <StorageKitScsiProtocol.h>
#include <StorageKitScsiCommandDesc.h>
#include <StorageKitU9VcCommandDesc.h>

int main(int argc, char** argv) {

    // TODO let user specific device path in the command line args
    char* userDevicePath = "/dev/sdb";

    SKBaseDeviceInfo* deviceInfo = SKStorageProtocol::scan(userDevicePath);
    SKScsiProtocol* scsiInterface = new SKScsiProtocol(deviceInfo->devicePath, deviceInfo->deviceHandle);

    SKAlignedBuffer *buffer = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    std::cout << "scsi command status: " << scsiInterface->issueScsiCommand(SKU9VcCommandDesc::createFirmwareUpdateExecute(), buffer) << std::endl;

    std::cout << "Data in buffer:" << std::endl;
    printf("buffer size in bytes: %d\n", buffer->GetSizeInByte());
    for (int i = 0; i < SECTOR_SIZE_IN_BYTES; i++) {
        printf("%x", buffer->ToDataBuffer()[i]);
    }
    std::cout << std::endl;
}