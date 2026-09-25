//============================================================================
//  See jasshashtablectors2.h.
//============================================================================
#include "jasshashtablectors2.h"

#define JASS_HASHTABLE2_THUNK(addr) \
    {                                \
        __asm { mov eax, addr }     \
        __asm { jmp eax }            \
    }

__declspec(naked) SScriptDataTable* SScriptDataTable::Construct()
    JASS_HASHTABLE2_THUNK(06F450450h)

__declspec(naked) SFuncAddrTable* SFuncAddrTable::Construct()
    JASS_HASHTABLE2_THUNK(06F450720h)

__declspec(naked) SString2HandleTable* SString2HandleTable::Construct()
    JASS_HASHTABLE2_THUNK(06F450E80h)

__declspec(naked) SFuncAddr2HandleTable* SFuncAddr2HandleTable::Construct()
    JASS_HASHTABLE2_THUNK(06F4512E0h)

__declspec(naked) SFuncAddr2NameTable* SFuncAddr2NameTable::Construct()
    JASS_HASHTABLE2_THUNK(06F451740h)

//  0x6F453FF0 - vtable slot 2, `retn 4`.  Real destructor 0x6F450640
//  tail-jumps into __CxxFrameHandler3 - see jasshashtablectors2.h's own
//  note on why this stays a naked thunk of its own.
__declspec(naked) void* SScriptDataTable::ScalarDeletingDestructor(unsigned int)
    JASS_HASHTABLE2_THUNK(06F453FF0h)

//  0x6F454020 - vtable slot 2, `retn 4`.  Real destructor 0x6F450AA0, same
//  SEH shape as above.
__declspec(naked) void* SFuncAddrTable::ScalarDeletingDestructor(unsigned int)
    JASS_HASHTABLE2_THUNK(06F454020h)

//  0x6F454080 - vtable slot 2, `retn 4`.  Real destructor 0x6F451200, same
//  SEH shape as above.
__declspec(naked) void* SString2HandleTable::ScalarDeletingDestructor(unsigned int)
    JASS_HASHTABLE2_THUNK(06F454080h)

//  0x6F4540B0 - vtable slot 2, `retn 4`.  Real destructor 0x6F451660, same
//  SEH shape as above.
__declspec(naked) void* SFuncAddr2HandleTable::ScalarDeletingDestructor(unsigned int)
    JASS_HASHTABLE2_THUNK(06F4540B0h)

//  0x6F4540E0 - vtable slot 2, `retn 4`.  Real destructor 0x6F451AE0, same
//  SEH shape as above.
__declspec(naked) void* SFuncAddr2NameTable::ScalarDeletingDestructor(unsigned int)
    JASS_HASHTABLE2_THUNK(06F4540E0h)

__declspec(naked) void __fastcall RelinkScriptDataBucketList(void*)
    JASS_HASHTABLE2_THUNK(06F44C3B0h)

__declspec(naked) void __fastcall RelinkFuncAddrBucketList(void*)
    JASS_HASHTABLE2_THUNK(06F44C4E0h)

__declspec(naked) void __fastcall RelinkString2HandleBucketList(void*)
    JASS_HASHTABLE2_THUNK(06F44C710h)

__declspec(naked) void __fastcall RelinkFuncAddr2HandleBucketList(void*)
    JASS_HASHTABLE2_THUNK(06F44C780h)

__declspec(naked) void __fastcall RelinkFuncAddr2NameBucketList(void*)
    JASS_HASHTABLE2_THUNK(06F44C7F0h)
