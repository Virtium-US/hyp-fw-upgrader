#include "StorageKitStorageProtocol.h"

#include <windows.h>
#include <string.h>

#include "StorageKitAtaCommandDesc.h"
#include "StorageKitScsiProtocol.h"

using namespace std;

#define EFI_SYSTEM_PARTITION_NAME L"EFI system partition"
#define BASIC_DATA_PARTITION_NAME L"Basic data partition"

#define EFI_SYSTEM_PARTITION_LENGTH wcslen(EFI_SYSTEM_PARTITION_NAME)
#define BASIC_DATA_PARTITION_LENGTH wcslen(BASIC_DATA_PARTITION_NAME)

#define COMMAND_TIMEOUT_IN_SECONDS 2

union STORAGE_DEVICE_DESCRIPTOR_DATA
{
    STORAGE_DEVICE_DESCRIPTOR desc;
    U8 raw[256];
};

bool isSat(const DeviceHandle &handle)
{
    SKAtaCommandDesc *cmd;
    SKAtaOutputTaskFileRegister lastOutputTFR;
    U16 lastError;
    SKReturnCode returnCode;
    SKAlignedBuffer buffer(SECTOR_SIZE_IN_BYTES);

    // Try ata identify device command
    // If the device can handle ata command via SCSI protocol, it's considered as a Sat
    cmd = SKAtaCommandDesc::createIdentifyDeviceDesc();
    returnCode = SKScsiProtocol::issueAtaCommand(handle, cmd, &buffer, lastError, lastOutputTFR);
    delete cmd;

    return (SKSucceeded == returnCode);
}

bool isVirtiumUsbVendorId(const U8 *buffer)
{
    if ('V' == buffer[8] && 'i' == buffer[9] && 'r' == buffer[10] &&
        't' == buffer[11] && 'i' == buffer[12] && 'u' == buffer[13] &&
        'm' == buffer[14])
        return true;
    return false;
}

bool isDeviceSecurityLocked(const U8 *buffer)
{
     // determine device security locked: Word 128, bit 2
    U16 word128 = static_cast<U16>((buffer[(128 * 2) + 1] << 8) | buffer[128 * 2]);
    bool securityLocked = (word128 >> 2) & 0x1;
    return securityLocked;
}

SKDeviceType getUsbType(const DeviceHandle &handle)
{
    U16 lastError;
    SKScsiSense lastSense;

    // Try scsi inquiry device identification 0x83h command
    // If the device cannot handle scsi inquiry command
    //   Try scsi inquiry standard command
    //   If the device cannot handle scsi inquiry standard command, it's a unknown device
    //   Otherwise, it's a scsi device
    SKScsiCommandDesc *scsiCmd = SKScsiCommandDesc::createInquiryDesc(SKInquiryPageCode::SKDeviceIdentification);
    SKAlignedBuffer inquiryBuffer(SECTOR_SIZE_IN_BYTES);
    SKReturnCode inquiryReturnCode = SKScsiProtocol::issueScsiCommand(handle, scsiCmd, &inquiryBuffer, lastError, lastSense, COMMAND_TIMEOUT_IN_SECONDS);
    delete scsiCmd;

    if (SKReturnCode::SKSucceeded != inquiryReturnCode)
    {
        scsiCmd = SKScsiCommandDesc::createInquiryDesc(SKInquiryPageCode::SKStandard);
        inquiryReturnCode = SKScsiProtocol::issueScsiCommand(handle, scsiCmd, &inquiryBuffer, lastError, lastSense, COMMAND_TIMEOUT_IN_SECONDS);
        delete scsiCmd;
        if (SKReturnCode::SKSucceeded == inquiryReturnCode)
        {
            return SCSI;
        }

        return UNKNOWN;
    }

    // Try ata identify device command
    // If the device cannot handle ata identify command, it's a scsi device
    SKAtaOutputTaskFileRegister lastOutputTFR;
    SKAtaCommandDesc *ataCmd = SKAtaCommandDesc::createIdentifyDeviceDesc();
    SKAlignedBuffer identifyBuffer(SECTOR_SIZE_IN_BYTES);
    SKReturnCode identifyReturnCode = SKScsiProtocol::issueAtaCommand(handle, ataCmd, &identifyBuffer, lastError, lastOutputTFR, COMMAND_TIMEOUT_IN_SECONDS);
    delete ataCmd;

    if (SKReturnCode::SKSucceeded == identifyReturnCode)
    {
        // If the usb vendor is Virtium drive, it's a Hyperstone device
        // Otherwise, it's a usb_sat device
        if (true == isVirtiumUsbVendorId(inquiryBuffer.ToDataBuffer()))
        {
            return HYP;
        }
        else
        {
            return SAT;
        }
    }

    return SCSI;
}

SKStorageProtocol::SKStorageProtocol(const std::string &devicePath, const DeviceHandle &handle)
{
    this->devicePath = devicePath;
    this->handle = handle;
}

SKStorageProtocol::~SKStorageProtocol()
{
    CloseHandle(this->handle);
    this->handle = INVALID_HANDLE_VALUE;
}

U16 SKStorageProtocol::getLastError() const
{
    return this->lastError;
}

DeviceHandle SKStorageProtocol::getDeviceHandle() const
{
    return this->handle;
}

void SKStorageProtocol::scan(std::vector<SKBaseDeviceInfo*> &devices)
{
    SKBaseDeviceInfo* device;
    char buffer[128];
    string devicePath;

    for (U32 i = 0; i < 16; ++i)
    {
        sprintf(buffer, "\\\\.\\PhysicalDrive%d", i);
        devicePath = string(buffer);
        device = SKStorageProtocol::scan(devicePath);

        if (nullptr != device)
        {
            devices.push_back(device);
        }
    }
}

SKBaseDeviceInfo* SKStorageProtocol::scan(const std::string &devicePath)
{
    SKBaseDeviceInfo* device = nullptr;

    DeviceHandle handle = CreateFileA(
             devicePath.c_str(),
             GENERIC_READ | GENERIC_WRITE,
             FILE_SHARE_READ | FILE_SHARE_WRITE,
             NULL,
             OPEN_EXISTING,
             FILE_ATTRIBUTE_NORMAL,
             NULL);

    if (INVALID_HANDLE_VALUE != handle)
    {
        STORAGE_PROPERTY_QUERY query;
        STORAGE_DEVICE_DESCRIPTOR_DATA descriptor;
        DWORD BytesReturned;
        query.PropertyId = StorageDeviceProperty;
        query.QueryType = PropertyStandardQuery;

        if (true == DeviceIoControl(
                handle,
                IOCTL_STORAGE_QUERY_PROPERTY,
                &query,
                sizeof(STORAGE_PROPERTY_QUERY),
                &descriptor,
                sizeof(STORAGE_DEVICE_DESCRIPTOR_DATA),
                &BytesReturned,
                NULL))
        {
            device = new SKBaseDeviceInfo();
            device->deviceHandle = handle;
            device->devicePath = devicePath;
            switch(descriptor.desc.BusType)
            {
                case BusTypeSata:
                case BusTypeAta:
                case BusTypeRAID:
                {
                    device->type = isSat(handle) ? SAT : ATA;
                    break;
                }

                case BusTypeUsb:
                {
                    device->type = getUsbType(handle);
                    break;
                }
                case BusTypeNvme:
                {
                    device->type = NVME;
                    break;
                }

                default:
                {
                    delete device;
                    device = nullptr;
                    CloseHandle(handle);
                    break;
                }
            }
        }
    }

    return device;
}

SKDeviceSpaceInfo* SKStorageProtocol::queryDeviceSpaceInfo()
{
    DISK_GEOMETRY diskGMT;
    STORAGE_DEVICE_NUMBER deviceSDN;
    DWORD bytesReturned;

    // query total device size
    if (false == DeviceIoControl(this->handle, IOCTL_DISK_GET_DRIVE_GEOMETRY, nullptr, 0, &diskGMT, sizeof(DISK_GEOMETRY), &bytesReturned, nullptr))
    {
        return nullptr;
    }

    U64 totalDeviceInBytes = diskGMT.Cylinders.QuadPart * diskGMT.TracksPerCylinder * diskGMT.SectorsPerTrack * diskGMT.BytesPerSector;
    U64 usedDeviceInBytes = 0;

    // query device number from device handle
    if (false == DeviceIoControl(this->handle, IOCTL_STORAGE_GET_DEVICE_NUMBER, nullptr, 0, &deviceSDN, sizeof(STORAGE_DEVICE_NUMBER), &bytesReturned, nullptr))
    {
        return nullptr;
    }

    char volName[MAX_PATH];
    HANDLE handleFVolume;

    // enumerate per volume to calculate used disk size if volume device number matched to device number
    handleFVolume = FindFirstVolumeA(volName, sizeof(volName));
    if (INVALID_HANDLE_VALUE != handleFVolume)
    {
        char orgVolName[MAX_PATH];
        memset(orgVolName, 0, MAX_PATH);

        do
        {
            size_t volNameLength = strlen(volName);
            memcpy(orgVolName, volName, volNameLength);
            if (volName[volNameLength-1] == '\\') // remove at back character '\' if exists
            {
                volName[volNameLength-1] = 0;
            }

            // get volume handle via volume name
            HANDLE hVol = CreateFileA(volName, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
            if (INVALID_HANDLE_VALUE != hVol)
            {
                STORAGE_DEVICE_NUMBER partitionSDN;

                // query volume device number
                if (DeviceIoControl(hVol, IOCTL_STORAGE_GET_DEVICE_NUMBER, nullptr, 0, &partitionSDN, sizeof(partitionSDN), &bytesReturned, nullptr))
                {
                    // if partition device number matched to the device number
                    if (deviceSDN.DeviceNumber == partitionSDN.DeviceNumber && deviceSDN.DeviceType == partitionSDN.DeviceType)
                    {
                        ULARGE_INTEGER available, total, free;

                        // query volume sapce information
                        if (GetDiskFreeSpaceExA(orgVolName, &available, &total, &free))
                        {
                            usedDeviceInBytes += (total.QuadPart - free.QuadPart);
                        }
                    }
                }

                CloseHandle(hVol);
            }
        } while (FindNextVolumeA(handleFVolume, volName, sizeof(volName)));

        FindVolumeClose(handleFVolume);
    }

    SKDeviceSpaceInfo* deviceSpaceInfo = new SKDeviceSpaceInfo();
    deviceSpaceInfo->totalSizeInBytes = totalDeviceInBytes;
    deviceSpaceInfo->usedSizeInBytes = usedDeviceInBytes;
    deviceSpaceInfo->freeSizeInBytes = (totalDeviceInBytes - usedDeviceInBytes);

    return deviceSpaceInfo;
}

SKReturnCode SKStorageProtocol::queryBootableStatus(bool &isBootable)
{
    STORAGE_DEVICE_NUMBER deviceSDN;
    DWORD bytesReturned;

    // query device number from device handle
    if (false == DeviceIoControl(this->handle, IOCTL_STORAGE_GET_DEVICE_NUMBER, nullptr, 0, &deviceSDN, sizeof(STORAGE_DEVICE_NUMBER), &bytesReturned, nullptr))
    {
        return SKReturnCode::SKErrorUnknown;
    }

    char volName[MAX_PATH];
    HANDLE handleFVolume;
    boolean bootableDevice = false;
    _PARTITION_STYLE partitionStyle = _PARTITION_STYLE::PARTITION_STYLE_RAW;
    boolean bootIndicator = false;
    boolean efiSystemPartitionExists = false;
    U32 basicDataPartitionCount = 0;

    // enumerate per volume to detect partition info
    handleFVolume = FindFirstVolumeA(volName, sizeof(volName));
    if (INVALID_HANDLE_VALUE != handleFVolume)
    {
        char orgVolName[MAX_PATH];
        memset(orgVolName, 0, MAX_PATH);

        do
        {
            size_t volNameLength = strlen(volName);
            memcpy(orgVolName, volName, volNameLength);
            if (volName[volNameLength-1] == '\\') // remove at back character '\' if exists
            {
                volName[volNameLength-1] = 0;
            }

            // get volume handle via volume name
            HANDLE hVol = CreateFileA(volName, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
            if (INVALID_HANDLE_VALUE != hVol)
            {
                STORAGE_DEVICE_NUMBER partitionSDN;

                // query volume device number
                if (DeviceIoControl(hVol, IOCTL_STORAGE_GET_DEVICE_NUMBER, nullptr, 0, &partitionSDN, sizeof(partitionSDN), &bytesReturned, nullptr))
                {
                    // if partition device number matched to the device number and device type
                    if (deviceSDN.DeviceNumber == partitionSDN.DeviceNumber && deviceSDN.DeviceType == partitionSDN.DeviceType)
                    {
                        PARTITION_INFORMATION_EX partitionInfo;
                        if (DeviceIoControl (hVol, IOCTL_DISK_GET_PARTITION_INFO_EX, nullptr, 0, &partitionInfo, sizeof(PARTITION_INFORMATION_EX), &bytesReturned, nullptr))
                        {
                            if (PARTITION_STYLE_MBR == partitionInfo.PartitionStyle)
                            {
                                partitionStyle = PARTITION_STYLE_MBR;
                                if (true == partitionInfo.Mbr.BootIndicator)
                                {
                                    bootIndicator = true;
                                    break;
                                }
                            }
                            else if (PARTITION_STYLE_GPT == partitionInfo.PartitionStyle)
                            {
                                partitionStyle = PARTITION_STYLE_GPT;
                                // Check if EFI system partition exists
                                if (0 == wcsncmp(partitionInfo.Gpt.Name, EFI_SYSTEM_PARTITION_NAME, EFI_SYSTEM_PARTITION_LENGTH))
                                {
                                    efiSystemPartitionExists = true;
                                }
                                // Count many of Basic data partitions
                                if (0 == wcsncmp(partitionInfo.Gpt.Name, BASIC_DATA_PARTITION_NAME, BASIC_DATA_PARTITION_LENGTH))
                                {
                                    ++basicDataPartitionCount;
                                }
                            }
                        }
                    }
                }

                CloseHandle(hVol);
            }
        } while (FindNextVolumeA(handleFVolume, volName, sizeof(volName)));

        FindVolumeClose(handleFVolume);
    }

    isBootable = false;

    // Partition style is MBR and one of partition' boot indicator activated
    if (PARTITION_STYLE_MBR == partitionStyle && true == bootIndicator)
    {
        isBootable = true;
    }

    // Partition style is GPT, exists one EFI system partition and exists at least one basic data partition
    if (PARTITION_STYLE_GPT == partitionStyle && true == efiSystemPartitionExists && 0 < basicDataPartitionCount)
    {
        isBootable = true;
    }
    return SKReturnCode::SKSucceeded;
}
