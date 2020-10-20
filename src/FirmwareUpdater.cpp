#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>

#include "FirmwareUpdater.h"

using namespace updater;

/* HELPER FUNCTIONS */

U32 makeBigEndian(U32 v) {
    U32 b0, b1, b2, b3;

    b0 = (v & 0x000000ff) << 24u;
    b1 = (v & 0x0000ff00) << 8u;
    b2 = (v & 0x00ff0000) >> 8u;
    b3 = (v & 0xff000000) >> 24u;

    return b0 | b1 | b2 | b3;
}

void writeBigEndian(unsigned char* arr, U32 v)
{
    arr[0] = v >> 24;
    arr[1] = v >> 16;
    arr[2] = v >> 8;
    arr[3] = v;
}

unsigned char* loadFileAsBuffer(const std::string filename, size_t* size)
{
    std::fstream fs;
    fs.open(filename, std::fstream::in | std::fstream::binary);
    if (!fs.is_open()) {
        throw updater::UpdateExeception("cannot open file", filename);
    }

    // get size of file
    fs.seekg(0, std::ios::end);
    *size = (size_t) fs.tellg();
    fs.seekg(0, std::ios::beg);
    unsigned char* buffer = new unsigned char[*size];
    fs.read(reinterpret_cast<char *>(buffer), *size);

    fs.close();

    return buffer;
}

/* CLASS FUNCTIONS */

UpdateExeception::UpdateExeception(std::string msg, const char* value)
{
    this->msg = msg.append("\"").append(value).append("\"");
}

UpdateExeception::UpdateExeception(std::string msg, std::string value)
{
    this->msg = msg.append("\"").append(value).append("\"");
}

const char* UpdateExeception::what() const throw()
{
    return msg.c_str();
}

FirmwareUpdater::FirmwareUpdater(const char* devPath, std::string configPath) 
{
    SKBaseDeviceInfo* devInfo = SKStorageProtocol::scan(devPath);
    this->scsiInterface = new SKScsiProtocol(devInfo->devicePath, devInfo->deviceHandle);
    
    const TargetInfo_t targetInfo = readTargetInfo(); 
    this->currDevice = {readFirmwareVersion(), loadDDData(configPath), findDDEntry(targetInfo, configPath)};

    // remove the "dd.txt" part from the config path to get the archive path
    archivePath = std::string(configPath);
    archivePath = archivePath.substr(0, archivePath.length() - 6);

    delete devInfo;
}

FirmwareUpdater::~FirmwareUpdater()
{
    delete scsiInterface;
}

// prints a list of the fields that are needed to perform firmware upgrade for the current device
void FirmwareUpdater::inspectCurrentDevice() 
{
    // ugly... someone needs to fix this lol
    printf("current fw: %c%c%c%c%c%c\n", currDevice.info.fwVersionDate[0], currDevice.info.fwVersionDate[1], currDevice.info.fwVersionDate[2], currDevice.info.fwVersionDate[3], currDevice.info.fwVersionDate[4], currDevice.info.fwVersionDate[5]);
    printf("controller revision: %c%c\n", currDevice.info.controllerRevisionIdString[0], currDevice.info.controllerRevisionIdString[1]);
    
    // make that ^^^ look like this vvv
    printf("general fw features: %s\n", currDevice.ddData.generalFwFeatures);
    printf("driver strengths: %s\n", currDevice.ddData.drvStrengths);
    printf("flash device id: %s\n", currDevice.ddEntry.flashDeviceId);
    printf("specific fw features: %s\n", currDevice.ddEntry.specificFwFeatures);
    printf("firmware file name: %s\n", currDevice.ddEntry.firmwareFileName);
    printf("anchor file name: %s\n", currDevice.ddEntry.anchorFileName);
}

// issues the VCs needed to perform the firmware upgrade process
void FirmwareUpdater::update()
{
    /* 
    this sequence of VCs is taken from the hsfmt source code. the firmware manual also 
    explains this process. i will suggest you to refer to the manual AND source code. 
    you have to do a whole lot more reading to understand the big picture by using the 
    manual alone. 

    i'll summarize the firmware update process below:

    - there are four phases (0..3)
        - phase #0: assemble - perpares the data to send over the VCs
        - phase #1: prepare  - issues the "update perpare" VC (makes a data buffer on the 
                               drive)
        - phase #2: transfer - uses a series of "update transfer" VCs to send firmware data
        - phase #3: execute  - issues the "update execute" VC that triggers drive to use 
                               the data buffer to updae itself
    */

    // ===phase #0: assemble===
    FWUpdatePrepareData_t prepareData = {};

    // sectors in files (first fw, second fw, and anchor)
    char pathToFW[MAX_LINE_LEN];
    char pathToAnchor[MAX_LINE_LEN];
    sprintf(pathToFW, "%s%s.e90", archivePath.c_str(), currDevice.ddEntry.firmwareFileName);
    sprintf(pathToAnchor, "%s%s.e90", archivePath.c_str(), currDevice.ddEntry.anchorFileName);
    int totalSectorsInFw = (int) sectorsInFile(pathToFW);

    U32 firstSectors = std::min(totalSectorsInFw, 257);
    U32 secondSectors = std::min(totalSectorsInFw - prepareData.sectorsInFirstFW, (unsigned int) 255);

    prepareData.sectorsInFirstFW = makeBigEndian(firstSectors);
    prepareData.sectorsInSecondFW = makeBigEndian(secondSectors);
    prepareData.sectorsAnchorProg = makeBigEndian(sectorsInFile(pathToAnchor));

    // specific fw mask and value
    prepareData.clearMaskSpecificFW = 0;//0xffffffff;
    prepareData.setMaskSpecificFW = 0;//makeBigEndian(hexStringToU32(currDevice.ddEntry.specificFwFeatures));

    // general fw mask and value
    prepareData.clearMaskGeneralFW = 0;//0xffffffff;
    prepareData.setMaskGeneralFW = 0;//makeBigEndian(hexStringToU32(currDevice.ddData.generalFwFeatures));

    // driver strength mask and value
    prepareData.clearMaskDriverStrength = 0;//0xffffffff;
    prepareData.setMaskDriverStrength = 0;//makeBigEndian(hexStringToU32(currDevice.ddData.drvStrengths));

    // stuff the struct into a buffer
    SKAlignedBuffer* prepareDataBuffer = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    memset(prepareDataBuffer->ToDataBuffer(), 0, SECTOR_SIZE_IN_BYTES); // initialize to 0
    memcpy(prepareDataBuffer->ToDataBuffer(), &prepareData, sizeof(FWUpdatePrepareData_t));
    
    // ===phase #1: prepare===

    // write_set_address_extension(0)
    SKScsiCommandDesc* setAddressExtension = SKU9VcCommandDesc::createSetAddressExtension(0);
    scsiInterface->issueScsiCommand(setAddressExtension, new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES));

    // write_firmware_update_prepare()
    SKScsiCommandDesc* firmwareUpdatePrepare = SKU9VcCommandDesc::createFirmwareUpdatePrepare();
    scsiInterface->issueScsiCommand(firmwareUpdatePrepare, prepareDataBuffer);

    // ===phase #2 transfer===

    // load file data
    size_t anchorSizeInBytes = 0;
    unsigned char* anchorFileData = loadFileAsBuffer(pathToAnchor, &anchorSizeInBytes);
    printf("anchor size: %d (%d sectors)\n", anchorSizeInBytes, anchorSizeInBytes/SECTOR_SIZE_IN_BYTES);

    // write_set_base_address()
    SKAlignedBuffer* baseAddress = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    memset(baseAddress->ToDataBuffer(), 0, SECTOR_SIZE_IN_BYTES);

    SKScsiCommandDesc* setBaseAddress = SKU9VcCommandDesc::createSetBaseAddress();
    scsiInterface->issueScsiCommand(setBaseAddress, baseAddress);

    int transferCount = 0;
    // write_firmware_update_transfer()
    SKScsiCommandDesc* firmwareUpdateTransfer = SKU9VcCommandDesc::createFirmwareUpdateTransfer();
    SKAlignedBuffer* anchorDataBuffer = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    memcpy(anchorDataBuffer->ToDataBuffer(), anchorFileData, anchorSizeInBytes);
    scsiInterface->issueScsiCommand(firmwareUpdateTransfer, anchorDataBuffer);
    
    delete[] anchorFileData;

    transferCount++;

    size_t fwSizeInBytes = 0;
    unsigned char* fwFileData = loadFileAsBuffer(pathToFW, &fwSizeInBytes);
    printf("firmware size: %d (%d sectors)\n", fwSizeInBytes, fwSizeInBytes/SECTOR_SIZE_IN_BYTES);

    SKAlignedBuffer* fwDataBuffer = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    for (U32 i = 0; i < firstSectors + secondSectors; i++) {
        // set base address
        writeBigEndian(baseAddress->ToDataBuffer(), i + 32);
        scsiInterface->issueScsiCommand(setBaseAddress, baseAddress);

        memcpy(fwDataBuffer->ToDataBuffer(), &fwFileData[i * SECTOR_SIZE_IN_BYTES], SECTOR_SIZE_IN_BYTES);
        scsiInterface->issueScsiCommand(firmwareUpdateTransfer, fwDataBuffer);
        transferCount++;
    }

    // ===phase #3: execute===

    SKAlignedBuffer* updateExecuteReturnData = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    // set to 0xff to see if data was written to the buffer
    memset(updateExecuteReturnData->ToDataBuffer(), 0xff, SECTOR_SIZE_IN_BYTES); 
    
    // read_firmware_update_execute()
    SKScsiCommandDesc* firmwareUpdateExecute = SKU9VcCommandDesc::createFirmwareUpdateExecute();
    scsiInterface->issueScsiCommand(firmwareUpdateExecute, updateExecuteReturnData);

    printf("number of transfer commands: %d\n", transferCount);

    U8 exitCode = updateExecuteReturnData->ToDataBuffer()[0];
    printf("first byte of return buffer %x%x%x%x\n", 
        updateExecuteReturnData->ToDataBuffer()[0], 
        updateExecuteReturnData->ToDataBuffer()[1], 
        updateExecuteReturnData->ToDataBuffer()[2], 
        updateExecuteReturnData->ToDataBuffer()[3]);
    if (exitCode == 0) {
        printf("success! (exit code %x)\n", exitCode);
    } else if (exitCode = 0xff) {
        printf("did not get any data from execute command (%x)\n", exitCode);
    } else {
        printf("failure! (exit code %x)\n", exitCode);
    }
}

// Issues the VC to read firmware version information
const FWVersionInfo_t FirmwareUpdater::readFirmwareVersion()
{
    // prepare and execute the command data
    SKAlignedBuffer* buffer = new SKAlignedBuffer(SECTOR_SIZE_IN_BYTES);
    SKScsiCommandDesc* desc = SKU9VcCommandDesc::createReadFirmwareVersion();
    this->scsiInterface->issueScsiCommand(desc, buffer);

    // format returned data
    FWVersionInfo_t info = {};
    memcpy(&info, buffer->ToDataBuffer(), sizeof(FWVersionInfo_t));

    // clean up
    delete buffer;
    delete desc;

    return info;
}

// Issues the VC to inspect information about the target device
const TargetInfo_t FirmwareUpdater::readTargetInfo()
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
const DeviceDescriptionData_t FirmwareUpdater::loadDDData(std::string path)
{
    // locate general fw features (the -features flag in dd.txt)
    const std::string rawFeaturesLine = findLineInDD(path, "-features=");
    // locate driver strength (the -drv_strengths flag in dd.txt)
    const std::string rawDrvStrengths = findLineInDD(path, "-drv_strengths=");

    // validating...
    if (rawFeaturesLine.empty()) {
        throw updater::UpdateExeception("cannot find line with matching search string ", "-features=");
    }

    if (rawDrvStrengths.empty()) {
        throw updater::UpdateExeception("cannot find line with matching search string ", "-drv_strengths=");
    }
    
    // construct well-formed data
    DeviceDescriptionData_t dd = {};

    // copies everything after '-<flag>='
    rawFeaturesLine.substr(10).copy(dd.generalFwFeatures, rawFeaturesLine.substr(10).length());
    rawDrvStrengths.substr(15).copy(dd.drvStrengths, rawDrvStrengths.substr(15).length());

    return dd;
}

// finds the target's device description line in the dd.txt file specified by the given path
const DeviceDescriptionEntry_t FirmwareUpdater::findDDEntry(const TargetInfo_t &info, std::string path) 
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

    if (entryLine.empty()) {
        throw updater::UpdateExeception("cannot find line with matching search string ", searchString);
    }

    // locate columns
    std::string specificFwFeatures = locateWord(entryLine, 7);
    std::string firmwareFileName = locateWord(entryLine, 8);
    std::string anchorFileName = locateWord(entryLine, 9);

    // convert raw dd.txt entry string entry into well formatted data type
    DeviceDescriptionEntry_t dde = {};

    memcpy(dde.flashDeviceId, flashIdStr, sizeof(flashIdStr));
    specificFwFeatures.copy(dde.specificFwFeatures, specificFwFeatures.length());
    firmwareFileName.copy(dde.firmwareFileName, firmwareFileName.length());
    anchorFileName.copy(dde.anchorFileName, anchorFileName.length());

    return dde;
}

/*
* searches the dd.txt file at 'path' for the first line that has a start that matches 'find'.
* left trims (removes white space to the left of) the string
*/
const std::string FirmwareUpdater::findLineInDD(std::string path, const char* find)
{
    std::fstream fs;

    // TODO: I don't think this program needs to open up a new stream every time this function is called
    fs.open(path, std::fstream::in);
    if (!fs.is_open()) {
        throw updater::UpdateExeception("cannot open file", path);
    }

    // find a line that has a start that matches the value of 'find'
    const unsigned int findLen = strlen(find);
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
    while(str[i] == ' ' && i < str.length()) {
        i++;
    }

    int currWord = 0;
    int start = i;
    for (i; i < str.length(); i++) {
        // find starting of word
        if (currWord != word) {
            if (str[i] == ' ') {
                currWord++;
                start = i + 1;
            }
        } else {
            // find end of word
            if (str[i] == ' ') {
                return str.substr(start, i - start);
            }
        }
    }
    
    return "";
}

const U32 FirmwareUpdater::hexStringToU32(const char* str)
{
    std::stringstream ss;
    ss << std::hex << str;

    U32 val;
    ss >> val;
    return val;
}

const U32 FirmwareUpdater::sectorsInFile(std::string pathOnDisk)
{
    std::fstream fs;
    fs.open(pathOnDisk, std::fstream::in | std::fstream::binary);

    if (!fs.is_open())
        throw updater::UpdateExeception("cannot open file", pathOnDisk);

    fs.ignore( std::numeric_limits<std::streamsize>::max() );
    fs.close();
    
    return fs.gcount()/SECTOR_SIZE_IN_BYTES;
}
