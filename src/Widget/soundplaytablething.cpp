//============================================================================
//  0x6F320BC0 - `retn 4`, __thiscall(ecx=table, stack=key): a
//  TSHashTable<T,K>::Ptr(const char*)-shaped lookup (mask at +0x24, buckets
//  at +0x1C, same generic Containers/tshash.inl shape Widget/sprresource.h
//  and Widget/modelrecordregistry.cpp already document for their own
//  registries) - hashes the key via Storm_590 then walks the bucket chain,
//  calling Storm_509 per candidate to compare the stored key string.
//
//  Reached only from 0x6F32F607, inside the not-yet-reconstructed
//  sound-playing dispatcher sub_6F32F310 (too large for this batch - see
//  its own worklist entry) - so the table's real element/key type is not
//  established anywhere in this closure, exactly the same "unconfirmed
//  element/key type" situation Widget/widgetsoundlabel.cpp's own
//  SSoundLabelTableThunk::LookupSoundLabelTable (0x6F3199F0) already
//  documents for an identical-shaped sibling. Thunked for the same reason:
//  guessing T here would be inventing a layout, not confirming one.
//============================================================================

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  Out of this batch's scope (see header comment above).
struct SSoundPlayTableThunk
{
    int __thiscall Ptr(const char* key);
};
ADDR_THUNK(int __thiscall SSoundPlayTableThunk::Ptr(const char* key),
           0x6F320BC0)
