//============================================================================
//  0x6F6D9F40 - close an OsNet subsystem mask over its own dependencies.
//
//  Bits 0 and 3 are always wanted; the timer (2) pulls in 0x10, and either
//  the timer or the select thread (2|4) pulls in 0x20.  Written as three
//  reads of `*mask` rather than one local because that is what the shipped
//  stream does - it re-loads the word after each `or`.
//============================================================================
#include "ostcp.h"

namespace OsNet
{


void __fastcall OsNetExpandMask(unsigned int* mask)
{
    *mask |= 9;

    if (*mask & 2)
        *mask |= 0x10;

    if (*mask & 6)
        *mask |= 0x20;
}

}  // namespace OsNet
