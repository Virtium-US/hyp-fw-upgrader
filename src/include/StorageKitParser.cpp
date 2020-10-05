#include "StorageKitParser.h"
#include "StorageKitBufferParser.h"

using namespace std;

#define ATA_IDENTIFY_WORD 27
#define ATA_IDENTIFY_LENGTH 20
#define ATA_SERIAL_NUMBER_WORD 10
#define ATA_SERIAL_NUMBER_LENGTH 10
#define ATA_FIRMWARE_REVISION_WORD 23
#define ATA_FIRMWARE_REVISION_LENGTH 4

#define NVME_CONTROLLER_DEVICE_MODEL_WORD 12
#define NVME_CONTROLLER_DEVICE_MODEL_LENGTH 20
#define NVME_CONTROLLER_SERIAL_NUMBER_WORD 2
#define NVME_CONTROLLER_SERIAL_NUMBER_LENGTH 10
#define NVME_CONTROLLER_FIRMWARE_REVISION_WORD 32
#define NVME_CONTROLLER_FIRMWARE_REVISION_LENGTH 4
#define NVME_CONTROLLER_SANITIZE_CAPABILITIES_BYTE_INDEX 328
#define NVME_CONTROLLER_FORMAT_NVM_ATTRIBUTES_INDEX 524
#define NVME_NAMEPSACE_SIZE_BYTE_INDEX 0
#define NVME_NAMEPSACE_CAPACITY_BYTE_INDEX 8
#define NVME_NAMEPSACE_USE_BYTE_INDEX 16
#define NVME_FIRMWARE_SLOT_INFO_SLOT1_BYTE_INDEX 8
#define NVME_FIRMWARE_SLOT_INFO_SLOT2_BYTE_INDEX 16
#define NVME_FIRMWARE_SLOT_INFO_SLOT3_BYTE_INDEX 24
#define NVME_FIRMWARE_SLOT_INFO_SLOT4_BYTE_INDEX 32
#define NVME_FIRMWARE_SLOT_INFO_SLOT5_BYTE_INDEX 40
#define NVME_FIRMWARE_SLOT_INFO_SLOT6_BYTE_INDEX 48
#define NVME_FIRMWARE_SLOT_INFO_SLOT7_BYTE_INDEX 56

SKDeviceInfo* SKParser::parseAtaIdentifyBuffer(const U8 *buffer, const string &devicePath, const SKDeviceType &deviceType)
{
    SKBufferParser bp(buffer);

    SKDeviceInfo *deviceInfo = new SKDeviceInfo();
    deviceInfo->devicePath = devicePath;
    deviceInfo->deviceType = deviceType;
    deviceInfo->deviceModel = bp.getString(ATA_IDENTIFY_WORD, ATA_IDENTIFY_LENGTH);
    deviceInfo->serialNumber = bp.getString(ATA_SERIAL_NUMBER_WORD, ATA_SERIAL_NUMBER_LENGTH);
    deviceInfo->firmwareRevision = bp.getString(ATA_FIRMWARE_REVISION_WORD, ATA_FIRMWARE_REVISION_LENGTH);

    // User Capacity
    U32 usercap_low = 0, usercap_high = 0;
    if((bp.getWord(83) >> 10) & 0x1) {
        // Read Capacity (48b) (word 100 -> 103)
        usercap_low = U32((bp.getWord(101) << 16) | bp.getWord(100));
        usercap_high = U32((bp.getWord(103) << 16) | bp.getWord(102));
    }
    else
    {
        // Normal capacity info (word 60 -> 61)
        usercap_low = U32((bp.getWord(61) << 16) | bp.getWord(60));
    }
    deviceInfo->deviceSectorCount = (U64(usercap_high) << 32) | usercap_low;
    deviceInfo->userCapacity = 2048.0 * usercap_high + usercap_low / 2097152.0;

    // SMART
    deviceInfo->isSmartSupported = bp.getBit(82, 0);      // Word 82, bit 0
    deviceInfo->isSmartEnabled = bp.getBit(85, 0);        // Word 85, bit 0

    // SecureErase
    deviceInfo->specific.ata.secureErase.isSecuritySupported = bp.getBit(82, 1);              // Word 82, bit 1
    deviceInfo->specific.ata.secureErase.isUserPasswordPresent = bp.getBit(85, 1);            // Word 85, bit 1
    deviceInfo->specific.ata.secureErase.isSecurityFeatureSetSupported = bp.getBit(128, 0);   // Word 128, bit 0
    deviceInfo->specific.ata.secureErase.isSecurityEnabled = bp.getBit(128, 1);               // Word 128, bit 1
    deviceInfo->specific.ata.secureErase.isDeviceLocked = bp.getBit(128, 2);                  // Word 128, bit 2
    deviceInfo->specific.ata.secureErase.isDeviceFrozen = bp.getBit(128, 3);                  // Word 128, bit 3
    deviceInfo->specific.ata.secureErase.isPasswordAttemptExceeded = bp.getBit(128, 4);       // Word 128, bit 4
    deviceInfo->specific.ata.secureErase.isEnhancedSecurityEraseSupported = bp.getBit(128, 5);// Word 128, bit 5
    deviceInfo->specific.ata.secureErase.isMasterPasswordMaximum = bp.getBit(128, 8);         // Word 128, bit 8

    // Sanitize
    deviceInfo->specific.ata.sanitize.isSanitizeAntifreezeLock = bp.getBit(59, 10);     // Word 59, bit 10
    deviceInfo->specific.ata.sanitize.isSanitizeSupported = bp.getBit(59, 12);          // Word 59, bit 12
    deviceInfo->specific.ata.sanitize.isCryptoScrambleSupported = bp.getBit(59, 13);    // Word 59, bit 13
    deviceInfo->specific.ata.sanitize.isOverwriteSupported = bp.getBit(59, 14);         // Word 59, bit 14
    deviceInfo->specific.ata.sanitize.isBlockEraseSupported = bp.getBit(59, 15);        // Word 59, bit 15

    // Trusted Computing
    deviceInfo->specific.ata.isTrustedComputingFeatureSetSupported = bp.getBit(48, 0);  // Word 48, bit 0

    return deviceInfo;
}

SKDeviceInfo* SKParser::parseNvmeIdentifyBuffer(const U8 *controllerBuffer, const U8 *namespaceBuffer, const U8 *firmwareSlotInfoLogBuffer,
                                                const int &nsid, const std::string &devicePath, const SKDeviceType &deviceType)
{
    SKBufferParser cbp(controllerBuffer);
    SKDeviceInfo *deviceInfo = new SKDeviceInfo();
    deviceInfo->devicePath = devicePath;        
    deviceInfo->deviceType = deviceType;
    deviceInfo->deviceModel = cbp.getStringInLE(NVME_CONTROLLER_DEVICE_MODEL_WORD, NVME_CONTROLLER_DEVICE_MODEL_LENGTH);
    deviceInfo->serialNumber = cbp.getStringInLE(NVME_CONTROLLER_SERIAL_NUMBER_WORD, NVME_CONTROLLER_SERIAL_NUMBER_LENGTH);
    deviceInfo->firmwareRevision = cbp.getStringInLE(NVME_CONTROLLER_FIRMWARE_REVISION_WORD, NVME_CONTROLLER_FIRMWARE_REVISION_LENGTH);
    U32 sanitizeCapabilities = cbp.getDWord(NVME_CONTROLLER_SANITIZE_CAPABILITIES_BYTE_INDEX);
    deviceInfo->specific.nvme.sanitize.isSanitizeSupported = (0 < sanitizeCapabilities);
    deviceInfo->specific.nvme.sanitize.isCryptoEraseSupported = (0x0001 && sanitizeCapabilities);
    deviceInfo->specific.nvme.sanitize.isBlockEraseSupported = (0x0002 && sanitizeCapabilities) >> 1;
    deviceInfo->specific.nvme.sanitize.isOverwriteSupported = (0x0004 && sanitizeCapabilities) >> 2;
    U8 fna = cbp.getByte(NVME_CONTROLLER_FORMAT_NVM_ATTRIBUTES_INDEX);
    deviceInfo->specific.nvme.format.isCryptoEraseSupported = (0x04 && fna) >> 2;
    deviceInfo->specific.nvme.format.isCryptoEraseAppliesAllNamespaces = (0x02 && fna) >> 1;
    deviceInfo->specific.nvme.format.isFormatAppliesAllNamespaces = (0x01 && fna);
    SKBufferParser nbp(namespaceBuffer);
    deviceInfo->deviceSectorCount = nbp.getQWord(8); //byte [15:8]
    deviceInfo->specific.nvme.nsid = nsid;
    deviceInfo->specific.nvme.activeNamespace.size = nbp.getQWord(NVME_NAMEPSACE_SIZE_BYTE_INDEX) * 512;
    deviceInfo->specific.nvme.activeNamespace.capacity = nbp.getQWord(NVME_NAMEPSACE_CAPACITY_BYTE_INDEX) * 512;
    deviceInfo->specific.nvme.activeNamespace.use = nbp.getQWord(NVME_NAMEPSACE_USE_BYTE_INDEX) * 512;
    deviceInfo->userCapacity = (double)deviceInfo->specific.nvme.activeNamespace.size / (1024 * 1024 * 1024); // Returns in GB(s)
    SKBufferParser fsilb(firmwareSlotInfoLogBuffer);
    deviceInfo->specific.nvme.firmwareSlotInfo.activeSlot = (firmwareSlotInfoLogBuffer[0] & 0x07);          // Bytes 0: Bit 2:0
    deviceInfo->specific.nvme.firmwareSlotInfo.pendingActivateSlot = (firmwareSlotInfoLogBuffer[0] & 0x70); // Bytes 0: Bit 6:4
    deviceInfo->specific.nvme.firmwareSlotInfo.fwRevSlot[0] = fsilb.getQWord(NVME_FIRMWARE_SLOT_INFO_SLOT1_BYTE_INDEX);
    deviceInfo->specific.nvme.firmwareSlotInfo.fwRevSlot[1] = fsilb.getQWord(NVME_FIRMWARE_SLOT_INFO_SLOT2_BYTE_INDEX);
    deviceInfo->specific.nvme.firmwareSlotInfo.fwRevSlot[2] = fsilb.getQWord(NVME_FIRMWARE_SLOT_INFO_SLOT3_BYTE_INDEX);
    deviceInfo->specific.nvme.firmwareSlotInfo.fwRevSlot[3] = fsilb.getQWord(NVME_FIRMWARE_SLOT_INFO_SLOT4_BYTE_INDEX);
    deviceInfo->specific.nvme.firmwareSlotInfo.fwRevSlot[4] = fsilb.getQWord(NVME_FIRMWARE_SLOT_INFO_SLOT5_BYTE_INDEX);
    deviceInfo->specific.nvme.firmwareSlotInfo.fwRevSlot[5] = fsilb.getQWord(NVME_FIRMWARE_SLOT_INFO_SLOT6_BYTE_INDEX);
    deviceInfo->specific.nvme.firmwareSlotInfo.fwRevSlot[6] = fsilb.getQWord(NVME_FIRMWARE_SLOT_INFO_SLOT7_BYTE_INDEX);
    return deviceInfo;
}

SKSmartInfo* SKParser::parseAtaSmartBuffer(const U8 *buffer, const SKDeviceType &deviceType)
{
    //TODO
}

SKSmartInfo* SKParser::parseNvmeSmartBuffer(const U8 *buffer, const SKDeviceType &deviceType)
{
    SKSmartInfo *smartInfo = new SKSmartInfo();
    smartInfo->deviceType = deviceType;
    SKBufferParser sbp(buffer);    
    smartInfo->specific.nvme.criticalWarning = sbp.getByte(0);                                         //byte[0]
    smartInfo->specific.nvme.temperature = ((U16)(sbp.getByte(1)) | ((U16)(sbp.getByte(2)) << 8));     //byte[2:1]
    smartInfo->specific.nvme.availableSpare = sbp.getByte(3);                                          //byte[3];
    smartInfo->specific.nvme.availableSpareThreshold = sbp.getByte(4);                                 //byte[4]
    smartInfo->specific.nvme.percentageUsed = sbp.getByte(5);                                          //byte[5]
    smartInfo->specific.nvme.enduranceGroupSummary = sbp.getByte(6);                                   //byte[6]
    smartInfo->specific.nvme.lowDataUnitRead = sbp.getQWord(32);                                       //byte[47:32]
    smartInfo->specific.nvme.highDataUnitRead = sbp.getQWord(40);
    smartInfo->specific.nvme.lowDataUnitWritten = sbp.getQWord(48);                                      //byte[63:48]
    smartInfo->specific.nvme.highDataUnitWritten = sbp.getQWord(56);
    smartInfo->specific.nvme.lowHostReadCommands = sbp.getQWord(64);                                   //byte[79:64]
    smartInfo->specific.nvme.highHostReadCommands = sbp.getQWord(72);
    smartInfo->specific.nvme.lowHostWrittenCommands = sbp.getQWord(80);                                  //byte[95:80]
    smartInfo->specific.nvme.highHostWtittenCommands = sbp.getQWord(88);
    smartInfo->specific.nvme.lowControllerBusyTime = sbp.getQWord(96);                                 //byte[111:96]
    smartInfo->specific.nvme.highControllerBusyTime = sbp.getQWord(104);
    smartInfo->specific.nvme.lowPowerCycle = sbp.getQWord(112);                                        //byte[127:112]
    smartInfo->specific.nvme.highPowerCycle = sbp.getQWord(120);
    smartInfo->specific.nvme.lowPowerOnHours = sbp.getQWord(128);                                      //byte[143:128]
    smartInfo->specific.nvme.highPowerOnHours = sbp.getQWord(136);
    smartInfo->specific.nvme.lowUnsafeShutdowns = sbp.getQWord(144);                                   //byte[159:144]
    smartInfo->specific.nvme.highUnsafeShutdowns = sbp.getQWord(152);
    smartInfo->specific.nvme.lowMediaErrors = sbp.getQWord(160);                                       //byte[175:160]
    smartInfo->specific.nvme.highMediaErrors = sbp.getQWord(168);
    smartInfo->specific.nvme.lowErrorInfoLogEntryNum = sbp.getQWord(176);                              //byte[191:176]
    smartInfo->specific.nvme.highErrorInfoLogEntryNum = sbp.getQWord(184);
    smartInfo->specific.nvme.warningCompositeTemperatureTime = sbp.getDWord(192);                      //byte[195:192]
    smartInfo->specific.nvme.criticalCompositeTemperatureTime = sbp.getDWord(196);                     //byte[199:196]
    smartInfo->specific.nvme.temperatureSensor1 = sbp.getWord(100);                                    //byte[201:200]
    smartInfo->specific.nvme.temperatureSensor2 = sbp.getWord(101);                                    //byte[203:202]
    smartInfo->specific.nvme.temperatureSensor3 = sbp.getWord(102);                                    //byte[205:204]
    smartInfo->specific.nvme.temperatureSensor4 = sbp.getWord(103);                                    //byte[207:206]
    smartInfo->specific.nvme.temperatureSensor5 = sbp.getWord(104);                                    //byte[209:208]
    smartInfo->specific.nvme.temperatureSensor6 = sbp.getWord(105);                                    //byte[211:210]
    smartInfo->specific.nvme.temperatureSensor7 = sbp.getWord(106);                                    //byte[213:212]
    smartInfo->specific.nvme.temperatureSensor8 = sbp.getWord(107);                                    //byte[215:214]
    smartInfo->specific.nvme.thermalManagementTemperature1TransitionCount = sbp.getDWord(216);         //byte[219:216]
    smartInfo->specific.nvme.thermalManagementTemperature2TransitionCount = sbp.getDWord(220);         //byte[223:220]
    smartInfo->specific.nvme.totalTimeForThermalManagementTemperature1 = sbp.getDWord(224);            //byte[227:224]
    smartInfo->specific.nvme.totalTimeForThermalManagementTemperature2 = sbp.getDWord(228);            //byte[231:228]
    return smartInfo;
}
