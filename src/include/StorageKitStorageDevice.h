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
#ifndef StorageDevice_h
#define StorageDevice_h

#include <string>

#include "StorageKitAlignedBuffer.h"
#include "defines.h"

class SKCommandDesc;

/**
*
* @section DESCRIPTION
*
* An object is responsible to issue commands via SKCommandDesc (declared command description) with SKAlignedBuffer.
* And be able to query space info, bootable status, and also get device path and device type.
*
*/
class STORAGEAPI_DLLSPEC SKStorageDevice
{
  protected:
    /**
    * @brief A SKDeviceType object to store the device type.
    */
    SKDeviceType deviceType;

    /**
    * @brief A string object to store the device path.
    */
    std::string devicePath;

  public:
    /**
    * @brief Destructor that frees up memory.
    */
    virtual ~SKStorageDevice();

  protected:
    /**
    * @brief Creates an SKStorageDevice object with a device type and a device path.
    *
    * @param type A SKDeviceType object to specify the type of the device.
    * @param devicePath A string to specify path of the device.
    */
    SKStorageDevice(const SKDeviceType &type, const std::string &devicePath);

    /**
    * @brief Issues command with a command desc and an aligned buffer.
    *
    * @param cmdDesc A SKCommandDesc object to describe command in details.
    * @param buffer A SKAlignedBuffer pointer uses to read or write commands.
    *
    * @return A SKReturnCode object to specify return code when issuing command done.
    */
    virtual SKReturnCode issueCommand(const SKCommandDesc *cmdDesc, SKAlignedBuffer *buffer);

  public:
    /**
    * @brief Queries the space information of the device.
    *
    * @return A SKDeviceSpaceInfo pointer to specify information (total, used, free) of space size.
    */
    virtual SKDeviceSpaceInfo* queryDeviceSpaceInfo();

    /**
    * @brief Queries the bootable status of the device.
    *
    * @param[out] isBootable A bool variable to determined if the current device is bootable or not.
    *
    * @return A SKReturnCode object to specify return code.
    */
    virtual SKReturnCode queryBootableStatus(bool &isBootable);

  public:
    /**
    * @brief Gets the type of current device.
    *
    * @return A SKDeviceType object to specify the device type.
    */
    SKDeviceType getDeviceType() const;

    /**
    * @brief Gets the path of current device.
    *
    * @return A string to specify the device path.
    */
    std::string getDevicePath() const;
};

#endif
