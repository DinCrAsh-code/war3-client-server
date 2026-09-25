//============================================================================
//  Two small, unrelated GameMain-closure leaves (gamemain-batch-5).  A
//  third (0x6F6D98D0, a one-line TlsFree() wrapper) was dropped from here:
//  it was independently assigned to both this batch and gamemain-batch-3,
//  which reconstructed it first as FreeThreadLocalIndex (Sync/tlsfree.cpp)
//  - same address, same body, one name should own it.
//============================================================================
#include "storm.h"
#include <windows.h>

//  0x6F525F00 - a bare global-read accessor, in the same source cluster as
//  the display-device state machine (sub_6F526CA0, GameMain-batch-5 #19,
//  thunked) that owns dword_6FA88720 - a -1/0/1/2 enum this dump never
//  fully labels the meaning of past "not yet decided/failed/software/
//  hardware".  Named g_screenStatusA by gamemain-batch-2, which reached
//  this same global independently and first (funcmap.py DATA).
int __fastcall QueryDisplayDeviceState();

extern int g_screenStatusA;   // dword_6FA88720

int __fastcall QueryDisplayDeviceState()
{
    return g_screenStatusA;
}

//  0x6F4C3510 - allocate one fixed-size (0x48-byte) block through Storm,
//  tagged with the module's own debug file/line the way every other
//  SMemAlloc call site in this binary is.  The type this feeds is not in
//  this call tree (no caller reached here at this dump's depth), so the
//  return is left `void*` rather than guessing a struct shape from a size
//  alone.
void* __fastcall AllocPropBlock()
{
    return SMemAlloc(0x48, ".\\Prop.cpp", 0x36, 8);
}
