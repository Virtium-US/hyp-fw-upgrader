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
#ifndef HypDevice_h
#define HypDevice_h

#include "StorageKitScsiDevice.h"

class SKScsiProtocol;

/**
*
* @section DESCRIPTION
*
* An object is responsible to issue ATA identify device and SCSI commands
* <br>such as inquiry, write10, read10, and read capacity.
*
*/
class STORAGEAPI_DLLSPEC SKHypDevice : public SKScsiDevice
{
  public:
    /**
    * @brief Creates an SKHypDevice object with a device path and a device handle.
    *
    * @param devicePath is a string to specify path of the device.
    * @param deviceHandle is a DeviceHandle object to specify handle of the device.
    */
    SKHypDevice(const std::string &devicePath, const DeviceHandle &deviceHandle);

  public:
    /**
    * @brief Issues ATA identify device to retrieve raw device info on the aligned buffer.
    *
    * @param[out] buffer A SKAlignedBuffer pointer to store raw information of the identifying device.
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
    * @brief Issues SCSI trim.
    *
    * @param buffer A SKAlignedBuffer pointer to specify trim buffer (range start LBA, range LBA count).
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode scsiTrim(U64 startLba, U64 count);

  protected:
    virtual SKReturnCode issueCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer) override;

  private:
    /**
    * @brief Handles ATA command with a command desc and an aligned buffer.
    *
    * @param cmdDesc A SKCommandDesc pointer to specify a command descriptor.
    * @param buffer A SKAlignedBuffer pointer to specify a data buffer.
    *
    * @return A SKReturnCode object to specify the return code.
    */
    SKReturnCode handleAtaCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer);

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
