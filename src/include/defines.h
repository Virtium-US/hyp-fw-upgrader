#ifndef StorageApiDefines_h
#define StorageApiDefines_h

#include "StorageKitTypes.h"

#ifdef WIN32
#define os_win
#elif defined(__linux__) || defined(linux) || defined(__linux)
#define os_linux
#endif

#ifdef SHARED_LIBRARY
#ifdef os_win
#if defined STORAGEAPI_LIBRARY
#define STORAGEAPI_DLLSPEC __declspec(dllexport)
#else
#define STORAGEAPI_DLLSPEC __declspec(dllimport)
#endif
#else
#define STORAGEAPI_DLLSPEC // NOT os_win
#endif
#else
#define STORAGEAPI_DLLSPEC // STATIC_LIBRARY
#endif

/**
 * @brief Declares command type.
 */
enum SKCommandType
{
    /** ATA command */
    ATA_COMMAND,
    /** SCSI command */
    SCSI_COMMAND,
    /** NVME command */
    NVME_COMMAND,
    /** Number of types */
    TYPE_COUNT
};

/**
 * @brief Declares return code for storage-api. Includes both ATA and SCSI.
 */
enum SKReturnCode
{
    /** No error */
    SKSucceeded,
    /** Invalid command */
    SKErrorInvalidCommand,
    /** Invalid parameters */
    SKErrorInvalidParameters,
    /** Error IO */
    SKErrorIO,

    /** Error command completion time out */
    SKErrorRegisterCommandCompletionTimeOut,
    /** Error end of media */
    SKErrorRegisterEndOfMedia,
    /** Error abort */
    SKErrorRegisterAbort,
    /** Error ID not found */
    SKErrorRegisterIDNotFound,
    /** Error uncorrectable */
    SKErrorRegisterUncorrectable,
    /** Error interface CRC */
    SKErrorRegisterInterfaceCRC,

    /** Error check condition failed. */
    SKErrorScsiCheckCondition,
    /** Error command busy. */
    SKErrorScsiDeviceBusy,
    /** Error task set full. */
    SKErrorScsiTaskSetFull,
    /** Error ACA active. */
    SKErrorScsiACAActive,
    /** Error task aborted. */
    SKErrorScsiTaskAborted,

    /** Error invalid firmware slot. */
    SKErrorNvmeInvalidFirmwareSlot,
    /** Error invalid firmware image. */
    SKErrorNvmeInvalidFirmwareImage,
    /** Error firmware activation requires conventional reset. */
    SKErrorNvmeActivationRequireConventionalReset,
    /** Error firmware activation requires NVM subsystem reset. */
    SKErrorNvmeActivationRequireNVMSubSystemReset,
    /** Error firmware activation requires controller level reset. */
    SKErrorNvmeActivationRequireControllerLevelReset,
    /** Error firmware activation requires maximum time violation. */
    SKErrorNvmeRequireMaximumTimeViolation,
    /** Error firmware activation prohibited. */
    SKErrorNvmeActivationProhibited,
    /** Error overlapping range. */
    SKErrorNvmeOverlappingRange,
    /** Error boot partition write prohibited. */
    SKErrorNvmeBootPartitionWriteProhibited,

    /** Error no sense. */
    SKErrorSenseNoSense,
    /** Error recovered error. */
    SKErrorSenseRecoveredError,
    /** Error not ready. */
    SKErrorSenseNotReady,
    /** Error medium error. */
    SKErrorSenseMediumError,
    /** Error hardware error. */
    SKErrorSenseHardwareError,
    /** Error illegal request. */
    SKErrorSenseIllegalRequest,
    /** Error unit attention. */
    SKErrorSenseUnitAttention,
    /** Error data protect. */
    SKErrorSenseDataProtect,
    /** Error blank check. */
    SKErrorSenseBlankCheck,
    /** Error vendor specific. */
    SKErrorSenseVendorSpecific,
    /** Error copy aborted. */
    SKErrorSenseCopyAborted,
    /** Error aborted command. */
    SKErrorSenseAbortedCommand,
    /** Error volume overflow. */
    SKErrorSenseVolumeOverflow,
    /** Error miscompare. */
    SKErrorSenseMiscompare,
    /** Error compared. */
    SKErrorSenseCompleted,
    /** Error unknown. */
    SKErrorUnknown
};

/**
*
* @section DESCRIPTION
*
* An object is responsible to store ATA output task file register.
*
*/
struct SKAtaOutputTaskFileRegister
{
    /** Error. */
    U8 Error;
    /** Sector count. */
    U16 Count;
    /** Lba low. */
    U16 LbaLow;
    /** Error mid. */
    U16 LbaMid;
    /** Error high. */
    U16 LbaHigh;
    /** Device. */
    U8 Device;
    /** Status. */
    U8 Status;
    /** Reserved. */
    U8 Reserved;
};

/**
*
* @section DESCRIPTION
*
* An object is responsible to store SCSI output formated as a byte array.
*
*/
const U32 SENSE_BUFFER_LENGTH = 32;
struct SKScsiSense
{
    /** Byte array of sense */
    U8 Sense[SENSE_BUFFER_LENGTH];
};

/**
*
* @section DESCRIPTION
*
* An object is responsible to store space information of a device.
*
*/
struct SKDeviceSpaceInfo
{
    U64 totalSizeInBytes;
    U64 usedSizeInBytes;
    U64 freeSizeInBytes;
};

/**
 * @brief Declares inquiry page code.
 */
enum SKInquiryPageCode
{
    /** Standard */
    SKStandard = 0xFF,

    /** Supported VPD Pages */
    SKSupportedVPDPages = 0x00,
    /** ASCII Information */
    SKASCIIInformation = 0x01, // 01h - 7Fh
    /** Unit Serial Number */
    SKUnitSerialNumber = 0x80,
    /** Device Identification */
    SKDeviceIdentification = 0x83,
    /** Software Interface Identification */
    SKSoftwareInterfaceIdentification = 0x84,
    /** Management Network Addresses */
    SKManagementNetworkAddresses = 0x85,
    /** Extended Inquiry Data */
    SKExtendedInquiryData = 0x86,
    /**  Mode Page Policy */
    SKModePagePolicy = 0x87,
    /** SCSI Ports */
    SKSCSIPorts = 0x88,
    /**  Power Condition */
    SKPowerCondition = 0x8A,
    /** Device Constituents */
    SKDeviceConstituents = 0x8B,
    /** CFA Profile Information */
    SKCFAProfileInformation = 0x8C,
    /**  Power Consumption */
    SKPowerConsumption = 0x8D,
    /** Block Limits */
    SKBlockLimits = 0xB0,
    /**  Block Device Characteristics */
    SKBlockDeviceCharacteristics = 0xB1,
    /**  Logical Block Provisioning */
    SKLogicalBlockProvisioning = 0xB2,
    /** Referrals */
    SKReferrals = 0xB3,
    /** Supported Block Lengths and Protection Types */
    SKSupportedBlockLengthsAndProtectionTypes = 0xB4,
    /** Block Device Characteristics Extension */
    SKBlockDeviceCharacteristicsExtension = 0xB5,
    /**  Zoned Block Device Characteristics */
    SKZonedBlockDeviceCharacteristics = 0xB6,
    /** Block Limits Extension */
    SKBlockLimitsExtension = 0xB7,
    /** Firmware Numbers Page */
    SKFirmwareNumbersPage = 0xC0,
    /**  Date Code Page */
    SKDateCodePage = 0xC1,
    /** Jumper Settings Page */
    SKJumperSettingsPage = 0xC2,
    /**  Device Behavior Page */
    SKDeviceBehaviorPage = 0xC3
};

#ifdef os_win
#include "windows.h"
typedef HANDLE DeviceHandle;
#elif defined os_linux
typedef U32 DeviceHandle;
#endif

const U32 SECTOR_SIZE_IN_BYTES = 512;

#endif
