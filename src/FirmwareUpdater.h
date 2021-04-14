#ifndef __FIRMWARE_UPDATER_H__
#define __FIRMWARE_UPDATER_H__

#include <StorageKitTypes.h>
#include <StorageKitScsiProtocol.h>
#include <StorageKitU9VcCommandDesc.h>

namespace updater 
{

#define MAX_LINE_LEN 1024

// the data returned by the Read Firmware Version VSC.
typedef struct 
{
    U8 fwVersionDate[6];              // 0..5 Firmware version date (year, month, day)
    U8 controllerRevisionIdString[2]; // 6..7 Controller revision ID as character string
    U8 unused_1[4];                   // 8..11 –
    U8 fwRevisionId[4];               // 12..15 Firmware revision ID as character string (e.g. “1.03”)
    U8 fwInfoText[4];                 // 16..19 Additional firmware version information text
    U8 unused_2[2];                   // 20..21 –
    U8 controllerSubRevisionId;       // 22 Controller sub-revision ID
    U8 controllerRevisionId;          // 23 Controller revision ID
    U8 fwFeatures[4];                 // 24..27 Flash specific firmware feature bit vector 
    U8 generalFwFeatures[4];          // 28..31 General firmware feature bit vector
    U8 reserved[8];                   // 32..39 Reserved
    U8 fwManagement[4];               // 40..43 Firmware management data structure version 
    U8 unused_3[468];                 // 44..511 –
} FWVersionInfo_t;

// the data supplied to the Firmware Update Prepare VSC
typedef struct
{
    U32 sectorsInFirstFW;        // Number n of sectors in the first firmware part (0..257)
    U32 sectorsInSecondFW;       // Number m of sectors in the second firmware part (0..255)
    U32 sectorsAnchorProg;       // Number k of Anchor program sectors (0..1)
    U32 clearMaskSpecificFW;     // Bits to be cleared in flash specific firmware features
    U32 setMaskSpecificFW;       // Bits to be set in flash specific firmware features
    U32 clearMaskGeneralFW;      // Bits to be cleared in flash general firmware features
    U32 setMaskGeneralFW;        // Bits to be set in flash general firmware features
    U32 clearMaskDriverStrength; // Bits to be cleared in driver strength configuration
    U32 setMaskDriverStrength;   // Bits to be set in driver strength configuration
} FWUpdatePrepareData_t;

/* 
* data returned by the Target Info VSC. 
*
* NOTE: NOT ALL FIELDS ARE PRESENT. Refer to the Target Info section in the Hyperstone Ux 
* Firmware Manual for more information
*/
typedef struct
{
    U32 signatureWord;    // 0..3 Signature word (always 89ABCDEFh) 
    U32 romVersion;       // 4..7 ROM version
    U32 flashId_0;        // 8..11 Flash ID (bytes 0..3)
    U16 firmwareType;     // 12..13 Firmware type 0: U9 block based firmware, 1: U9 hyMap firmware 
    U16 cardType;         // 14..15 Card type 1: USB flash disk 
    U16 unused;           // 16..17 Unused (always 0)
    U16 flashId_1;        // 18..19 Flash ID (bytes 4..5)
    U16 sectorsPerECC;    // 20..21 Sectors per ECC unit
    U16 sectorsPerPage;   // 22..23 Sectors per flash page
    U32 interleaveFactor; // 24..27 Interleave factor
} TargetInfo_t;

/*
* fields used by the input of the Update Customer Specific Device Description Data VSC
* and output of the Read Customer Specific Device Description Data VSC
*
* NOTE: NOT ALL FIELDS ARE PRESENT. Refer to the Update Customer Specific Device 
* Description Data section in the Hyperstone Ux Firmware Manual for more information
*/
typedef struct
{
    U32 formatVersion;      // 0..3 Format version. Current format is version 2
    U8 ataFirmwareRev[8];   // 4..11 ATA Firmware revision within ATA Identify Device Information
    U16 _resv;              // 12..13 Reserved
    U16 bcdDevice;          // 14..15 bcdDevice Version within the USB Device Descriptor
    U32 scsiInqData_LUN0;   // 16..19 Product Revision Level within the SCSI Inquiry data for LUN 0
    U32 scsiInqData_LUN1;   // 20..23 Product Revision Level within the SCSI Inquiry data for LUN 1
    U32 scsiInqData_LUN2;   // 24..27 Product Revision Level within the SCSI Inquiry data for LUN 2
    U32 scsiInqData_LUN3;   // 28..31 Product Revision Level within the SCSI Inquiry data for LUN 3
} UpdateCustSpecDD_t;

/*
* a line from a dd file. for example:
* ; M60A MT29F4G08ABADA
*       2cdc90950000 1 1 4096 80 256 0x01FF 0000000A m11f1 am11i1 m11p1 1 160 2 4 100000
* Device description lines consist of fields separated by white space. These fields are:
* - the flash memory device ID (six bytes),
* - the flash memory interface type (1 for legacy, 2 for Toggle and 4 for ONFI-2),
* - the interleave factor (1 or 2) that applies to the firmware,
* - the number of blocks on the flash memory chip,
* - the maximum number of defect blocks per flash memory chip,
* - the number of sectors (512 bytes) per flash memory block,
* - a threshold setting for the wear leveling, in hex,
* - a 32 bit number corresponding to certain flash memory device features, in hex,
* - the names of the firmware, anchor block and preformat hex files for this flash memory 
* device and interleave setting,
* - four fields that define the controller and flash operating frequency,
* - the target number of erase cycles per flash block, for the SMART calculations.
*
* NOTE: NOT ALL FIELDS ARE PRESENT. Refer to the Device Description File section in the 
* Hyperstone Ux Firmware Manual for more information
*/
typedef struct
{
    char flashDeviceId[13]; // I know the exact size is 13 bytes
    char specificFwFeatures[32]; // making the assumption that 32 bytes is the max size this field can be
    char firmwareFileName[32]; // making the assumption that 32 bytes is the max size this field can be
    char anchorFileName[32]; // making the assumption that 32 bytes is the max size this field can be
} DeviceDescriptionEntry_t;

// the data contained by a dd.txt file for a Hyperstone firmware archive. NOTE: NOT ALL FIELDS ARE PRESENT.
typedef struct
{
    // -features=<generalFwFeatures>
    char generalFwFeatures[32]; // making the assumption that 32 bytes is the max size this field can be
    char drvStrengths[32]; // making the assumption that 32 bytes is the max size this field can be
    char ataFirmwareRevision[32]; // -ATAFirmwareRevision
    char bcdDevice[32]; // -bcdDevice
    char inqRevision[32]; // -INQRevision
} DeviceDescriptionData_t;

class UpdateExeception : public std::exception
{
    private:
        std::string msg;

    public:
        UpdateExeception(std::string msg, const char* value);
        UpdateExeception(std::string msg, std::string value);
        const char* what() const throw ();
};

// all of the data collected from a configuration file (dd.txt) and target info on the device
typedef struct
{
    FWVersionInfo_t info;
    DeviceDescriptionData_t ddData;
    DeviceDescriptionEntry_t ddEntry;
} DeviceInfo_t;

// wrapper class to abstract away the details of a firmware upgrade for Ux devices
class FirmwareUpdater {
    private:
        SKScsiProtocol* scsiInterface;
        DeviceInfo_t currDevice;
        std::string archivePath;

    public:
        FirmwareUpdater(const char* devPath, std::string configPath);
        ~FirmwareUpdater();

    public:
        void inspectCurrentDevice();
        int update();
        void updateCustomerVersionData();
    
    private:
        const FWVersionInfo_t readFirmwareVersion();
        const void printCustomerVersionData(); 
        const TargetInfo_t readTargetInfo();
        const DeviceDescriptionData_t loadDDData(std::string path);
        const DeviceDescriptionEntry_t findDDEntry(const TargetInfo_t &info, std::string path);
        const std::string findLineInDD(std::string path, const char* find);
        const std::string locateWord(const std::string &str, int word);
        const U32 hexStringToU32(const char* str);
        const U32 sectorsInFile(std::string pathOnDisk);
};

}

#endif
