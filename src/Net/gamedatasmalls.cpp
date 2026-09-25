//============================================================================
//  0x6F535D70 - one write that gives up on the whole stream if it failed.
//  Four arguments forwarded untouched, then a single test.
//
//  A member for the reason src/Net/datastorereadfixedname.cpp gives: the
//  shipped body is `retn 10h` against a receiver in ecx and four pushes, so
//  a __fastcall free function would clean four bytes too few.
//============================================================================
#include <windows.h>
#include "gamedatawriter.h"

void SGameDataWriter::WriteOrAbandon(int a, int b, int c, int d)
{
    if (WriteRecord(a, b, c, d))
        Abandon();
}
