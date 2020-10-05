#ifndef AtaProtocol_h
#define AtaProtocol_h

#include "StorageKitAlignedBuffer.h"
#include "StorageKitAtaCommandDesc.h"
#include "StorageKitStorageProtocol.h"

class SKAtaProtocol : public SKStorageProtocol
{
  private:
    SKAtaOutputTaskFileRegister lastOutputTaskFileRegister;

  public:
    SKAtaProtocol(const std::string &devicePath, const DeviceHandle &handle);
    virtual ~SKAtaProtocol() override;

    SKReturnCode issueCommand(const SKAtaCommandDesc *cmdDesc, SKAlignedBuffer *buffer, const U32 &timeoutInSeconds = 10);
    SKAtaOutputTaskFileRegister getLastOutputTaskFileRegister() const;
};

#endif
