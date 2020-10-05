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
#ifndef ScsiDevice_h
#define ScsiDevice_h

#include "StorageKitStorageDevice.h"

class SKScsiProtocol;

/**
*
* @section DESCRIPTION
*
* An object is responsible to issue SCSI commands such as inquiry, write10, read10, and read capacity.
*
*/
class STORAGEAPI_DLLSPEC SKScsiDevice : public SKStorageDevice
{
  protected:
    /**
    * @brief A SKScsiProtocol pointer to store the SCSI protocol.
    */
    SKScsiProtocol *protocol;

  public:
    /**
    * @brief Creates an SKScsiDevice object with a device path and a device handle.
    *
    * @param devicePath A string to specify path of the device.
    * @param deviceHandle A DeviceHandle object to specify handle of the device.
    */
    SKScsiDevice(const std::string &devicePath, const DeviceHandle &deviceHandle);

    /**
    * @brief Destructor that frees up memory.
    */
    virtual ~SKScsiDevice() override;

  public:
    virtual SKDeviceSpaceInfo* queryDeviceSpaceInfo() override;
    virtual SKReturnCode queryBootableStatus(bool &isBootable) override;

  public:
    /**
    * @brief Issues SCSI inquiry to retrieve raw device info on the aligned buffer.
    *
    * @param pageCode A SKInquiryPageCode object to declare inquiry page code.
    * @param[out] buffer A SKAlignedBuffer pointer to stored raw information of the inquiry device.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode scsiInquiry(const SKInquiryPageCode &pageCode, SKAlignedBuffer *buffer);

    /**
    * @brief Issues SCSI write6 to device at given lba and given sector count with an aligned buffer.
    *
    * @param lba An U32 variable to specify a logical block address. Max lba for this command is 0x1FFFF.
    * @param sectorCount An U16 variable to specify a sector count. Max sector count for this command is 256.
    * @param buffer A SKAlignedBuffer pointer to specify data buffer write to device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode scsiWrite6(const U32 &lba, const U16 &sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Issues SCSI write10 to device at given lba and given sector count with an aligned buffer.
    *
    * @param lba An U32 variable to specify a logical block address.
    * @param sectorCount An U16 variable to specify a sector count.
    * @param buffer A SKAlignedBuffer pointer to specify data buffer write to device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode scsiWrite10(const U32 &lba, const U16 &sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Issues SCSI write12 to device at given lba and given sector count with an aligned buffer.
    *
    * @param lba An U32 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param buffer A SKAlignedBuffer pointer to specify data buffer write to device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode scsiWrite12(const U32 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Issues SCSI write16 to device at given lba and given sector count with an aligned buffer.
    *
    * @param lba An U64 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param buffer A SKAlignedBuffer pointer to specify data buffer write to device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode scsiWrite16(const U64 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Issues SCSI read6 to device at given lba and given sector count with an aligned buffer.
    *
    * @param lba An U32 variable to specify a logical block address. Max lba for this command is 0x1FFFF.
    * @param sectorCount An U16 variable to specify a sector count. Max sector count for this command is 256.
    * @param buffer A SKAlignedBuffer pointer to specify data buffer read from device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode scsiRead6(const U32 &lba, const U16 &sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Issues SCSI read10 to device at given lba and given sector count with an aligned buffer.
    *
    * @param lba An U32 variable to specify a logical block address.
    * @param sectorCount An U16 variable to specify a sector count.
    * @param buffer A SKAlignedBuffer pointer to specify data buffer read from device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode scsiRead10(const U32 &lba, const U16 &sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Issues SCSI read12 to device at given lba and given sector count with an aligned buffer.
    *
    * @param lba An U32 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param buffer A SKAlignedBuffer pointer to specify data buffer read from device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode scsiRead12(const U32 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Issues SCSI read16 to device at given lba and given sector count with an aligned buffer.
    *
    * @param lba An U64 variable to specify a logical block address.
    * @param sectorCount An U32 variable to specify a sector count.
    * @param buffer A SKAlignedBuffer pointer to specify data buffer read from device via lba and sector count.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode scsiRead16(const U64 &lba, const U32 &sectorCount, SKAlignedBuffer *buffer);

    /**
    * @brief Issues SCSI read capacity from device to an aligned buffer.
    *
    * @param[out] buffer A SKAlignedBuffer pointer to specify data buffer read from device via lba and sector count.
    *
    * @return A SKReturnCode pointer to specify the return code.
    */
    SKReturnCode scsiReadCapacity(SKAlignedBuffer *buffer);

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

  private:
    /**
    * @brief Handles SCSI command with a command desc and an aligned buffer.
    *
    * @param cmdDesc A SKCommandDesc pointer to specify a command descriptor.
    * @param buffer A SKAlignedBuffer pointer to specify a data buffer.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode handleScsiCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer);
};

#endif
