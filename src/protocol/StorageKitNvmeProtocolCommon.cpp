#include "StorageKitNvmeProtocol.h"

const U8 SKNvmeProtocol::NVME_STATUS_SUCCESS_COMPLETION = 0x00;
const U8 SKNvmeProtocol::NVME_STATUS_INVALID_FIRMWARE_SLOT = 0x06;
const U8 SKNvmeProtocol::NVME_STATUS_INVALID_FIRMWARE_IMAGE = 0x07;
const U8 SKNvmeProtocol::NVME_STATUS_FIRMWARE_ACTIVATION_REQUIRES_CONVENTIONAL_RESET = 0x0B;
const U8 SKNvmeProtocol::NVME_STATUS_FIRMWARE_ACTIVATION_REQUIRES_NVM_SUBSYSTEM_RESET = 0x10;
const U8 SKNvmeProtocol::NVME_STATUS_FIRMWARE_ACTIVATION_REQUIRES_RESET = 0x11;
const U8 SKNvmeProtocol::NVME_STATUS_FIRMWARE_ACTIVATION_REQUIRES_MAX_TIME_VIOLATION = 0x12;
const U8 SKNvmeProtocol::NVME_STATUS_FIRMWARE_ACTIVATION_PROHIBITED = 0x13;
const U8 SKNvmeProtocol::NVME_STATUS_OVERLAPPING_RANGE = 0x14;

SKReturnCode SKNvmeProtocol::convertNvmeStatus(const S32 &status)
{
    switch(status)
    {
    case NVME_STATUS_SUCCESS_COMPLETION:
        return SKSucceeded;
    case NVME_STATUS_INVALID_FIRMWARE_SLOT:
        return SKReturnCode::SKErrorNvmeInvalidFirmwareSlot;
    case NVME_STATUS_INVALID_FIRMWARE_IMAGE:
        return SKReturnCode::SKErrorNvmeInvalidFirmwareImage;
    case NVME_STATUS_FIRMWARE_ACTIVATION_REQUIRES_CONVENTIONAL_RESET:
        return SKReturnCode::SKErrorNvmeActivationRequireConventionalReset;
    case NVME_STATUS_FIRMWARE_ACTIVATION_REQUIRES_NVM_SUBSYSTEM_RESET:
        return SKReturnCode::SKErrorNvmeActivationRequireNVMSubSystemReset;
    case NVME_STATUS_FIRMWARE_ACTIVATION_REQUIRES_RESET:
        return SKReturnCode::SKErrorNvmeActivationRequireControllerLevelReset;
    case NVME_STATUS_FIRMWARE_ACTIVATION_REQUIRES_MAX_TIME_VIOLATION:
        return SKReturnCode::SKErrorNvmeRequireMaximumTimeViolation;
    case NVME_STATUS_FIRMWARE_ACTIVATION_PROHIBITED:
        return SKReturnCode::SKErrorNvmeActivationProhibited;
    case NVME_STATUS_OVERLAPPING_RANGE:
        return SKReturnCode::SKErrorNvmeOverlappingRange;
    default:
        break;
    }

    return SKErrorUnknown;
}
