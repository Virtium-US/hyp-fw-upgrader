#include "StorageKitStorageDeviceUtils.h"
#include "StorageKitAtaProtocol.h"
#include "StorageKitScsiProtocol.h"
#include "StorageKitNvmeProtocol.h"
#include "StorageKitAtaDevice.h"
#include "StorageKitSatDevice.h"
#include "StorageKitHypDevice.h"
#include "StorageKitScsiDevice.h"
#include "StorageKitNvmeDevice.h"

using namespace std;

SKStorageDevice* createStorageDevice(SKBaseDeviceInfo *scannedDevice)
{
    if (nullptr == scannedDevice)
    {
        return nullptr;
    }

    SKStorageDevice *storageDevice = nullptr;
    switch(scannedDevice->type)
    {
        case ATA:
        {
            storageDevice = new SKAtaDevice(scannedDevice->devicePath, scannedDevice->deviceHandle);
            break;
        }

        case SAT:
        {
            storageDevice = new SKSatDevice(scannedDevice->devicePath, scannedDevice->deviceHandle);
            break;
        }

        case HYP:
        {
            storageDevice = new SKHypDevice(scannedDevice->devicePath, scannedDevice->deviceHandle);
            break;
        }

        case SCSI:
        {
            storageDevice = new SKScsiDevice(scannedDevice->devicePath, scannedDevice->deviceHandle);
            break;
        }

        case NVME:
        {
            int nsid = SKNvmeProtocol::getNsid(scannedDevice->devicePath, scannedDevice->deviceHandle);            
            // create NVME device instance if get nsid succeeded
            if (0 < nsid)
            {
                storageDevice = new SKNvmeDevice(scannedDevice->devicePath, scannedDevice->deviceHandle, nsid);
            }

            break;
        }

        default:
        {
            storageDevice = nullptr;
            break;
        }
    }

    return storageDevice;
}

SKStorageDevice* SKStorageDeviceUtils::scanDevice(const std::string &devicePath)
{
    SKBaseDeviceInfo *deviceInfo = SKStorageProtocol::scan(devicePath);
    SKStorageDevice *storageDevice = createStorageDevice(deviceInfo);
    if (deviceInfo)
        delete deviceInfo;

    return storageDevice;
}

void SKStorageDeviceUtils::scanDevices(vector<SKStorageDevice *> &storageDevices)
{
    vector<SKBaseDeviceInfo*> deviceInfos;
    SKStorageDevice *storageDevice;

    SKStorageProtocol::scan(deviceInfos);
    for (U32 i = 0; i < deviceInfos.size(); ++i)
    {
        storageDevice = createStorageDevice(deviceInfos[i]);
        if (deviceInfos[i])
        {
            delete deviceInfos[i];
        }
        if (nullptr != storageDevice)
        {
            storageDevices.push_back(storageDevice);
        }
    }
}
