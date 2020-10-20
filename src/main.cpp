#include <iostream>
#include <signal.h>
#include <string.h>
#include <vector>

#include "FirmwareUpdater.h"

int main(int argc, char** argv) {
    // takes in a path to a Hyperstone firmware archive and the path to the device to upgrade
    // TODO let user specific device path in the command line args
    const char* devPath = "/dev/sdb";
    std::string ddPath = "/home/bypie5/Documents/Virtium/hsfmt/bin/U9/dd.txt";

    updater::FirmwareUpdater* updater = new updater::FirmwareUpdater(devPath, ddPath);
    
    updater->inspectCurrentDevice();

    updater->update();

    // clean up
    delete updater;

    return 0;
}