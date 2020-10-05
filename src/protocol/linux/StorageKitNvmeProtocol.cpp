#include <fcntl.h>
#include <scsi/sg.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include "StorageKitNvmeProtocol.h"

using namespace std;

struct SKNvmeAdmin {
    U8	OpCode;
    U8	Flags;
    U16	Rsvd1;
    U32	Nsid;
    U32	Cdw2;
    U32	Cdw3;
    U64	MetaData;
    U64	Addr;
    U32	MetaDataLength;
    U32	DataLength;
    U32	Cdw10;
    U32	Cdw11;
    U32	Cdw12;
    U32	Cdw13;
    U32	Cdw14;
    U32	Cdw15;
    U32	TimeoutInMs;
    U32	Result;
};

struct SKNvmeUser {
    U8	OpCode;
    U8	Flags;
    U16	Control;
    U16	Nblocks;
    U16	Rsvd;
    U64	MetaData;
    U64	Addr;
    U64	Slba;
    U32	Dsmgmt;
    U32	RefTag;
    U16	AppTag;
    U16	AppMask;
};

#define NVME_IOCTL_ID           _IO('N', 0x40)
#define NVME_IOCTL_ADMIN_CMD	_IOWR('N', 0x41, struct SKNvmeAdmin)
#define NVME_IOCTL_USER_CMD     _IOW('N', 0x42, struct SKNvmeUser)

int SKNvmeProtocol::getNsid(const std::string& devicePath, const DeviceHandle &handle)
{
    static struct stat nvmeStat;
    int retCode = fstat(handle, &nvmeStat);

    if (retCode < 0)
    {
        return retCode;
    }

    if (!S_ISBLK(nvmeStat.st_mode))
    {
        return -ENOTBLK;
    }

    return ioctl(handle, NVME_IOCTL_ID);
}

SKReturnCode SKNvmeProtocol::issueNvmeCommand(const DeviceHandle &handle, const SKNvmeCommandDesc *cmdDesc, SKAlignedBuffer *buffer, U16 &lastError)
{
    int retCode = 1;

    if (SKNvmeCommandDesc::SKNvmeFieldFormatting::COMMAND_ADMIN == cmdDesc->fieldFormatting)
    {
        SKNvmeAdmin cmd;
        memset(&cmd, 0, sizeof(cmd));

        cmd.OpCode = cmdDesc->inputFields.OpCode;
        cmd.Nsid = cmdDesc->inputFields.AdminCommand.Nsid;
        cmd.DataLength = cmdDesc->dataTransferLengthInSectors * SECTOR_SIZE_IN_BYTES;
        if (nullptr != buffer->ToDataBuffer())
        {
            cmd.Addr = (U64)(buffer->ToDataBuffer());
        }
        cmd.Cdw10 = cmdDesc->inputFields.AdminCommand.Cdw10;
        cmd.Cdw11 = cmdDesc->inputFields.AdminCommand.Cdw11;
        cmd.Cdw12 = cmdDesc->inputFields.AdminCommand.Cdw12;
        cmd.Cdw13 = cmdDesc->inputFields.AdminCommand.Cdw13;
        cmd.TimeoutInMs = cmdDesc->inputFields.AdminCommand.TimeoutInMs;

        // TODO: update remaining fields

        retCode = ioctl(handle, NVME_IOCTL_ADMIN_CMD, &cmd);
    }
    else if (SKNvmeCommandDesc::SKNvmeFieldFormatting::COMMAND_IO == cmdDesc->fieldFormatting)
    {
        // TODO: handle ioctl for command IO
    }

    if (0 != retCode)
    {
        lastError = retCode;
        return SKErrorIO;
    }

    return SKNvmeProtocol::convertNvmeStatus(retCode);
}

SKNvmeProtocol::SKNvmeProtocol(const std::string &devicePath, const DeviceHandle &handle) :
    SKStorageProtocol(devicePath, handle)
{
}

SKNvmeProtocol::~SKNvmeProtocol()
{
}

SKReturnCode SKNvmeProtocol::issueCommand(const SKNvmeCommandDesc *cmdDesc, SKAlignedBuffer *buffer)
{
    return issueNvmeCommand(this->handle, cmdDesc, buffer, this->lastError);
}
