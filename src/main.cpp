#include <iostream>
#include <signal.h>
#include <StorageKitSatDevice.h>
#include <StorageKitStorageDeviceUtils.h>

int main(int argc, char** argv) {
    SKStorageDevice* dev = SKStorageDeviceUtils::scanDevice("/dev/sdb");
    SKSatDevice* device = dynamic_cast<SKSatDevice*>(dev);

    SKAlignedBuffer *buffer = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    bool success = (SKReturnCode::SKSucceeded == device->ataIdentifyDevice(buffer));

    std::cout << "Success: " << success << std::endl;
    std::cout << "Buffer Data:" << std::endl;
    for (int i = 0; i < SECTOR_SIZE_IN_BYTES; i++)
        std::cout << (char) buffer->ToDataBuffer()[i];
    std::cout << std::endl;

    std::cout << "total size in bytes: " << device->queryDeviceSpaceInfo()->totalSizeInBytes << std::endl;
}