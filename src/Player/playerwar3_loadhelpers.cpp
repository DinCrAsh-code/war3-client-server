//============================================================================
//  CPlayerWar3::Load's own small per-field readers (playerwar3_load.cpp),
//  split out here per CLAUDE.md's "one translation unit per original
//  module" - each of these lives in its own address neighbourhood, away
//  from the root's own (0x6F4193xx).  The load-side counterpart of every
//  writer playerwar3_save.cpp already names, in the same field order.
//============================================================================
#include "playerwar3.h"
#include "cdatastore.h"
#include "integerlistener.h"
#include "game.h"
#include "agentregistry.h"
#include "agenttypedslots.h"
#include "storm.h"   // placement new

//  0x6F03FA30 - handle.cpp.
CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag);
//  Storm.dll ordinal 401.
void* __stdcall SMemAlloc(unsigned int size, const char* logfilename,
                          int logline, unsigned int flags);
//  0x6F2D6720 - agentunitrefread.cpp.
CDataStore* __fastcall ReadUnitRef(CDataStore* store, SCheckedUnitSlot* out);

//----------------------------------------------------------------------------
//  0x6F4192C0 - the load-side counterpart of WriteAgentHandlePairDupA
//  (playerwar3_savewritehandlepair.cpp): ReadUnitRef's own shape
//  (agentunitrefread.cpp) with SCheckedAllySlot::Assign in place of
//  SCheckedUnitSlot::Assign, over the +0x38 ally slot.  `retn 0`.
//----------------------------------------------------------------------------
CDataStore* __fastcall ReadAllySlotRef(CDataStore* store, SCheckedAllySlot* out)
{
    unsigned int handle;
    unsigned int typeTag;
    store->ReadDwordAlt(&handle);
    store->ReadDwordAlt(&typeTag);

    SItemHandleObject* object =
        (SItemHandleObject*)LookupHandle(handle, (int)typeTag);

    if (object != 0 && object->m_kindTag == (int)kAgileHandleTag)
    {
        out->Assign((CAgent*)object->m_field54);
    }
    else
    {
        CAgent* held = out->m_value;
        if (held != 0)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
            out->m_value = 0;
        }
    }

    return store;
}

//----------------------------------------------------------------------------
//  0x6F2D3FB0 - the load-side counterpart of the m_dumpArrayA/m_dumpArrayB
//  writer loop in playerwar3_save.cpp: read a count and, unless it exceeds
//  the field's own current value (its own capacity - 12 the first time this
//  ever runs, since the constructor seeds both count words to 0xC), that
//  many raw dwords into the fixed 12-entry array right after it.  Used
//  twice by the root (+0x1E0 and +0x214) - the identical body compiled once
//  and shared, unlike playerwar3_save.cpp's own two write loops which are
//  a real loop the compiler did not fold into one, either.
//
//  A count over capacity does not touch the array at all - it forces the
//  store's own read position one past its declared end
//  (`store->m_readPos = store->m_field10 + 1`), the same "poison the
//  stream" idiom `versionedload.cpp`'s own family reads back off of a
//  version mismatch, here used against a corrupt/oversized count instead.
//  `retn 0`.
//----------------------------------------------------------------------------
CDataStore* __fastcall ReadFixedIntMiniArray(CDataStore* store,
                                             unsigned int* countField)
{
    unsigned int capacity = *countField;
    if (capacity == 0)
        capacity = 0x0C;

    unsigned int count = 0;
    store->ReadDword(&count);

    if (store->m_readPos > store->m_field10)
        return store;

    if (count > capacity)
    {
        store->m_readPos = store->m_field10 + 1;
        return store;
    }

    *countField = count;
    if (count == 0)
        return store;

    unsigned int* array = countField + 1;
    for (unsigned int i = 0; i < count; i++)
    {
        store->ReadDword(&array[i]);
        if (store->m_readPos > store->m_field10)
            break;
    }
    return store;
}

//----------------------------------------------------------------------------
//  0x6F0537C0 - the load-side counterpart of WriteListenerPair::Do
//  (playerwar3_save.cpp), used seven times over +0x27C..+0x294.  The
//  sentinel (first int == -1) is a hard early return - the shipped code
//  neither releases nor nulls the slot when it sees it, so this does not
//  either.  Otherwise: release whatever is there, adopt a freshly allocated
//  IntegerListener (refcount 1) regardless of whether the second int was
//  itself a sentinel, and - if the {handle, typeTag} pair resolves through
//  LookupHandle - copy the resolved object's own m_flags/m_typeTag
//  (CHandleObject, game.h; the same +0x14/+0x18 offsets
//  floatlistenerrefread.cpp's own SMadeModifier reads under different
//  names) onto the new listener's own two reserved ints.  No '+agl' kind
//  tag check here, unlike ReadAllySlotRef/ReadUnitRef above - the shipped
//  stream reads object->m_flags/m_typeTag unconditionally once `object` is
//  non-null.  `retn 0`.
//----------------------------------------------------------------------------
static const char kIntegerListenerAllocTag[] =
    "e:\\drive1\\temp\\buildwar3x\\engine\\source\\base\\intprop.h";

CDataStore* __fastcall ReadIntegerListenerField(CDataStore* store, void** out)
{
    int handle;
    int typeTag;
    store->ReadDword((unsigned int*)&handle);
    store->ReadDword((unsigned int*)&typeTag);

    if (handle == -1)
        return store;

    IntegerListener* made = 0;
    void* raw = SMemAlloc(0x10, kIntegerListenerAllocTag, 0x3A, 0);
    if (raw != 0)
    {
        made = new (raw) IntegerListener();
        made->m_refcount += 1;
    }

    IntegerListener* old = *(IntegerListener**)out;
    if (old != 0)
    {
        if (--old->m_refcount == 0)
            old->ReleaseSelf();
    }
    *out = made;

    CHandleObject* object = LookupHandle((unsigned int)handle, typeTag);
    if (object != 0)
    {
        made->m_reserved08 = (int)object->m_flags;
        made->m_reserved0C = object->m_typeTag;
    }
    return store;
}

//----------------------------------------------------------------------------
//  0x6F3A8A50 - the load-side counterpart of the "dead hero" array writer
//  in playerwar3_save.cpp: read a count and, unless it exceeds the array's
//  own current count (its own fallback capacity - 0xFFFF0 the first time),
//  SetCount to it and read that many unit references (ReadUnitRef,
//  agentunitrefread.cpp) into the growable array's own storage - the same
//  `TSGrowableArray<SCheckedUnitSlot>` the constructor's own +0x2AC
//  allocation already resolves this field to (playerwar3_ctorsubobjects.cpp).
//  `retn 0`.
//----------------------------------------------------------------------------
CDataStore* __fastcall ReadDeadHeroArray(CDataStore* store,
                                         TSGrowableArray<SCheckedUnitSlot>* array)
{
    unsigned int capacity = array->m_count;
    if (capacity == 0)
        capacity = 0xFFFF0;

    unsigned int count = 0;
    store->ReadDword(&count);

    if (store->m_readPos > store->m_field10)
        return store;

    if (count > capacity)
    {
        store->m_readPos = store->m_field10 + 1;
        return store;
    }

    array->SetCount(count);

    for (unsigned int i = 0; i < count; i++)
    {
        ReadUnitRef(store, &array->m_data[i]);
        if (store->m_readPos > store->m_field10)
            break;
    }
    return store;
}

//----------------------------------------------------------------------------
//  0x6F418790 - the load-side counterpart of the IntegerMini-array writer
//  in playerwar3_save.cpp: read a count and, unless it exceeds the array's
//  own current count (fallback capacity 0xFFFF0), SetCount it
//  (PlayerWar3IntMiniArraySetCount, playerwar3_ctorsubobjects.cpp - now
//  externally linked because this is its second caller) and read that many
//  raw dwords into the growable buffer, re-fetching the data pointer once
//  up front (the header's own +0x08 field) rather than per element - unlike
//  the dead-hero/BestHeroData readers, nothing here re-derives it inside
//  the loop, because SetCount cannot move `header->m_data` again once the
//  grow above it has already happened.  `retn 0`.
//----------------------------------------------------------------------------
struct PlayerWar3IntMiniArrayHeader
{
    unsigned int  m_alloc;
    unsigned int  m_count;
    unsigned int* m_data;
    unsigned int  m_chunk;
};

void PlayerWar3IntMiniArraySetCount(void* arrv, unsigned int count);

CDataStore* __fastcall ReadIntMiniArray(CDataStore* store,
                                        PlayerWar3IntMiniArrayHeader* header)
{
    unsigned int capacity = header->m_count;
    if (capacity == 0)
        capacity = 0xFFFF0;

    unsigned int count = 0;
    store->ReadDword(&count);

    if (store->m_readPos > store->m_field10)
        return store;

    if (count > capacity)
    {
        store->m_readPos = store->m_field10 + 1;
        return store;
    }

    PlayerWar3IntMiniArraySetCount(header, count);

    for (unsigned int i = 0; i < count; i++)
    {
        store->ReadDword(&header->m_data[i]);
        if (store->m_readPos > store->m_field10)
            break;
    }
    return store;
}

//----------------------------------------------------------------------------
//  0x6F419330 - the load-side counterpart of PlayerWar3WriteBestHeroDataArray
//  (playerwar3_save.cpp): read a count and, unless it exceeds the array's
//  own current count (fallback capacity 0xFFFF0), SetCount it
//  (TSFixedArray<BestHeroData>::SetCount, 0x6F418CE0,
//  Player/bestherodataarray.cpp) and read that many 0xC-byte
//  {typeId, level, experience} records - re-reading the data pointer every
//  iteration by an accumulated byte offset, the same reload idiom
//  playerwar3_save.cpp's own writer already documents for this exact
//  array.  `retn 0`.
//----------------------------------------------------------------------------
CDataStore* __fastcall ReadBestHeroDataArray(CDataStore* store,
                                             TSFixedArray<BestHeroData>* header)
{
    unsigned int capacity = header->m_count;
    if (capacity == 0)
        capacity = 0xFFFF0;

    unsigned int count = 0;
    store->ReadDword(&count);

    if (store->m_readPos > store->m_field10)
        return store;

    if (count > capacity)
    {
        store->m_readPos = store->m_field10 + 1;
        return store;
    }

    header->SetCount(count);

    unsigned int byteOffset = 0;
    for (unsigned int i = 0; i < count; i++)
    {
        char* elem = (char*)header->m_data + byteOffset;
        store->ReadDword((unsigned int*)(elem + 0));
        store->ReadDword((unsigned int*)(elem + 4));
        store->ReadDword((unsigned int*)(elem + 8));
        if (store->m_readPos > store->m_field10)
            break;
        byteOffset += 0x0C;
    }
    return store;
}
