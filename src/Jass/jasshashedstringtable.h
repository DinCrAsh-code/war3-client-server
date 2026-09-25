//============================================================================
//  `.\Jass.cpp`'s own HashedString table pair - a
//  TAllocatorHashTable<HashedString,HASHKEY_STR,1024> wrapped in a small
//  "own or borrow the buffer" struct (0x6F454390).
//============================================================================
#ifndef JASSHASHEDSTRINGTABLE_H
#define JASSHASHEDSTRINGTABLE_H

//  TSHashTable<HashedString,HASHKEY_STR> - 0x6F450150's own redirected
//  constructor target, the same four-vs-two vtable slot situation
//  jasshashtablectors.h documents.  0x28 bytes, Storm/storm.h's own layout.
struct SHashedStringHashTable
{
    SHashedStringHashTable* Construct();   // 0x6F450150 - `retn 0`
    char m_storage[0x28];
};

//  TAllocatorHashTable<HashedString,HASHKEY_STR,1024> - derives from the
//  table above and stamps its own vtable (the "allocate/free one node"
//  slots allochashtable.h's own header explains this repo deliberately
//  does not declare `virtual` for).  Kept a redirect for the same reason:
//  constructing it for real would need this repo's own vtable to agree
//  with the shipped one, and nothing here reconstructs that shape.
struct SHashedStringAllocTable
{
    SHashedStringAllocTable* Construct();  // 0x6F453ED0 - `retn 0`
    char m_storage[0x28 + 0x10];  // TSHashTable's 0x28 plus the bucket array header
};

//  0x6F454390 - `retn 0x10`.  ecx = this, four stack args: a callback
//  pointer, an opaque tag, an externally-supplied buffer (or null to
//  allocate one), and a size/kind word.  When no buffer is supplied it
//  allocates 0x38 bytes (".\Jass.cpp" line 0x156) and default-constructs a
//  SHashedStringAllocTable into it.
struct SHashedStringTablePrimer
{
    void*                    m_field0;
    void*                    m_field4;
    SHashedStringAllocTable* m_table;
    void*                    m_fieldC;

    SHashedStringTablePrimer(void* field0, void* field4,
                              SHashedStringAllocTable* externalTable,
                              void* fieldC);
};

//  0x6F44C340 - `retn 0`, SHashedStringHashTable::Construct's own
//  TSExplicitList relink helper (jasslinklistrelink.cpp's three others).
void __fastcall RelinkHashedStringBucketList(void*);

#endif
