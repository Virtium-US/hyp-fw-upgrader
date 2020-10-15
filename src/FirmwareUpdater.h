#ifndef __FIRMWARE_UPDATER_H__
#define __FIRMWARE_UPDATER_H__

#include <StorageKitTypes.h>
#include <StorageKitScsiProtocol.h>
#include <StorageKitU9VcCommandDesc.h>

namespace updater 
{

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
    U32 sectorsInFirstFW;        // Number n of sectors in the first firmware part (0..129)
    U32 sectorsInSecondFW;       // Number m of sectors in the second firmware part (0..127)
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
* NOTE: NOT ALL FIELDS ARE PRESENT. Refer to the Target Info section in the Hyperstone U9 
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

// the data contained by a dd.txt file for a Hyperstone firmware archive
typedef struct
{
    /* data */
} DeviceDescription_t;

class UpdateExeception : public std::exception
{
    private:
        std::string msg;

    public:
        UpdateExeception(std::string msg, const char* path);
        const char* what() const throw ();
};

// wrapper class to abstract away the details of a firmware upgrade for Ux devices
class FirmwareUpdater {
    private:
        SKScsiProtocol* scsiInterface;

    public:
        FirmwareUpdater(char* devPath);
        ~FirmwareUpdater();

    public:
        FWVersionInfo_t readFirmwareVersion();
        TargetInfo_t readTargetInfo();
        DeviceDescription_t loadDDFile(const char* path);
    //private:

};

}

#endif
