#include <iostream>
#include <signal.h>
#include <string.h>
#include <vector>

#include "FirmwareUpdater.h"

using namespace updater;

int main(int argc, char** argv) {
    // TODO let user specific device path in the command line args
    const char* devPath = "/dev/sdb";
    const char* ddPath = "/home/bypie5/Documents/Virtium/hsfmt/U9/dd.txt";

    FirmwareUpdater* updater = new FirmwareUpdater(devPath);
    
    FWVersionInfo_t info = updater->readFirmwareVersion();
    printf("current fw: %c%c%c%c%c%c\n", info.fwVersionDate[0], info.fwVersionDate[1], info.fwVersionDate[2], info.fwVersionDate[3], info.fwVersionDate[4], info.fwVersionDate[5]);
    printf("controller revision: %c%c\n", info.controllerRevisionIdString[0], info.controllerRevisionIdString[1]);

    try {
        DeviceDescriptionData_t dd = updater->loadDDData(ddPath);
        printf("general fw features: %s\n", dd.generalFwFeatures);
        printf("driver strengths: %s\n", dd.drvStrengths);

        // get dd.txt entry
        TargetInfo_t targetInfo = updater->readTargetInfo();        
        DeviceDescriptionEntry_t dde = updater->findDDEntry(targetInfo, ddPath);
        printf("flash device id: %s\n", dde.flashDeviceId);
        printf("specific fw features: %s\n", dde.specificFwFeatures);
        printf("firmware file name: %s\n", dde.firmwareFileName);
        printf("achor file name: %s\n", dde.anchorFileName);
    } catch(std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    // clean up
    delete updater;

    return 0;
}