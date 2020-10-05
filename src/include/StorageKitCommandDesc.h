#ifndef CommandDesc_h
#define CommandDesc_h

#include "defines.h"

enum SKDataAccess
{
    NONE,
    READ_FROM_DEVICE,
    WRITE_TO_DEVICE
};

class SKCommandDesc
{
  protected:
    SKCommandType commandType;

  protected:
    SKCommandDesc();
    SKCommandDesc(const SKCommandType &type);

  public:
    SKCommandType getCommandType() const;
};

#endif
