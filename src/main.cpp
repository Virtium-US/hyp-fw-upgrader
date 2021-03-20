#include <iostream>
#include <signal.h>
#include <string.h>
#include <vector>

#include "FirmwareUpdater.h"

#define INPUT_ARGUMENT_COUNT     3
#define DEV_PATH_ARG             1
#define DD_PATH_ARG              2


int main(int argc, char** argv) {
    // takes in a path to a Hyperstone firmware archive and the path to the device to upgrade
    
    if (argc != INPUT_ARGUMENT_COUNT) {
        printf("ERROR - Invalid input parameters!\nUSAGE: hyp-fw-upgrader [device] [dd.txt path]\n\nEXAMPLES\n\n");
        printf("LINUX:\t\t hyp-fw-upgrader /dev/sdb /path_to_fw/dd.txt \n");
        printf("WINDOWS:\t hyp-fw-upgrader \\\\.\PhysicalDrive2 C:\\path_to_fw\\dd.txt \n");

        return 1;
    }

    const char* devPath = argv[DEV_PATH_ARG];
    std::string ddPath = argv[DD_PATH_ARG];

    try {
        printf("DEBUG:\tFile: main.cpp\tFunction: main\tPoint: #1\n");
        updater::FirmwareUpdater* updater = new updater::FirmwareUpdater(devPath, ddPath);
        
        printf("DEBUG:\tFile: main.cpp\tFunction: main\tPoint: #2\n");
        updater->inspectCurrentDevice();
        printf("DEBUG:\tFile: main.cpp\tFunction: main\tPoint: #3\n");
        updater->update();
        printf("DEBUG:\tFile: main.cpp\tFunction: main\tPoint: #4\n");

        // clean up
        delete updater;
    } catch (std::exception &e) {
        printf("Application encountered a runtime exception!\n'%s'\n", e.what());
        return 1;
    }

    return 0;
}