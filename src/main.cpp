#include <iostream>
#include <signal.h>
#include <string.h>
#include <vector>

#include "FirmwareUpdater.h"

int main(int argc, char** argv) {
    // TODO let user specific device path in the command line args
    FirmwareUpdater* updater = new FirmwareUpdater("/dev/sdb");
    FirmwareVersionInfo info = updater->readFirmwareVersion();

    printf("%c%c%c%c%c%c\n", info.fwVersionDate[0], info.fwVersionDate[1], info.fwVersionDate[2], info.fwVersionDate[3], info.fwVersionDate[4], info.fwVersionDate[5]);

    printf("%s\n", info.controllerRevisionIdString);

    delete updater;
}