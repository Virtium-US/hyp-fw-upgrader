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
#ifndef DeviceManager_h
#define DeviceManager_h

#include <vector>

#include "StorageKitStorageDevice.h"

/**
*
* @section DESCRIPTION
*
* An object is responsible to perform one of following actions:
* \arg \c Scans all of current devices.
* \arg \c Scans a device with a specified device path.
*
*/
class STORAGEAPI_DLLSPEC SKStorageDeviceUtils
{
  public:
    /**
    * @brief Scans a device with a specified device path.
    *
    * @param devicePath A string to specify device path.
    *
    * @return A SKStorageDevice pointer.
    */
    static SKStorageDevice* scanDevice(const std::string &devicePath);

    /**
    * @brief Scans all of current storage device.
    *
    * @param[out] storageDevices A vector of SKStorageDevice to stored list of storage devices.
    */
    static void scanDevices(std::vector<SKStorageDevice*> &storageDevices);
};

#endif
