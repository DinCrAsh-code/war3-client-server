//============================================================================
//  0x6F3F4A30 - look the key up and, when it is there, register it with this
//  object under kind 1.
//
//  The found pointer goes back into the *parameter's* own stack slot, which
//  is what the shipped `mov [esp+8+arg_0], esi` says: a separate local would
//  be a second slot and two more instructions.
//
//  A member, not a `__fastcall` free function: the shipped body is `retn 4`
//  against a receiver in ecx and one push, so a free function taking the
//  receiver would put the key in edx and clean nothing.
//============================================================================
#include <windows.h>

#include "objectdataowner.h"

//  0x6F3F4410 - not in this tree.
void* __fastcall ObjectDataFind(const void* key);

void* SObjectDataOwner::FindAndRegister(const void* key)
{
    key = ObjectDataFind(key);

    if (key != 0)
        Register(1, (void**)&key);

    return (void*)key;
}
