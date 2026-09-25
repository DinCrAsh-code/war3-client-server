//============================================================================
//  Support types for abilitytablesound.cpp (0x6F32F7B0) - separate from
//  Widget/widget.h's SWidgetOffsetSource, which is a different shipped
//  struct (its one named field sits at +0x10; this one is read at +0x0/
//  +0x4/+0x8, three plain floats).
//============================================================================
#ifndef ABILITYTABLESOUND_H
#define ABILITYTABLESOUND_H

struct SVec3
{
    float x, y, z;
};

//  0x6F32F700 - below this batch's own worklist; a real, unhooked callee
//  left as a naked thunk.  __fastcall(int entry, void* posOrNull) plus four
//  stack dwords (all zero at every call site seen so far), retn 10h -
//  confirmed from the shipped tail's own `pop esi` / `retn 10h`.
void __fastcall FilterAbilitySoundCue(int entry, void* posOrNull,
                                      int a3, int a4, int a5, int a6);

#endif
