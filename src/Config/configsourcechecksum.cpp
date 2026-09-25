//============================================================================
//  0x6F6FA990 - CConfigSource::ComputeChecksum().  Called (non-virtually,
//  through a real thiscall, not a vtable slot) by CMiscCustom::ComputeChecksum
//  (CMiscCustom.cpp) once per non-null source, each contribution folded
//  through the family's usual rol(hash ^ contribution, 3) combine.
//
//  Left a naked redirect rather than a reconstruction: its own body reads
//  m_pFile and hands it, a callback (sub_6F6FA970) and a stack accumulator
//  to a generic thiscall enumerator (sub_6F4D0570) that walks
//  CConfigFile's own section/entry hash tables - the same
//  "HashKeyNodeA then HashKeyNodeB" chain config.h's own header comment
//  already flags as a real, two-level walk (0x6F4D05B0/0x6F4D1D30/
//  0x6F4D1E60), not a boring MSVC internal, but a genuinely separate
//  investigation from this batch's own checksum-provider-registry seeds.
//  Its own `retn` is plain (no stack cleanup - all arguments arrive in
//  registers), so the redirect needs no argument-byte declaration beyond
//  the thiscall `this` already in ecx.
//============================================================================
#include "config.h"

#pragma warning(disable : 4035)   // naked: no return value, leaves via the
                                  // transcribed jmp's own retn

__declspec(naked) unsigned int CConfigSource::ComputeChecksum() const
{
    __asm
    {
        mov     eax, 06F6FA990h
        jmp     eax
    }
}

#pragma warning(default : 4035)
