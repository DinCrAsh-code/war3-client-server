//============================================================================
//  0x6F3EC740 - CPathingFootprint::~CPathingFootprint(), `retn 0`.  No SEH
//  furniture at all (unlike the constructor next door) - a plain `this`-in-
//  ecx member body.  Frees the per-cell flag array (Storm_403, tagged
//  ".?AE@@" per the dump's own `aE_1` type-descriptor name - the same
//  generic-array-of-bytes tag CPathingMapIndicator::~CPathingMapIndicator's
//  own two `aG_4`/`aAvc*` frees use for their own non-object arrays) when
//  the grid holds more than 0x40 cells, zeroes the three grid-state fields,
//  frees the flag array a *second* time under the same guard (the dump
//  really does repeat the whole `cmp/jbe/free` sequence verbatim - not a
//  transcription slip, see the two identical blocks below), then re-stamps
//  this object's own vtable pointer back to `TRefCnt` before returning -
//  the standard "unwind to my own base class before the implicit base
//  destructor call" step every virtual-bearing destructor gets, and (since
//  `TRefCnt::~TRefCnt()` is `{}`, refcnt.h) the last thing this function
//  does.
//============================================================================
#include "pathingfootprint.h"

//  Storm.dll ordinal 403 - SMemFree, tagged with an RTTI type-descriptor
//  name rather than a bare "delete" string - see
//  docs/msvc-vc8-idioms.md, "Declaring imports so the diff can see them".
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                         unsigned int flags);

static const char kTypeTag_E[] = ".?AE@@";   // aE_1 in the dump

CPathingFootprint::~CPathingFootprint()
{
    if (m_cellCount > 0x40)
        SMemFree(m_cellFlags, kTypeTag_E, -2, 0);

    m_cellCount = 0;
    *(unsigned int*)m_reserved1C = 0;
    m_cellFlags = 0;

    //  Same guard, same free, a second time - the dump repeats this block
    //  rather than folding it into the one above (the field state between
    //  the two is: cellCount==0, cellFlags==0, so the second `cmp
    //  cellCount,0x40` always takes the `jbe` branch and the second
    //  `Storm_403` never actually runs live - transcribed anyway since it
    //  is what the shipped code does).
    if (m_cellCount > 0x40)
        SMemFree(m_cellFlags, kTypeTag_E, -2, 0);

    //  Re-stamp TRefCnt's own vtable - the compiler-inserted vptr reset
    //  every virtual-bearing destructor gets before its (empty) base-class
    //  body runs, same as trefcnt_dtorstamp_6f383b90.cpp's own explicit
    //  `TRefCnt::~TRefCnt()` call for the identical shape.
    TRefCnt::~TRefCnt();
}

//----------------------------------------------------------------------------
//  0x6F3F03E0 - vtable slot 1, overriding TRefCnt::DeleteSelf.  Out of this
//  session's own scope (no dump) - naked thunk, kept only so this class's
//  own vtable is the real two-slot shape rather than missing a slot.
//----------------------------------------------------------------------------
__declspec(naked) TRefCnt* CPathingFootprint::DeleteSelf(int /*flags*/)
{
    __asm
    {
        mov eax, 06F3F03E0h
        jmp eax
    }
}

