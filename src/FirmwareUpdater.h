#ifndef __FIRMWARE_UPDATER_H__
#define __FIRMWARE_UPDATER_H__

#include <StorageKitTypes.h>
#include <StorageKitScsiProtocol.h>
#include <StorageKitU9VcCommandDesc.h>

// FirmwareVersionInfo is the data returned by the Read Firmware Version VSC.
struct FirmwareVersionInfo
{
    U8 fwVersionDate[6]; // 0..5 Firmware version date (year, month, day)
    U8 controllerRevisionIdString[2]; // 6..7 Controller revision ID as character string
    U8 unused_1[4]; // 8..11 –
    U8 fwRevisionId[4]; // 12..15 Firmware revision ID as character string (e.g. “1.03”)
    U8 fwInfoText[4]; // 16..19 Additional firmware version information text
    U8 unused_2[2]; // 20..21 –
    U8 controllerSubRevisionId; // 22 Controller sub-revision ID
    U8 controllerRevisionId; // 23 Controller revision ID
    U8 fwFeatures[4]; // 24..27 Flash specific firmware feature bit vector 
    U8 generalFwFeatures[4]; // 28..31 General firmware feature bit vector
    U8 reserved[8]; // 32..39 Reserved
    U8 fwManagement[4]; // 40..43 Firmware management data structure version 
    U8 unused_3[468]; // 44..511 –
};

class FirmwareUpdater {
    private:
        SKScsiProtocol* scsiInterface;

    public:
        FirmwareUpdater(char* devPath);
        ~FirmwareUpdater();

    public:
        FirmwareVersionInfo readFirmwareVersion();
};

#endif
