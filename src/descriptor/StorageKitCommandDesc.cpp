#include "StorageKitCommandDesc.h"

SKCommandDesc::SKCommandDesc()
{
}

SKCommandDesc::SKCommandDesc(const SKCommandType &type)
{
    this->commandType = type;
}

SKCommandType SKCommandDesc::getCommandType() const
{
    return this->commandType;
}
