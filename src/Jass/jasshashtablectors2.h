//============================================================================
//  Five more `.\Jass.cpp` TSHashTable constructors, redirected for the same
//  four-vs-two vtable slot reason jasshashtablectors.h documents:
//
//      0x6F450450  TSHashTable<ScriptData,        HASHKEY_CONSTSTR>
//      0x6F450720  TSHashTable<FuncAddr,          HASHKEY_CONSTSTR>
//      0x6F450E80  TSHashTable<String2HandleReg,  HASHKEY_CONSTSTR>
//      0x6F4512E0  TSHashTable<FuncAddr2HandleReg,HASHKEY_NONE>
//      0x6F451740  TSHashTable<FuncAddr2NameReg,  HASHKEY_NONE>
//
//  plus their five own TSExplicitList link-offset migration helpers
//  (jasslinklistrelink.cpp's three others, same situation):
//
//      0x6F44C3B0, 0x6F44C4E0, 0x6F44C710, 0x6F44C780, 0x6F44C7F0
//============================================================================
#ifndef JASSHASHTABLECTORS2_H
#define JASSHASHTABLECTORS2_H

struct SScriptDataTable
{
    SScriptDataTable* Construct();   // 0x6F450450 - `retn 0`
    //  0x6F453FF0 - vtable slot 2.  Same `ScalarDeletingDestructor` shape
    //  as every other one in this repo (Pathfinding/
    //  pathclient_scalardtor.cpp): real destructor, then `SMemFree` when
    //  bit 0 of `flags` is set.  The real destructor
    //  (0x6F450640, outside this session's own range) tail-jumps into
    //  `__CxxFrameHandler3` - the same fixed-toolchain SEH shape this
    //  repo already can't emit for JassInstance::Construct/Run and
    //  friends (docs/targets/JassVM.md) - so it stays a naked thunk of
    //  its own rather than a real `~SScriptDataTable()` call.
    void* ScalarDeletingDestructor(unsigned int flags);
    char m_storage[0x28];
};

struct SFuncAddrTable
{
    SFuncAddrTable* Construct();     // 0x6F450720 - `retn 0`
    //  0x6F454020 - vtable slot 2, same shape/reason as SScriptDataTable's
    //  own above (real destructor 0x6F450AA0 is the same
    //  __CxxFrameHandler3-tailed shape).
    void* ScalarDeletingDestructor(unsigned int flags);
    char m_storage[0x28];
};

struct SString2HandleTable
{
    SString2HandleTable* Construct(); // 0x6F450E80 - `retn 0`
    //  0x6F454080 - vtable slot 2, same shape/reason (real destructor
    //  0x6F451200 is the same __CxxFrameHandler3-tailed shape).
    void* ScalarDeletingDestructor(unsigned int flags);
    char m_storage[0x28];
};

struct SFuncAddr2HandleTable
{
    SFuncAddr2HandleTable* Construct(); // 0x6F4512E0 - `retn 0`
    //  0x6F4540B0 - vtable slot 2, same shape/reason (real destructor
    //  0x6F451660 is the same __CxxFrameHandler3-tailed shape).
    void* ScalarDeletingDestructor(unsigned int flags);
    char m_storage[0x28];
};

struct SFuncAddr2NameTable
{
    SFuncAddr2NameTable* Construct();  // 0x6F451740 - `retn 0`
    //  0x6F4540E0 - vtable slot 2, same shape/reason (real destructor
    //  0x6F451AE0 is the same __CxxFrameHandler3-tailed shape).
    void* ScalarDeletingDestructor(unsigned int flags);
    char m_storage[0x28];
};

void __fastcall RelinkScriptDataBucketList(void*);        // 0x6F44C3B0
void __fastcall RelinkFuncAddrBucketList(void*);          // 0x6F44C4E0
void __fastcall RelinkString2HandleBucketList(void*);     // 0x6F44C710
void __fastcall RelinkFuncAddr2HandleBucketList(void*);   // 0x6F44C780
void __fastcall RelinkFuncAddr2NameBucketList(void*);     // 0x6F44C7F0

#endif
