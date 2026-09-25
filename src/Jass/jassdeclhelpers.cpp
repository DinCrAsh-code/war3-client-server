//============================================================================
//  Two trivial one-field-deeper accessors on a JASS declaration object -
//  neither dump names the class, so both are written as raw pointer
//  arithmetic through the one field (+0x18) they agree on rather than a
//  guessed struct.  NodeVar::NodeVar (nodevar.cpp) is the one caller of the
//  first that is in this batch's scope.
//============================================================================
#include "jassnodes.h"

//  0x6F460670 - `*(decl+0x18) + 0x10`: the resolved type pair NodeVar's own
//  constructor copies out of a variable declaration.
void* __fastcall JassDecl_TypeSlot(void* decl)
{
    return *(char**)((char*)decl + 0x18) + 0x10;
}

//  0x6F460750 - the same +0x18 indirection, a different field (+0x28) past
//  it.  The `cmp [x+4], 9` the shipped body carries changes nothing this
//  call tree can see - no branch follows it, and this reconstruction's own
//  copy of it optimises away identically - so it is not reproduced; see the
//  BEHAVIOUR entry.
void* __fastcall JassDecl_Field28(void* decl)
{
    return *(void**)(*(char**)((char*)decl + 0x18) + 0x28);
}
