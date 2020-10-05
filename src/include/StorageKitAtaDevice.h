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
#ifndef AtaDevice_h
#define AtaDevice_h

#include <vector>

#include "StorageKitStorageDevice.h"

class SKAtaProtocol;

/**
*
* @section DESCRIPTION
*
* An object is responsible to issue ATA commands and extended ATA commands.
* \arg \c ATA commands: identify, read sector, write sector, ...
* \arg \c Extended ATA commands: read sector extend, write sector extend, ...
* \arg \c ATA security commands: unlock, freeze lock, sanitize block erase, sanitize status extend, ...
*
*/
class STORAGEAPI_DLLSPEC SKAtaDevice : public SKStorageDevice
{
  private:
    /**
    * @brief An SKAtaProtocol pointer to store the ATA protocol.
    */
    SKAtaProtocol *protocol;

  public:
    /**
    * @brief Creates an SKAtaDevice object with a device type and a device handle.
    *
    * @param devicePath A string to specify path of the device.
    * @param deviceHandle A DeviceHandle object to specify handle of the device.
    */
    SKAtaDevice(const std::string &devicePath, const DeviceHandle &deviceHandle);

    /**
    * @brief Destructor that frees up memory.
    */
    virtual ~SKAtaDevice() override;

  protected:
    SKAtaDevice(const SKDeviceType &type, const std::string &devicePath);

  public:
    virtual SKDeviceSpaceInfo* queryDeviceSpaceInfo() override;
    virtual SKReturnCode queryBootableStatus(bool &isBootable) override;

  public:
    /**
    * @brief Gets the last output task file register of current device.
    *
    * @return An SKAtaOutputTaskFileRegister object to specify the ATA output task file register.
    */
    virtual SKAtaOutputTaskFileRegister getLastOutputTaskFileRegister() const;

  public:
    /**
    * @brief Issues identify device to retrieve raw device info on the aligned buffer.
    *
    * @param[out] buffer A SKAlignedBuffer object to store raw information of the identifying device.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataIdentifyDevice(SKAlignedBuffer *buffer);

    /**
    * @brief Issues read smart to retrieve raw smart info on the aligned buffer.
    *
    * @param[out] buffer A SKAlignedBuffer object to stored raw information of the smart.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataReadSmart(SKAlignedBuffer *buffer);

    /**
    * @brief Issues read smart threshold to retrieve raw smart threshold on the aligned buffer.
    *
    * @param[out] buffer A SKAlignedBuffer object to stored raw information of the smart threshold.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataReadSmartThreshold(SKAlignedBuffer *buffer);

    /**
    * @brief Reads sector from device at given lba and given sector count to an aligned buffer.
    *
    * @param lba An U64 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param[out] buffer A SKAlignedBuffer object to specify data buffer read from device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataReadSector(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Reads sector extended from device at given lba and given sector count to an aligned buffer.
    *
    * @param lba An U64 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param[out] buffer A SKAlignedBuffer object to specify data buffer read from device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataReadSectorExt(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Reads DMA from device at given lba and given sector count to an aligned buffer.
    *
    * @param lba An U64 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param[out] buffer A SKAlignedBuffer object to specify data buffer read from device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataReadDma(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Reads DMA extended from device at given lba and given sector count to an aligned buffer.
    *
    * @param lba An U64 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param[out] buffer A SKAlignedBuffer object to specify data buffer read from device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataReadDmaExt(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Writes sector to device at given lba and given sector count with an aligned buffer.
    *
    * @param lba An U64 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param buffer A SKAlignedBuffer object to specify data buffer write to device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataWriteSector(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Writes sector extended to device at given lba and given sector count with an aligned buffer.
    *
    * @param lba An U64 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param buffer A SKAlignedBuffer object to specify data buffer write to device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataWriteSectorExt(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Writes DMA to device at given lba and given sector count with an aligned buffer.
    *
    * @param lba An U64 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param buffer A SKAlignedBuffer object to specify data buffer write to device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataWriteDma(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Writes DMA extended to device at given lba and given sector count with an aligned buffer.
    *
    * @param lba An U64 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param buffer A SKAlignedBuffer object to specify data buffer write to device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataWriteDmaExt(U64 lba, U32 sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Issues security erase prepare.
    *
    * @return A SKReturnCode object to specify the return code.
    *
    * @note This command shall be issued immediately before the #ataSecurityEraseUnit command.
    */
    SKReturnCode ataSecurityErasePrepare();

    /**
    * @brief Issues security erase unit.
    *
    * @param buffer A SKAlignedBuffer object to specify data buffer of Ata Security Erase Unit.
    *
    * @return A SKReturnCode object to specify the return code.
    *
    * @note This command must be issued after the #ataSecurityErasePrepare command.
    */
    SKReturnCode ataSecurityEraseUnit(SKAlignedBuffer *buffer);

    /**
    * @brief Issues security disable password.
    *
    * @details If the password matches the password previously saved by the device, then the device shall disable the user password,
    * <br>and return the device to Security Disabled/Not Locked/Not Frozen state.
    * <br>This command shall not change the master password or master password identifier.
    *
    * @param buffer A SKAlignedBuffer object to specify data buffer of Ata Security Disable Password.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataSecurityDisablePassword(SKAlignedBuffer *buffer);

    /**
    * @brief Issues security set user password or master password with a security level (MAX or HIGH).
    *
    * @param buffer A SKAlignedBuffer object to specify data buffer of Ata Security Set Password.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataSecuritySetPassword(SKAlignedBuffer *buffer);

    /**
    * @brief Issues security unlock user password or master password.
    *
    * @param buffer A SKAlignedBuffer object to specify data buffer of Ata Security Unlock.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataSecurityUnlock(SKAlignedBuffer *buffer);

    /**
    * @brief Issues security freeze lock to set the device to frozen mode.
    *
    * @return A SKReturnCode object to specify the return code.
    *
    * @note Frozen mode shall be disabled by power-off or hardware reset.
    */
    SKReturnCode ataSecurityFreezeLock();

    /**
    * @brief Issues sanitize crypto scramble extended.
    *
    * @details The command starts a crypto scramble sanitize operation that changes the internal encryption keys that are used for user data
    * <br> causing the user data to become unretrievable.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataSanitizeCryptoScrambleExt();

    /**
    * @brief Issues sanitize overwrite extended.
    *
    * @details The command starts an overwrite sanitize operation to overwrite the internal media with a constant value
    * <br>that fills physical sectors within the Sanitize operation scope with a four byte pattern specified
    * <br>by the OVERWRITE PATTERN field of the command.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataSanitizeOverwriteExt();

    /**
    * @brief Issues sanitize block erase extended.
    *
    * @details The command starts a block erase sanitize operation that uses the block erase method on the user data areas,
    * <br>including user data areas that are not currently allocated to cause the user data to become unretrievable.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataSanitizeBlockEraseExt();

    /**
    * @brief Issues sanitize status extended. Returns the information about current or previously completed sanitize operations.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataSanitizeStatusExt();

    /**
    * @brief Issues command causes the device to enter sleep mode.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataSleep();

    /**
    * @brief Issues trusted send by security protocol and security comID with aligned buffer.
    *
    * @param protocol An U8 variable to specify security protocol.
    * @param comID An U16 variable to specify security comID.
    * @param buffer A SKAlignedBuffer object to specify data buffer to send.
    *
    * @details The command sends one or more security protocol specific instructions to be processed by the device.
    * <br>The host uses TRUSTED RECEIVE commands to retrieve any data resulting from these instructions.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataTrustedSend(U8 protocol, U16 comID, U32 sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Issues trusted receive by security protocol and security comID with aligned buffer.
    *
    * @param protocol An U8 variable to specify security protocol.
    * @param comID An U16 variable to specify security comID.
    * @param[out] buffer A SKAlignedBuffer object to specify data buffer to receive.
    *
    * @details The command retrieves security protocol information or the results from one or more TRUSTED SEND commands.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode ataTrustedReceive(U8 protocol, U16 comID, U32 sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Sets the quick erase method and its parameter.
    *
    * @param[out] buffer A SKAlignedBuffer object to contain retrieved previously set erase method and parameter.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode getQuickEraseMethod(SKAlignedBuffer *buffer);

    /**
    * @brief Retrieves the previously set erase method and its parameter.
    *
    * @param buffer A SKAlignedBuffer object to contain set erase method and parameter.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode setQuickEraseMethod(SKAlignedBuffer *buffer);

  protected:
    virtual SKReturnCode issueCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer) override;

  private:
    void createProtocol(const std::string &devicePath, const DeviceHandle &deviceHandle);
};

#endif
