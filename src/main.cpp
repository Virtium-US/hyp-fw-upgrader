#include <iostream>
#include <signal.h>
#include <string.h>
#include <vector>

#include "FirmwareUpdater.h"

int main(int argc, char** argv) {
    // TODO let user specific device path in the command line args
    updater::FirmwareUpdater* updater = new updater::FirmwareUpdater("/dev/sdb");
    updater::FWVersionInfo_t info = updater->readFirmwareVersion();

    printf("%c%c%c%c%c%c\n", info.fwVersionDate[0], info.fwVersionDate[1], info.fwVersionDate[2], info.fwVersionDate[3], info.fwVersionDate[4], info.fwVersionDate[5]);

    printf("%s\n", info.controllerRevisionIdString);

    updater::TargetInfo_t targetInfo = updater->readTargetInfo();
    
    printf("signature word: 0x%x\n", targetInfo.signatureWord);
    printf("flash id 0: 0x%x\n", targetInfo.flashId_0);
    printf("flash id 1: 0x%x\n", targetInfo.flashId_1);
    printf("card type: 0x%x\n", targetInfo.cardType);
    printf("interleave Factor: 0x%x\n", targetInfo.interleaveFactor);
}