#ifndef structures_h
#define structures_h

#include "defines.h"
#include <string>

#include "StorageKitTypes.h"

struct SKAtaDeviceInfo
{
    // Security feature
    struct
    {
        bool isSecuritySupported;
        bool isUserPasswordPresent;
        bool isSecurityFeatureSetSupported;
        bool isSecurityEnabled;
        bool isDeviceLocked;
        bool isDeviceFrozen;
        bool isPasswordAttemptExceeded;
        bool isEnhancedSecurityEraseSupported;
        bool isMasterPasswordMaximum;
    } secureErase;

    // Sanitize
    struct
    {
        bool isSanitizeAntifreezeLock;
        bool isSanitizeSupported;
        bool isCryptoScrambleSupported;
        bool isBlockEraseSupported;
        bool isOverwriteSupported;
    } sanitize;

    bool isTrustedComputingFeatureSetSupported;
};

struct SKScsiDeviceInfo
{

};

struct SKNvmeDeviceInfo
{
    int nsid;
    // Namespace
    struct
    {
        U64 size;
        U64 capacity;
        U64 use;
    } activeNamespace;
    // Firmware slot info
    struct
    {
        U8 pendingActivateSlot;     // Bytes 0: Bits 6:4
        U8 activeSlot;              // Bytes 0: Bits 2:0
        U64 fwRevSlot[7];
        // Firmware Revision for slot[n]: contains the revision of firmware download to firmware slot n
        //   If no valid firmware revision is present or if this slot is unsupported, this field shall be cleared to 0h
    } firmwareSlotInfo;

    struct
    {
        bool isCryptoEraseSupported;              // Indicates if Crypto erase suppported as part of Secure Erase or not
        bool isCryptoEraseAppliesAllNamespaces;   // Indicates if Crypto erase applies to all namespaces or single namespace
        bool isFormatAppliesAllNamespaces;        // Indicates if Format applies to all namespaces or single namespace
    } format;

    struct
    {
        bool isSanitizeSupported;
        bool isCryptoEraseSupported;
        bool isBlockEraseSupported;
        bool isOverwriteSupported;
    } sanitize;
};

union SKSpecificDeviceInfo
{
    SKAtaDeviceInfo ata;
    SKScsiDeviceInfo scsi;
    SKNvmeDeviceInfo nvme;
};

enum SKBootableStatus
{
    YES,
    NO,
    UNRECOGNIZED
};

struct SKNvmeSmartInfo
{
    U8  criticalWarning;
    U16 temperature;
    U8  availableSpare;
    U8  availableSpareThreshold;
    U8  percentageUsed;
    U8  enduranceGroupSummary;
    U64 highDataUnitRead;
    U64 lowDataUnitRead;
    U64 highDataUnitWritten;
    U64 lowDataUnitWritten;
    U64 highHostReadCommands;
    U64 lowHostReadCommands;
    U64 highHostWtittenCommands;
    U64 lowHostWrittenCommands;
    U64 highControllerBusyTime;
    U64 lowControllerBusyTime;
    U64 highPowerCycle;
    U64 lowPowerCycle;
    U64 highPowerOnHours;
    U64 lowPowerOnHours;
    U64 highUnsafeShutdowns;
    U64 lowUnsafeShutdowns;
    U64 highMediaErrors;
    U64 lowMediaErrors;
    U64 highErrorInfoLogEntryNum;
    U64 lowErrorInfoLogEntryNum;
    U32 warningCompositeTemperatureTime;
    U32 criticalCompositeTemperatureTime;
    U16 temperatureSensor1;
    U16 temperatureSensor2;
    U16 temperatureSensor3;
    U16 temperatureSensor4;
    U16 temperatureSensor5;
    U16 temperatureSensor6;
    U16 temperatureSensor7;
    U16 temperatureSensor8;
    U32 thermalManagementTemperature1TransitionCount;
    U32 thermalManagementTemperature2TransitionCount;
    U32 totalTimeForThermalManagementTemperature1;
    U32 totalTimeForThermalManagementTemperature2;
};

union SKSpecificSmartInfo
{
    SKNvmeSmartInfo nvme;
};

struct SKSmartInfo
{
    SKDeviceType deviceType;
    SKSpecificSmartInfo specific;
};

struct SKDeviceInfo
{
    std::string devicePath;
    SKDeviceType deviceType;
    std::string deviceModel;
    std::string serialNumber;
    std::string firmwareRevision;
    U64 deviceSectorCount;
    double userCapacity;

    bool isSmartSupported;
    bool isSmartEnabled;

    struct SKDeviceSpaceInfo
    {
        U64 totalSizeInBytes;
        U64 usedSizeInBytes;
        U64 freeSizeInBytes;
    };
    SKDeviceSpaceInfo deviceSpace;
    SKBootableStatus bootableStatus;

    SKSpecificDeviceInfo specific;
};

enum SKSecureEraseMode
{
    Enhanced = 0,
    Normal
};

enum SKSanitizeSubCommand
{
    BlockErase = 0,
    Overwrite,
    CryptoScramble,
};

enum SKSanitizeStatus
{
    IDLE = 0,
    FROZEN,
    OPERATION_IN_PROGRESS,
    OPERATION_COMPLETED,
    OPERATION_FAILED,
    ANTIFREEZE_LOCK_SET,

    UNKNOWN_STATE // fail command
};
#endif
