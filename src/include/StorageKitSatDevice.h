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
#ifndef SatDevice_h
#define SatDevice_h

#include <vector>

#include "StorageKitAtaDevice.h"

class SKScsiProtocol;

/**
*
* @section DESCRIPTION
*
* An object is responsible to issue SAT commands and extended ATA commands.
* \arg \c ATA commands: identify, read sector, write sector, ...
* \arg \c Extended ATA commands: read sector extend, write sector extend, ...
* \arg \c ATA security commands: unlock, freeze lock, sanitize block erase, sanitize status extend, ...
*
*/
class STORAGEAPI_DLLSPEC SKSatDevice : public SKAtaDevice
{
  private:
    /**
    * @brief A SKScsiProtocol pointer to store the SCSI protocol.
    */
    SKScsiProtocol *scsiProtocol;

  public:
    /**
    * @brief Creates an SKSatDevice object with a device type and a device handle.
    *
    * @param devicePath is a string to specify path of the device.
    * @param deviceHandle is a DeviceHandle object to specify handle of the device.
    */
    SKSatDevice(const std::string &devicePath, const DeviceHandle &deviceHandle);

    /**
    * @brief Destructor that frees up memory.
    */
    virtual ~SKSatDevice() override;
    
  public:
    /**
    * @brief Gets the last output task file register of current device.
    *
    * @return An SKAtaOutputTaskFileRegister object to specify the ATA output task file register.
    */
    virtual SKAtaOutputTaskFileRegister getLastOutputTaskFileRegister() const override;

  public:
    virtual SKDeviceSpaceInfo* queryDeviceSpaceInfo() override;
    virtual SKReturnCode queryBootableStatus(bool &isBootable) override;

  protected:
    virtual SKReturnCode issueCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer) override;

  private:
    SKReturnCode handleAtaCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer);
    void createProtocol(const std::string &devicePath, const DeviceHandle &deviceHandle);
};

#endif
