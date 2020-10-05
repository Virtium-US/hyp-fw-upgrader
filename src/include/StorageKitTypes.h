#ifndef Types_h
#define Types_h

typedef char S8;
typedef short S16;
typedef int S32;
typedef long S64;

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long long U64;

enum SKDeviceType
{
    ATA,
    SAT,
    HYP,
    SCSI,
    NVME,
    UNKNOWN
};

#endif
