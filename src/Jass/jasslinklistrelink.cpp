//============================================================================
//  Three copies of the same body - `.\Jass.cpp`'s TSExplicitList link-offset
//  migration helper (the "poisoned 0xDDDDDDDD, terminator self-linked, then
//  re-linked to the real link offset" shape Containers/hashtablectors.cpp
//  already documents for the font cache and command tables' own
//  constructors), one instantiation per hash table whose redirected
//  constructor (jasshashtablectors.cpp) calls it:
//
//      0x6F44C840  from 0x6F44F990 (TSHashTable<NativeFunc,HASHKEY_STR>)
//      0x6F44D7A0  from 0x6F4528D0 (TSHashTable<Script2HandleReg,HASHKEY_NONE>)
//      0x6F44C550  from 0x6F450B80 (TSHashTable<JassHandle,HASHKEY_CONSTSTR>)
//
//  Kept a redirect for the same reason their three callers are: the type
//  genuinely touched is storm.h's TSExplicitList/TSLink pair at a *runtime*
//  link offset, reached only from a constructor this repo already keeps a
//  thunk (the four-vs-two vtable slot mismatch jasshashtablectors.h
//  explains), so nothing here ever calls this body live - there is no
//  caller in this tree to validate a from-scratch transcription of the
//  bucket-walk/relink loop against, and getting the loop's exit condition
//  wrong here is invisible to every check this repo runs short of a live
//  crash inside code that is, by construction, unreachable from our own
//  source.
//============================================================================
#define JASS_RELINK_THUNK(addr) \
    {                            \
        __asm { mov eax, addr } \
        __asm { jmp eax }        \
    }

//  0x6F44C840 - `retn 0`.
__declspec(naked) void __fastcall RelinkNativeFuncBucketList(void*)
    JASS_RELINK_THUNK(06F44C840h)

//  0x6F44D7A0 - `retn 0`.
__declspec(naked) void __fastcall RelinkScript2HandleBucketList(void*)
    JASS_RELINK_THUNK(06F44D7A0h)

//  0x6F44C550 - `retn 0`.
__declspec(naked) void __fastcall RelinkJassHandleBucketList(void*)
    JASS_RELINK_THUNK(06F44C550h)
