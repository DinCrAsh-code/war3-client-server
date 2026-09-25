//============================================================================
//  0x6F25C820 / 0x6F25C8D0 - "is there a row under this key" and "give me
//  the row".
//
//  Both start the same way: ask the table's own modified-object index at
//  +0x04 directly, and only when that misses go to the shared base store.
//  HasRow re-keys through the base store on the way - pack the four
//  characters, ask the store for its canonical key, unpack that back into a
//  buffer and try the index again - which is why PackFourCCString and
//  UnpackFourCCString both appear in one eleven-instruction stretch.
//
//  AcquireRow does not re-key: it asks the base store whether the key
//  exists, throws that answer away and returns null.  That is what the
//  shipped code does (`xor eax, eax` after the call, on the only path that
//  reaches it), not a transcription slip.
//
//  Both carry a /GS buffer cookie in the shipped prologue for the five-byte
//  key buffer, which this repo's fixed /GS- cannot emit.
//============================================================================
#include "objectdatatable.h"

//  0x6F4CA120 - the modified-object index's own lookup, and 0x6F2519F0 -
//  the base store's.  0x6F25B1D0 canonicalises a packed key, 0x6F005DC0
//  hands back the row object itself, 0x6F25B720 is the store singleton.
//  All five are naked redirects in objectdatatable_thunks.cpp.
void* __fastcall LookupModifiedObject(void* index, const char* key);
struct SBaseDataStore
{
    int  Contains(const char* key, int flags);   // 0x6F2519F0  retn 8
    int  Canonicalise(unsigned int packedKey);   // 0x6F25B1D0  retn 4
};
SBaseDataStore* __cdecl GetBaseDataStore();

//  0x6F4CA0F0 - DatabaseRecordSet_FindOrInsertEntry (citemclosure_dbfieldhash.cpp),
//  already a real body under that name.  Redeclared here on the return
//  type MakeRow's own callers actually use (void*, not the field-record
//  pointer that file's own signature spells) - same mangled symbol either
//  way, since the parameter types agree.
struct SDatabaseFieldRecord;
SDatabaseFieldRecord* __fastcall DatabaseRecordSet_FindOrInsertEntry(void* self, const char* key);

struct SRowHost
{
    void* LookupRow(const char* key);   // 0x6F005DA0 retn 4
    void* MakeRow(const char* key);     // 0x6F005DC0 retn 4
};

//----------------------------------------------------------------------------
//  0x6F005DA0 - CAbility depth<=5 closure batch 1.  A thin `this+4`-deref-
//  then-forward straight into LookupModifiedObject, the same shape
//  CObjectDataTable::HasRow's own preamble already has above - a distinct
//  address from it, reached as its own callee from sub_6F25F5C0's own
//  "does the row already exist" test (052990 dump), not itself a slot.
//----------------------------------------------------------------------------
void* SRowHost::LookupRow(const char* key)
{
    void* index = *(void**)((char*)this + 4);
    return LookupModifiedObject(index, key);
}

//----------------------------------------------------------------------------
//  0x6F005DC0 - the same `this+4`-deref-then-forward shape as LookupRow
//  above, into DatabaseRecordSet_FindOrInsertEntry instead of
//  LookupModifiedObject.  A naked thunk until this session.
//----------------------------------------------------------------------------
void* SRowHost::MakeRow(const char* key)
{
    void* self = *(void**)((char*)this + 4);
    return DatabaseRecordSet_FindOrInsertEntry(self, key);
}

int CObjectDataTable::HasRow(const char* key)
{
    void* index = *(void**)((char*)this + 4);
    //  The found arm first: the shipped `jz` jumps forward past the whole
    //  base-store path onto this build's `mov eax, 1`.
    if (LookupModifiedObject(index, key) != 0)
        return 1;

    SBaseDataStore* store = GetBaseDataStore();
    if (store->Contains(key, 0) == 0)
        return 0;

    //  The canonical key is named, not nested: written as one expression
    //  MSVC hoists UnpackFourCCString's constant `5` above both inner calls,
    //  where the shipped stream pushes it after the second one returns.
    char canonical[5];
    int canonicalKey = store->Canonicalise(PackFourCCString(key));
    UnpackFourCCString(canonicalKey, canonical, 5);

    //  The pointer is handed back as it stands - the shipped code returns
    //  eax raw.  `!= 0` would normalise it to 0/1 through `neg`/`sbb`/`neg`,
    //  three instructions the target does not have, and no caller cares:
    //  every one of them tests the answer.
    return (int)LookupModifiedObject(*(void**)((char*)this + 4), canonical);
}

void* CObjectDataTable::AcquireRow(const char* key)
{
    void* index = *(void**)((char*)this + 4);
    if (LookupModifiedObject(index, key) != 0)
        return ((SRowHost*)this)->MakeRow(key);

    //  The store is named, not reached through inline: MSVC evaluates a
    //  call's arguments before its object expression, so the in-place form
    //  puts the singleton fetch *after* the two pushes where the shipped
    //  stream has it before them (docs/msvc-vc8-idioms.md, "A named
    //  receiver forces the object expression to be evaluated first").
    SBaseDataStore* store = GetBaseDataStore();
    store->Contains(key, 0);
    return 0;
}
