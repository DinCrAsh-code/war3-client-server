//============================================================================
//  Three small sockaddr_in-shaped utilities in the 0x6F6DAExx/0x6F6DBxxx
//  neighbourhood - free `__fastcall` functions, not members of any class,
//  because none of the three ever reads `this` (each caller in this dump
//  overwrites `ecx` from its own stack argument before the call, the same
//  "declared as a member purely to occupy a vtable slot" shape
//  netproviderloop_addrslots.cpp's wrappers document).  A 16-byte
//  sockaddr_in-shaped struct: `unsigned short family` (always 2, AF_INET),
//  `unsigned short port` (network order), `unsigned int addr` (network
//  order, opaque to these three - never byte-swapped), 8 bytes of zero pad.
//============================================================================
#include <winsock2.h>

//----------------------------------------------------------------------------
//  0x6F6DB010 - field-by-field equality of two such structs' first 8 bytes
//  (family/port/addr-as-two-words/pad-word), reporting which field differed
//  first (4/3/2, or a plain bool for the last) through `outFlag` if given,
//  and returning whether they were equal overall.  The intermediate 4/3/2
//  "which field" codes are the shipped code's own early-exit values; only
//  their zero-ness is read for the return value.
//----------------------------------------------------------------------------
int __fastcall AddrFieldsEqual(const void* a, const void* b, int* outFlag)
{
    const unsigned short* pa = (const unsigned short*)a;
    const unsigned short* pb = (const unsigned short*)b;
    int diff;

    if (pa[0] != pb[0])
        diff = 4;
    else if (pa[1] != pb[1])
        diff = 3;
    else if (pa[2] != pb[2])
        diff = 2;
    else
        diff = (pa[3] != pb[3]);

    if (outFlag)
        *outFlag = diff;

    return diff == 0;
}

//----------------------------------------------------------------------------
//  0x6F6DB070 - `outPort`, if given, gets the struct's port field converted
//  host order; the return value is always the raw (never byte-swapped)
//  4-byte address field.
//----------------------------------------------------------------------------
unsigned int __fastcall GetAddrIPAndPort(const void* addr, unsigned short* outPort)
{
    if (outPort)
        *outPort = ntohs(*(const unsigned short*)((const char*)addr + 2));
    return *(const unsigned int*)((const char*)addr + 4);
}

//----------------------------------------------------------------------------
//  0x6F6DAEF0 - build a fresh struct at `out`: family = AF_INET, port
//  byte-swapped to network order, `addr` stored as-is (already network
//  order at every call site in this dump), pad zeroed.
//----------------------------------------------------------------------------
void __fastcall BuildSockAddr(unsigned int addr, unsigned short port, void* out)
{
    unsigned short* p = (unsigned short*)out;
    p[0] = 2;
    p[1] = htons(port);
    *(unsigned int*)((char*)out + 4)  = addr;
    *(unsigned int*)((char*)out + 8)  = 0;
    *(unsigned int*)((char*)out + 0xC) = 0;
}
