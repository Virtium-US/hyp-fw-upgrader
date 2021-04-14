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

    updater::FirmwareUpdater* updater = new updater::FirmwareUpdater(devPath, ddPath);
    try {
        std::cout << "=== Current Device Info ===" << std::endl;
        updater->inspectCurrentDevice();
        
        std::cout << "\n=== Starting Update ===" << std::endl;

        std::cout << "\nTransfering firmware files..." << std::endl;
        int exit = updater->update();
        if (exit == 0) {
            // "--update-customer-version-data"
            std::cout << "\nUpdating and printing customer specific version description data..." << std::endl;
            updater->updateCustomerVersionData();
        } else {
            // clean up
            delete updater;

            std::cout << "UPDATE FAILED!" << std::endl;
            return exit;
        }

        // clean up
        delete updater;
    } catch (std::exception &e) {
        printf("Application encountered a runtime exception!\n'%s'\n", e.what());
        // clean up
        delete updater;

        return 1;
    }

    std::cout << "UPDATE SUCCESSFUL" << std::endl;
    return 0;
}