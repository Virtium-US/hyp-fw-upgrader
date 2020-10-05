/**
*
* @section LICENSE
* Copyright 2018 Virtium Technology
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http ://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
#ifndef NvmeDevice_h
#define NvmeDevice_h

#include "StorageKitStorageDevice.h"

/**
 * @brief Declares nvme sanitize action.
 */
enum class SKNvmeSanitizeAction
{
    /** Reserved */
    SKReserved1 = 0,
    /** Exit failure mode */
    SKExitFailureMode = 1,
    /** Exit failure mode */
    SKBlockErase = 2,
    /** Exit failure mode */
    SKOverwrite = 3,
    /** Exit failure mode */
    SKCryptoErase = 4,
    /** Reserved */
    SKReserved2,
};

/**
 * @brief Declares nvme sanitize allow unrestricted sanitize exit.
 */
enum class SKNvmeSanitizeAUSE
{
    /** Not allow */
    SKNo = 0,
    /** Allow */
    SKYes = 1
};

/**
 * @brief Declares nvme sanitize overwrite pass count.
 */
enum class SKNvmeSanitizeOWPASS
{
    /** 0 Times */
    SK0Times = 0,
    /** 1 Times */
    SK1Times = 1,
    /** 2 Times */
    SK2Times = 2,
    /** 3 Times */
    SK3Times = 3,
    /** 4 Times */
    SK4Times = 4,
    /** 5 Times */
    SK5Times = 5,
    /** 6 Times */
    SK6Times = 6,
    /** 7 Times */
    SK7Times = 7,
    /** 8 Times */
    SK8Times = 8,
    /** 9 Times */
    SK9Times = 9,
    /** 10 Times */
    SK10Times = 10,
    /** 11 Times */
    SK11Times = 11,
    /** 12 Times */
    SK12Times = 12,
    /** 13 Times */
    SK13Times = 13,
    /** 14 Times */
    SK14Times = 14,
    /** 15 Times */
    SK15Times = 15,
};

/**
 * @brief Declares nvme sanitize overwrite invert pattern between passes.
 */
enum class SKNvmeSanitizeOIPBP
{
    /** Not inverted */
    SKNotInverted = 0,
    /** Inverted */
    SKInverted = 1
};

/**
 * @brief Declares nvme sanitize no deallocate after sanitize.
 */
enum class SKNvmeSanitizeNoDeallocate
{
    /** Deallocate */
    SKNo = 0,
    /** No deallocate */
    SKYes = 1
};

/**
 * @brief Declares nvme format LBA format.
 */
enum class SKNvmeFormatLBAF
{
    /** LBA Format 0 */
    SKLBAF0 = 0,
    /** LBA Format 1 */
    SKLBAF1 = 1,
    /** LBA Format 2 */
    SKLBAF2 = 2,
    /** LBA Format 3 */
    SKLBAF3 = 3,
    /** LBA Format 4 */
    SKLBAF4 = 4,
    /** LBA Format 5 */
    SKLBAF5 = 5,
    /** LBA Format 6 */
    SKLBAF6 = 6,
    /** LBA Format 7 */
    SKLBAF7 = 7,
    /** LBA Format 8 */
    SKLBAF8 = 8,
    /** LBA Format 9 */
    SKLBAF9 = 9,
    /** LBA Format 10 */
    SKLBAF10 = 10,
    /** LBA Format 11 */
    SKLBAF11 = 11,
    /** LBA Format 12 */
    SKLBAF12 = 12,
    /** LBA Format 13 */
    SKLBAF13 = 13,
    /** LBA Format 14 */
    SKLBAF14 = 14,
    /** LBA Format 15 */
    SKLBAF15 = 15
};

/**
 * @brief Declares nvme format secure erase.
 */
enum class SKNvmeFormatSES
{
    /** Default: No secure erase operation requested */
    SKDefault = 0,
    /** User-data erase */
    SKUserDataErase = 1,
    /** Cryptographic erase */
    SKCryptographicErase = 2
};

/**
 * @brief Declares nvme format location of protection information.
 */
enum class SKNvmeFormatPIL
{
    /** End: protection information is transferred as the last eight bytes of metadata */
    SKEnd = 0,
    /** Start: protection information is transferred as the first eight bytes of metadata */
    SKStart = 1
};

/**
 * @brief Declares nvme format protection information.
 */
enum class SKNvmeFormatPI
{
    /** Off */
    SKOff = 0,
    /** Type 1 on */
    SKType1On = 1,
    /** Type 2 on */
    SKType2On = 2,
    /** Type 3 on */
    SKType3On = 3
};

/**
 * @brief Declares nvme format extended format.
 */
enum class SKNvmeFormatMS
{
    /** Off (metadata in sparate buffer) */
    SKOff = 0,
    /** On (extended LBA used) */
    SKOn = 1
};

/**
 * @brief Declares nvme firmware slot.
 */
enum class SKNvmeFirmwareSlot
{
    /** Slot 1 */
    SKSlot1 = 1,
    /** Slot 2 */
    SKSlot2 = 2,
    /** Slot 3 */
    SKSlot3 = 3,
    /** Slot 4 */
    SKSlot4 = 4,
    /** Slot 5 */
    SKSlot5 = 5,
    /** Slot 6 */
    SKSlot6 = 6,
    /** Slot 7 */
    SKSlot7 = 7,

    /** Invalid slot */
    SKInvalidSlot = 0xFF
};

/**
 * @brief Declares nvme firmware action.
 */
enum class SKNvmeFirmwareAction
{
    /** Replace firmware without activating */
    SkReplaceFirmwareWithoutActivation = 0,
    /** Replace firmware with activation */
    SkReplaceFirmwareWithActivation = 1,
    /** Replace firmware with activation at next reset */
    SkReplaceFirmwareWithActivationAtNextRelease = 2
};

/**
 * @brief Declares nvme firmware boot partition identifier.
 */
enum class SKNvmeFirmwareBpid
{
    /** Identifier 0 */
    SKId0 = 0,
    /** Identifier 1 */
    SKId1 = 1,
};

class SKNvmeProtocol;

/**
*
* @section DESCRIPTION
*
* An object is responsible to issue NVME commands.
* <br>such as identify controller, identify namespace , firmware download, firmware commit, ...
*
*/
class STORAGEAPI_DLLSPEC SKNvmeDevice : public SKStorageDevice
{
  public:
    static const U32 IDENTIFY_BUFFER_SIZE_IN_SECTORS;

  private:
    /**
    * @brief A SKNvmeProtocol pointer to store the NVME protocol.
    */
    SKNvmeProtocol *protocol;

    /**
    * @brief A int variable to store the namespace ID.
    */
    int nsid;

  public:
    /**
    * @brief Creates an SKNvmeDevice object with a device type and a device handle.
    *
    * @param devicePath A string to specify path of the device.
    * @param deviceHandle A DeviceHandle object to specify handle of the device.
    * @param nsid A int variable to specify namespace ID.
    */
    SKNvmeDevice(const std::string &devicePath, const DeviceHandle &deviceHandle, const int &nsid);

    /**
    * @brief Destructor that frees up memory.
    */
    virtual ~SKNvmeDevice() override;

  public:
    virtual SKDeviceSpaceInfo* queryDeviceSpaceInfo() override;
    virtual SKReturnCode queryBootableStatus(bool &isBootable) override;

  public:
    /**
    * @brief Retrieve namespace ID.
    *
    * @return A U32 variable to specify namespace ID.
    */
    int getNsid();

    /**
    * @brief Issues identify controller to retrieve raw controller data structure info on the aligned buffer.
    *
    * @param[out] buffer A SKAlignedBuffer object to store raw information of the identifying controller.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode identifyController(SKAlignedBuffer *buffer);

    /**
    * @brief Issues identify namespace to retrieve raw namespace data structure info on the aligned buffer.
    *
    * @param[out] buffer A SKAlignedBuffer object to store raw information of the identifying controller.
    * @param present A bool variable to indicate present status.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode identifyNamespace(SKAlignedBuffer *buffer, bool present = 0);

    /**
    * @brief Issues get firmware slot information log.
    *
    * @param[out] buffer A SKAlignedBuffer object to store raw information of the firmware slot log.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode getFirmwareSlotInformationLog(SKAlignedBuffer *buffer);

    /**
    * @brief Issues get sanitize information log.
    *
    * @param[out] buffer A SKAlignedBuffer object to store raw information of the sanitize log.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode getSanitizeInformationLog(SKAlignedBuffer *buffer);

    /**
    * @brief Issues download a firmware image to the controller.
    *
    * @param offset An U32 variable to indicate each portion of the firmware image being downloaded.
    * @param buffer An SKAlignedBuffer object to store binary data update firmware.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode firmwareDownload(const U32 &offset, SKAlignedBuffer *buffer);

    /**
    * @brief Issues submit a firmware image download to download the firmware image to the controller.
    * <br>Firmware commit command verifies that the last firmware image downloaded is valid and commits
    * <br>that image to the firmware slot indicated for future use.
    *
    * @param slot A SKNvmeFirmwareSlot object to indicate firmware slot to active.
    * @param action A SKNvmeFirmwareAction object to indicate activation action.
    * @param bpid A SKNvmeFirmwareBpid object to indicate boot partition identifier.
    *
    * @return A SKReturnCode object to specify the return code.
    *
    * @note Device is not automatically reset following firmware activation.
    */
    SKReturnCode firmwareCommit(const SKNvmeFirmwareSlot &slot, const SKNvmeFirmwareAction &action, const SKNvmeFirmwareBpid &bpid);

    /**
    * @brief Issues submit a sanitize operation or recover from a previous failed sanitize operation.
    * <br>The sanitize operation types that may be supported are Block Erases, Cryto Erase, and Overwrite.
    * <br>All sanitize operations are processed in the background.
    *
    * @param action A SKNvmeSanitizeAction object to indicate sanitize action.
    * @param ause A SKNvmeSanitizeAUSE object to indicate allow unrestricted sanitize exit.
    * @param owpass A SKNvmeSanitizeOWPASS object to indicate overwrite pass count.
    * @param oipbp A SKNvmeSanitizeOIPBP object to indicate overwrite invert pattern between passes.
    * @param noDealloc A SKNvmeSanitizeNoDeallocate object to indicate no deallocate after sanitize.
    * @param overwritePattern A U32 object to indicate overwrite pattern.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode sanitize(const SKNvmeSanitizeAction &action, const SKNvmeSanitizeAUSE &ause, const SKNvmeSanitizeOWPASS &owpass,
                          const SKNvmeSanitizeOIPBP &oipbp, const SKNvmeSanitizeNoDeallocate &noDealloc, const U32 &overwritePattern);

    /**
    * @brief Issues submit a re-format command which can following supported:
    * <br>Erase all data in namespace (user data erase) or delete data encryption key if specified.
    * <br>Change LBAF to change namespace reported physical block format.
    *
    * @param lbaf A SKNvmeFormatLBAF object to indicate LBA format to apply.
    * @param ses A SKNvmeFormatSES object to indicate secure erase.
    * @param pi A SKNvmeFormatPI object to indicate protection info off/Type 1/Type 2/Type 3.
    * @param pil A SKNvmeFormatPIL object to indicate protection info location last/first 8 bytes of metadata.
    * @param ms A SKNvmeFormatMS object to indicate extended format off/on.
    * @param timeoutInMs A U32 object to indicate timeout value, in milliseconds.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode format(const SKNvmeFormatLBAF &lbaf, const SKNvmeFormatSES &ses, const SKNvmeFormatPI &pi,
                        const SKNvmeFormatPIL &pil, const SKNvmeFormatMS &ms, const U32 &timeoutInMs);

    /**
    * @brief  Issues get SMART to retrieve SMART raw data structure info on the alighed buffer
    *
    * @param[out] buffer A SKAlignedBuffer object to store raw information of the SMART.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode getSmart(SKAlignedBuffer *buffer);

    /**
    * @brief Writes sector to device at given lba and given sector count with an aligned buffer.
    *
    * @param lba An U64 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param buffer A SKAlignedBuffer object to specify data buffer write to device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode write(const U64 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Writes sector extended to device at given lba and given sector count with an aligned buffer.
    *
    * @param lba An U64 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param buffer A SKAlignedBuffer object to specify data buffer write to device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode writeExt(const U64 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Reads sector from device at given lba and given sector count to an aligned buffer.
    *
    * @param lba An U64 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param[out] buffer A SKAlignedBuffer object to specify data buffer read from device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode read(const U64 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Reads sector extended from device at given lba and given sector count to an aligned buffer.
    *
    * @param lba An U64 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param[out] buffer A SKAlignedBuffer object to specify data buffer read from device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode readExt(const U64 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer);

  protected:
    /**
    * @brief Creates protocol with a device path and a device handle.
    *
    * @param devicePath A string to specify path of the device.
    * @param deviceHandle A DeviceHandle to specify handle of the device.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    void createProtocol(const std::string &devicePath, const DeviceHandle &deviceHandle);
    virtual SKReturnCode issueCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer) override;
};

#endif
