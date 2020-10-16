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

    FirmwareUpdater* updater = new FirmwareUpdater(devPath, ddPath);
    
    updater->inspectCurrentDevice();

    // clean up
    delete updater;

    return 0;
}