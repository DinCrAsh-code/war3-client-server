//============================================================================
//  0x6F493CA0 - PathPlacementFilter.
//
//  Not part of this dump: FindFreeGridPoint (0x6F4957A0) only ever takes
//  this function's address to install as the search's own filter when a
//  unit type was named, and nothing in the call tree we have reads its
//  body.  This is not a reconstruction - it is a direct redirect to the
//  game's own code at its real address, so that a build hooking the rest
//  of this target still calls the genuine original filter when that path
//  is taken, rather than link-failing on a symbol nothing here defines.
//============================================================================
#include "pathfind.h"

__declspec(naked) int __fastcall PathPlacementFilter(const CFloat*, void*)
{
    __asm
    {
        mov eax, 06F493CA0h
        jmp eax
    }
}
