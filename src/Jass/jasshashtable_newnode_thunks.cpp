//============================================================================
//  0x6F454250 / 0x6F4537F0 / 0x6F451EF0 - a TSHashTable<T,K>::NewNode /
//  CheckGrow / Initialize instantiation from this closure (the shapes
//  Containers/tshash.inl already reconstructs for the canonical
//  0x6F4713D0/0x6F4710A0/0x6F470CC0/0x6F4701D0/0x6F4700B0 instantiation -
//  same bucket-header math, `lea eax,[edi+edi*2]; lea eax,[ecx+eax*4]`
//  picking a 12-byte TSExplicitList<T> slot the same way).
//
//  Kept redirects rather than a fresh instantiation of tshash.inl's own
//  template: NewNode's own body dispatches through the table's AllocNode
//  vtable slot, and nothing in this dump pins down *which* of this
//  closure's several TSHashTable<T,K> pairs (NativeFunc/HASHKEY_STR,
//  Script2HandleReg/HASHKEY_NONE, JassHandle/HASHKEY_CONSTSTR,
//  ScriptData/FuncAddr/String2HandleReg all on HASHKEY_CONSTSTR,
//  FuncAddr2HandleReg/FuncAddr2NameReg on HASHKEY_NONE, HashedString on
//  HASHKEY_STR) these three actually belong to - every one of them shares
//  this repo's own four-vs-two vtable slot mismatch (jasshashtablectors.h),
//  so guessing a T,K pair to instantiate against would risk stamping
//  nothing wrong (NewNode never stamps a vtable itself) but validating
//  nothing either, since there is no live caller in this tree to compare
//  against.
//============================================================================
#define JASS_NEWNODE_THUNK(addr) \
    {                             \
        __asm { mov eax, addr }  \
        __asm { jmp eax }         \
    }

//  0x6F454250 - `retn 0xC`.
__declspec(naked) void* __fastcall JassHashTableNewNode(unsigned int)
    JASS_NEWNODE_THUNK(06F454250h)

//  0x6F4537F0 - `retn 4`.
__declspec(naked) int __fastcall JassHashTableCheckGrow(unsigned int)
    JASS_NEWNODE_THUNK(06F4537F0h)

//  0x6F451EF0 - `retn 0`.
__declspec(naked) void __fastcall JassHashTableInitialize()
    JASS_NEWNODE_THUNK(06F451EF0h)
