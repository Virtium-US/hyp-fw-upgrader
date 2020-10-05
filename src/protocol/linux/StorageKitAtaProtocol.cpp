#include "StorageKitAtaProtocol.h"

using namespace std;

SKAtaProtocol::SKAtaProtocol(const std::string &devicePath, const DeviceHandle &handle) :
    SKStorageProtocol(devicePath, handle)
{
}

SKAtaProtocol::~SKAtaProtocol()
{
}

SKReturnCode SKAtaProtocol::issueCommand(const SKAtaCommandDesc *cmdDesc, SKAlignedBuffer *buffer, const U32 &timeoutInSeconds)
{
    return SKErrorInvalidCommand;
}

SKAtaOutputTaskFileRegister SKAtaProtocol::getLastOutputTaskFileRegister() const
{
    return this->lastOutputTaskFileRegister;
}
