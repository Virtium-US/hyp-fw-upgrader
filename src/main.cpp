#include <iostream>
#include <signal.h>
#include <string.h>
#include <vector>
#include <StorageKitStorageProtocol.h>
#include <StorageKitScsiProtocol.h>
#include <StorageKitScsiCommandDesc.h>

int main(int argc, char** argv) {

    // TODO let user specific device path in the command line args
    char* userDevicePath = "/dev/sdb";

    SKBaseDeviceInfo* deviceInfo = SKStorageProtocol::scan(userDevicePath);
    SKScsiProtocol* scsiInterface = new SKScsiProtocol(deviceInfo->devicePath, deviceInfo->deviceHandle);

    SKAlignedBuffer *buffer = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    scsiInterface->issueScsiCommand(SKScsiCommandDesc::createInquiryDesc(SKInquiryPageCode::SKDeviceIdentification), buffer);

    std::cout << "Buffer Data:" << std::endl;
    for (int i = 0; i < SECTOR_SIZE_IN_BYTES; i++) {
        std::cout << (char) buffer->ToDataBuffer()[i];
    }
    std::cout << std::endl;
}