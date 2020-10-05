#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <mntent.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

#include "StorageKitStorageProtocol.h"
#include "StorageKitAtaCommandDesc.h"
#include "StorageKitNvmeProtocol.h"
#include "StorageKitScsiProtocol.h"

using namespace std;

#define DETECTABLE_BOOTABLE_SECTORS         3
#define PARTITION_NUMBERS                   4
#define MBR_PARTITION_OFFSET                0x01BE

#define MBR_SIGNATURE_OFFET                 0x1FE
#define GPT_SIGNATURE_OFFET                 0x00

#define GPT_PARTITION_ENTRIES_SIZE          128
#define GPT_PARTITION_ATTRIBUTE_OFFSET      0x30

#define EFI_SYSTEM_PARTITION_GUID           "C12A7328-F81F-11D2-BA4B-00A0C93EC93B"

#define COMMAND_TIMEOUT_IN_SECONDS          2

const U8 MBR_SIGNATURE_BYTE_ARRAY[] = {0x55, 0xAA};
const U8 GPT_SIGNATURE_BYTE_ARRAY[] = {0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54};

struct MBRPartition
{
    U8 BootIndicator;    /* 0x80 - active */
    U8 Head;             /* starting head */
    U8 Sector;           /* starting sector */
    U8 Cylinder;         /* starting cylinder */
    U8 SystemIndex;      /* what partition type */
    U8 EndHead;          /* end head */
    U8 EndSector;        /* end sector */
    U8 EndCylinder;      /* end cylinder */
    U8 Start4[4];        /* starting sector counting from 0 */
    U8 Size4[4];         /* number of sectors in partition */
};

struct GPTPartitionAttribute
{
    U8 platformRequired :1, efiFirmware :1, bootable :1, reserved: 5;
};

bool isMBRType(const U8* lba0)
{
    const U8 *signature = (U8*)(lba0 + MBR_SIGNATURE_OFFET);
    return (0 == memcmp(MBR_SIGNATURE_BYTE_ARRAY, signature, sizeof(MBR_SIGNATURE_BYTE_ARRAY))  ? true : false);
}

bool isGPType(const U8* lba1)
{
    const U8 *signature = (U8*)(lba1 + GPT_SIGNATURE_OFFET);
    int aa = sizeof (GPT_SIGNATURE_BYTE_ARRAY);
    return (0 == memcmp(GPT_SIGNATURE_BYTE_ARRAY, signature, sizeof (GPT_SIGNATURE_BYTE_ARRAY)) ? true : false);
}

bool isMBRPartitionsBootable(const U8* mbrBuffer)
{
    bool isBootable = false;
    MBRPartition *partition;
    U32 partitionOffset = MBR_PARTITION_OFFSET;

    // Loop all partitions to check if one of them active then the disk is bootable
    for (U32 i = 0; i < PARTITION_NUMBERS; ++i)
    {
        partition = (MBRPartition*)(mbrBuffer + partitionOffset);
        if (1 == (partition->BootIndicator >> 7)) // active or bootable if bit 7 set enabled
        {
            isBootable = true;
            break;
        }
        partitionOffset += sizeof(MBRPartition);
    }
    return isBootable;
}

std::string getPartitionGUID(const U8* uuid)
{
    char str[40];
    sprintf(str, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
            uuid[3], uuid[2], uuid[1], uuid[0], uuid[5], uuid[4], uuid[7], uuid[6], uuid[8], uuid[9],
            uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
    return std::string(str);
}

bool isGPTPartitionsBootable(const U8* partitionEntriesBuffer)
{
    bool isBootable = false;
    GPTPartitionAttribute *partitionAtt;
    U32 partitionOffset = 0;

    // if uniqueGUID equals to EFI GUID then it is a bootable
    std::string uniqueGUID = getPartitionGUID(partitionEntriesBuffer);
    if (EFI_SYSTEM_PARTITION_GUID == uniqueGUID)
    {
        return true;
    }

    // Loop all partitions to check if one of them active then the disk is bootable
    for (U32 i = 0; i < PARTITION_NUMBERS; ++i)
    {
        partitionAtt = (GPTPartitionAttribute*)(partitionEntriesBuffer + GPT_PARTITION_ATTRIBUTE_OFFSET);
        if (1 == partitionAtt->bootable)
        {
            isBootable = true;
            break;
        }
        partitionOffset += GPT_PARTITION_ENTRIES_SIZE;
    }
    return isBootable;
}

SKStorageProtocol::SKStorageProtocol(const std::string &devicePath, const DeviceHandle &handle)
{
    this->devicePath = devicePath;
    this->handle = handle;
}

SKStorageProtocol::~SKStorageProtocol()
{
    close(this->handle);
}

U16 SKStorageProtocol::getLastError() const
{
    return this->lastError;
}

DeviceHandle SKStorageProtocol::getDeviceHandle() const
{
    return this->handle;
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

SKDeviceType getDeviceType(const DeviceHandle &handle, const std::string &devicePath)
{
    // If device path contains nvme, it's a Nvme device
    if (nullptr != strstr(devicePath.c_str(), "/dev/nvme"))
    {
        return NVME;
    }

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
    SKReturnCode identifyReturnCode = SKScsiProtocol::issueAtaCommand(handle, ataCmd, &identifyBuffer, lastError, lastOutputTFR);
    delete ataCmd;

    if (SKReturnCode::SKSucceeded == identifyReturnCode)
    {
        // If the usb vendor is Virtium drive, it's a Hyperstone device
        // Otherwise, it's a sat device
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

// filter device path if it starts with sd or sg and is must matched the appropriated self-format
int filterDevicePaths(const struct dirent *d)
{
    if ('.' == d->d_name[0])
    {
        return 0;
    }

    // keep verifying if candidate contains one of following (sd, sg, nvme)
    if (!strstr(d->d_name, "sd") && !strstr(d->d_name, "sg") && !strstr(d->d_name, "nvme"))
    {
        return 0;
    }

    char path[128];
    struct stat bd;
    snprintf(path, sizeof(path), "/dev/%s", d->d_name);
    if (stat(path, &bd))
    {
        return 0;
    }
    if (!S_ISBLK(bd.st_mode))
    {
        return 0;
    }

    if (strstr(d->d_name, "nvme"))
    {
        int ctrl, ns, part;
        if (3 == sscanf(d->d_name, "nvme%dn%dp%d", &ctrl, &ns, &part))
        {
            return 0;
        }
        return 1; // nvme device
    }
    else
    {
        int part;
        char devCharacter;
        if (strstr(d->d_name, "sd"))
        {
            if (2 == sscanf(d->d_name, "sd%c%d", &devCharacter, &part))
            {
                return 0;
            }
            return 1; // ex: sda1, sdy, sdaa
        }
        else if (strstr(d->d_name, "sg"))
        {
            if (2 == sscanf(d->d_name, "sg%c%d", &devCharacter, &part))
            {
                return 0;
            }
            return 1; // ex: sg1, sgy, sgbb
        }
    }

    return 0;
}

void SKStorageProtocol::scan(std::vector<SKBaseDeviceInfo*> &devices)
{
    SKBaseDeviceInfo* device;
    char buffer[128];
    string devicePath;

    struct dirent **dirents;
    int n = scandir("/dev/", &dirents, filterDevicePaths, alphasort);

    for (int i = 0; i < n; ++i)
    {
        sprintf(buffer, "/dev/%s", dirents[i]->d_name);

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
    DeviceHandle handle = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
    SKBaseDeviceInfo* device = nullptr;
    if (-1 != handle)
    {
        U64 deviceSizeInBytes;
        int retCode = ioctl(handle, BLKGETSIZE64, &deviceSizeInBytes); // get device size in bytes
        if (0 == retCode && 0 < deviceSizeInBytes) // just keep device which has space size, avoid usb hub plugin
        {
            device = new SKBaseDeviceInfo();
            device->devicePath = devicePath;
            device->deviceHandle = handle;
            device->type = getDeviceType(handle, devicePath);
        }
        else
        {
            close(handle);
        }
    }

    return device;
}

SKDeviceSpaceInfo* SKStorageProtocol::queryDeviceSpaceInfo()
{
    int retCode;
    U64 fileSizeInBytes;

    // get total size of the device
    retCode = ioctl(handle, BLKGETSIZE64, &fileSizeInBytes);
    if (0 != retCode)
    {
        return nullptr;
    }

    U64 totalDeviceInBytes = fileSizeInBytes;
    U64 usedDeviceInBytes = 0;

    struct mntent* ent;
    FILE* aFile;

    // query all the mounts which mapping the device path
    aFile = setmntent("/proc/mounts", "r");
    if (nullptr == aFile)
    {
        return nullptr;
    }

    struct statvfs stat;
    size_t deviceLength = strlen(this->devicePath.c_str());
    while (NULL != (ent = getmntent(aFile)))
    {
        // just handle to query file system information of the mounted directory mapped to device path
        if (0 == strncmp(this->devicePath.c_str(), ent->mnt_fsname, deviceLength))
        {
            if (0 == statvfs(ent->mnt_dir, &stat))
            {
                usedDeviceInBytes += ((stat.f_blocks - stat.f_bavail) * stat.f_bsize);
            }
        }
    }

    endmntent(aFile);

    SKDeviceSpaceInfo* deviceSpaceInfo = new SKDeviceSpaceInfo();
    deviceSpaceInfo->totalSizeInBytes = totalDeviceInBytes;
    deviceSpaceInfo->usedSizeInBytes = usedDeviceInBytes;
    deviceSpaceInfo->freeSizeInBytes = (totalDeviceInBytes - usedDeviceInBytes);

    return deviceSpaceInfo;
}

SKReturnCode SKStorageProtocol::queryBootableStatus(bool &isBootable)
{
    isBootable = false;

    const U32 bufferSize = DETECTABLE_BOOTABLE_SECTORS * SECTOR_SIZE_IN_BYTES;
    U8 buffer[bufferSize];

    // Read 3 first buffers
    // LBA 0: contains protective MBR
    // LBA 1: contains primary GPT header
    // LBA 2: contains GPT partition entries
    if (bufferSize != read(this->handle, &buffer, bufferSize))
    {
        return SKReturnCode::SKErrorUnknown;
    }

    // MBR buffer at sector 0
    U8 *mbrBuffer = buffer;

    // GPT buffer at sector 1
    U8 *gptBuffer = buffer + SECTOR_SIZE_IN_BYTES;

     // handle check if the device type is MBR first
    if (true == isMBRType(mbrBuffer))
    {
        isBootable = isMBRPartitionsBootable(mbrBuffer);
    }

    // then handle check partition bootable if the device type is GPT
    if (false == isBootable && true == isGPType(gptBuffer))
    {
        // GPT Partition Entries buffer at sector 2
        U8 *gptPartitionEntriesBuffer = buffer + (2 * SECTOR_SIZE_IN_BYTES);
        isBootable = isGPTPartitionsBootable(gptPartitionEntriesBuffer);
    }

    return SKReturnCode::SKSucceeded;
}
