#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>

#include "FirmwareUpdater.h"

using namespace updater;

UpdateExeception::UpdateExeception(std::string msg, const char* path)
{
    this->msg = msg.append(" at path \"").append(path).append("\"");
}

const char* UpdateExeception::what() const throw()
{
    return msg.c_str();
}

FirmwareUpdater::FirmwareUpdater(const char* devPath) 
{
    SKBaseDeviceInfo* devInfo = SKStorageProtocol::scan(devPath);
    this->scsiInterface = new SKScsiProtocol(devInfo->devicePath, devInfo->deviceHandle);
    
    delete devInfo;
}

FirmwareUpdater::~FirmwareUpdater()
{
    delete scsiInterface;
}

// Issues the VC to read firmware version information
FWVersionInfo_t FirmwareUpdater::readFirmwareVersion()
{
    // prepare and execute the command data
    SKAlignedBuffer* buffer = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    SKScsiCommandDesc* desc = SKU9VcCommandDesc::createReadFirmwareVersion();
    this->scsiInterface->issueScsiCommand(desc, buffer);

    // format returned data
    FWVersionInfo_t info;
    memcpy(&info, buffer->ToDataBuffer(), sizeof(FWVersionInfo_t));

    // clean up
    delete buffer;
    delete desc;

    return info;
}

// Issues the VC to inspect information about the target device
TargetInfo_t FirmwareUpdater::readTargetInfo()
{
    // prepare and execute the command
    SKAlignedBuffer* buffer = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    SKScsiCommandDesc* desc = SKU9VcCommandDesc::createTargetInfo();
    this->scsiInterface->issueScsiCommand(desc, buffer);

    // format returned data
    TargetInfo_t targetInfo;
    memcpy(&targetInfo, buffer->ToDataBuffer(), sizeof(TargetInfo_t));

    delete buffer;
    delete desc;

    return targetInfo;
}

// loads the common data in the dd.txt file specified by the given path
const DeviceDescriptionData_t FirmwareUpdater::loadDDData(const char* path)
{
    DeviceDescriptionData_t dd;
    return dd;
}

// finds the target's device description line in the dd.txt file specified by the given path
const DeviceDescriptionEntry_t FirmwareUpdater::findDDEntry(const TargetInfo_t &info, const char* path) 
{
    // construct a hex string representation of flash id from the supplied target into
    char flashIdStr[13]; // flashId is 6 bytes, so we need 12 bytes + 1 ("\0") to encode it as a hex string
    sprintf(flashIdStr, "%x%x", info.flashId_1, info.flashId_0);

    // change the endianness of the hex string
    for (int i = 0; i <= 4; i += 2) {
        int j = 10 - i;
        char tempA = flashIdStr[j];
        char tempB = flashIdStr[j+1];

        flashIdStr[j] = flashIdStr[i];
        flashIdStr[j+1] = flashIdStr[i+1];
        flashIdStr[i] = tempA;
        flashIdStr[i+1] = tempB;
    }

    // construct search string (<flashIdStr> <interfaceType> <interleaveFactor> like "98d7a03277d6 1 2")
    char searchString[MAX_LINE_LEN];
    sprintf(searchString, "%s %x %x", flashIdStr, (info.cardType >> 8), (info.interleaveFactor >> 24));
    
    // locate the raw string data in dd.txt
    const std::string entryLine = findLineInDD(path, searchString);

    /*if (entryLine.empty()) {
        return;
    }*/

    // convert raw string entry into well formatted data type
    DeviceDescriptionEntry_t dde;
    memcpy(dde.flashDeviceId, flashIdStr, sizeof(flashIdStr));

    std::cout << "word found: " << locateWord(entryLine, 0) << std::endl;
    std::cout << "word found: " << locateWord(entryLine, 1) << std::endl;

    return dde;
}

/*
* searches the dd.txt file at 'path' for the first line that has a start that matches 'find'.
* left trims (removes white space to the left of) the string
*/
const std::string FirmwareUpdater::findLineInDD(const char* path, const char* find)
{
    std::fstream fs;
    fs.open(path, std::fstream::in);

    if (!fs.is_open()) {
        throw updater::UpdateExeception("cannot open file", path);
    }

    std::cout << "searching file at \"" << path << "\" for line that starts with \"" << find << "\"..." << std::endl;

    const unsigned int findLen = strlen(find);

    // find a line that has a start that matches the value of 'find'
    char line[MAX_LINE_LEN];
    while(fs.getline(line, MAX_LINE_LEN)) {

        // only check lines that are the same size or longer than 'find'
        if (strlen(line) >= findLen) {
            
            // skip white space in the current line (left trim)
            int offset = 0;
            while (line[offset] == ' ') {
                offset++;
            }
            
            int matching = 0;
            for (int i = 0; i < findLen; i++) {
                if (find[i] == line[i + offset]) {
                    matching++;
                }
            }

            // found a line that has a beginning that matches the value of 'find'
            if (matching == findLen) {
                fs.close();
                std::cout << "found: " << &line[offset] << std::endl;
                return &line[offset]; // left-trimmed line
            }
        }
    }

    fs.close();

    // did not find a matching line
    return "";
}

const std::string FirmwareUpdater::locateWord(const std::string &str, int word)
{
    int i = 0;
    // skip white space
    while(str[i] == ' ') {
        i++;
    }

    int currWord = 0;
    int start = i;
    for (i; i < str.length(); i++) {
        // find starting of word
        if (currWord != word) {
            if (str[i] == ' ') {
                currWord++;
                start = i;
            }
        } else {
            // find end of word
            if (str[i] == ' ') {
                return str.substr(start, i);
            }
        }
    }
    
    return "";
}
