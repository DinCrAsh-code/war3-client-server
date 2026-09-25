//============================================================================
//  Five load-side helpers ability_typemigrate.cpp's own preamble calls,
//  each in its own shipped module (address neighbourhoods 0x6F0299xx and
//  0x6F03Exxx are one module together, distinct from 0x6F040xxx and from
//  0x6F02Cxxx/0x6F0384xx below) but grouped in one translation unit since
//  none of the five calls any of the others and each is small.
//
//  0x6F029970 - ConstructThreeFloatMiniHeader: placement-constructs a
//  0x34-byte {unsigned int kind; FloatMiniB elems[3];} local in place -
//  kind=3, each FloatMiniB base-then-derived (floatmini.h's own note on
//  why the redundant vtable stamp survives).  __thiscall, `retn 0`.
//
//  0x6F03EB80/0x6F03EC10/0x6F03EC90 - three "read up to N legacy values"
//  loops sharing one shape: read a declared count off the stream, replace
//  the header's own default (3) with it if smaller, then read that many
//  elements one at a time, bailing early the moment the stream runs out
//  (`m_readPos > m_field10`).  0x6F03EB80's elements are `FloatMini`
//  (read via CDataStore::ReadCFloat into a scratch, then written through
//  the element's own vtable slot 0 - FloatMini's "notify" slot,
//  floatmini.h); 0x6F03EC10's are plain dwords (CDataStore::ReadDword
//  straight into the element); 0x6F03EC90's are `FloatMiniB` (three
//  ReadCFloat calls per element - m_min/m_max written directly, m_value
//  through the same vtable slot 0 as 0x6F03EB80's).  All three are
//  __fastcall(CDataStore* store, header*), `retn 0`, returning `store`.
//
//  0x6F0384B0/0x6F02C740 - out of scope for a full account: a lazily-
//  allocated global tracker-record singleton (dword_6FAB6628, allocated
//  through Storm's own SMemAlloc the first time it is asked for) that
//  this switch's shared tail asks for by a literal (code, flags) pair
//  after every migration attempt, and a small notify that re-reads the
//  stream version twice and stores a fixed dword through the tracker
//  record's own +8 field only when both bounds gate it.  Transcribed
//  literally; neither field's real meaning is established by this call
//  tree.
//============================================================================
#include "ability.h"
#include "cdatastore.h"
#include "floatmini.h"
#include "CFloat.h"
#include "storm.h"

extern CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);   // cdatastore_readcfloat.cpp

struct SVersionedFloatMiniTriple { unsigned int m_count; FloatMini m_values[3]; };
struct SVersionedDwordTriple      { unsigned int m_count; unsigned int m_values[3]; };

class CThreeFloatMiniHeader
{
public:
    unsigned int m_kind;
    FloatMiniB   m_elems[3];
};

void __fastcall ConstructThreeFloatMiniHeader(SVersionedFloatMiniTriple* dest)
{
    CThreeFloatMiniHeader* self = (CThreeFloatMiniHeader*)dest;
    self->m_kind = 3;
    new (&self->m_elems[0]) FloatMiniB();
    new (&self->m_elems[1]) FloatMiniB();
    new (&self->m_elems[2]) FloatMiniB();
}

CDataStore* __fastcall ReadBoundedFloatMiniArray(CDataStore* store, SVersionedFloatMiniTriple* header)
{
    unsigned int bound = header->m_count;
    if (bound == 0)
        bound = 3;

    unsigned int declared;
    store->ReadDword(&declared);

    if (store->m_readPos > store->m_field10)
        return store;

    if (declared > bound)
    {
        store->m_field10 = store->m_field10 + 1;
        return store;
    }

    header->m_count = declared;
    if (declared == 0)
        return store;

    FloatMini* elem = header->m_values;
    for (unsigned int i = 0; i < declared; ++i, ++elem)
    {
        CFloat value;
        ReadCFloat(store, &value);
        typedef void (__thiscall *NotifyFn)(void*, CFloat*, int);
        ((NotifyFn)(*(void***)elem)[0])(elem, &value, 0);

        if (store->m_readPos > store->m_field10)
            break;
    }
    return store;
}

CDataStore* __fastcall ReadBoundedDwordArray(CDataStore* store, SVersionedDwordTriple* header)
{
    unsigned int bound = header->m_count;
    if (bound == 0)
        bound = 3;

    unsigned int declared;
    store->ReadDword(&declared);

    if (store->m_readPos > store->m_field10)
        return store;

    if (declared > bound)
    {
        store->m_field10 = store->m_field10 + 1;
        return store;
    }

    header->m_count = declared;
    if (declared == 0)
        return store;

    unsigned int* elem = header->m_values;
    for (unsigned int i = 0; i < declared; ++i, ++elem)
    {
        store->ReadDword(elem);
        if (store->m_readPos > store->m_field10)
            break;
    }
    return store;
}

CDataStore* __fastcall ReadBoundedFloatMiniBArray(CDataStore* store, SVersionedFloatMiniTriple* header)
{
    CThreeFloatMiniHeader* self = (CThreeFloatMiniHeader*)header;
    unsigned int bound = self->m_kind;
    if (bound == 0)
        bound = 3;

    unsigned int declared;
    store->ReadDword(&declared);

    if (store->m_readPos > store->m_field10)
        return store;

    if (declared > bound)
    {
        store->m_field10 = store->m_field10 + 1;
        return store;
    }

    self->m_kind = declared;
    if (declared == 0)
        return store;

    FloatMiniB* elem = self->m_elems;
    for (unsigned int i = 0; i < declared; ++i, ++elem)
    {
        CFloat value;
        ReadCFloat(store, &value);
        ReadCFloat(store, &elem->m_min);
        ReadCFloat(store, &elem->m_max);
        typedef void (__thiscall *NotifyFn)(void*, CFloat*, int);
        ((NotifyFn)(*(void***)elem)[0])(elem, &value, 0);

        if (store->m_readPos > store->m_field10)
            break;
    }
    return store;
}

//----------------------------------------------------------------------------
//  0x6F0384B0 - the lazily-allocated singleton tracker record.  `flags`
//  really is `0` at MigrateLegacyTypeId's own call site (its own version
//  gate having just failed), pushed straight into SMemAlloc's own log-
//  flags slot when the record does not exist yet.
//----------------------------------------------------------------------------
//  Reused as two different views of the same three dwords: this
//  function writes them as {code, flags, target}, NotifyLegacyMigrationDone
//  below reads the identical object back as {low, high, outPtr} - the
//  version-range gate this call tree's one live caller actually wants
//  (0x1795/6037 as the low bound, the flags word's -1 doubling as "no
//  upper bound", and the ability's own m_timer as the dword to refresh).
struct SLegacyTrackerRecord { unsigned int m_field0; unsigned int m_field4; unsigned int* m_field8; };

//  extern, not defined here: this is the real, shipped global at
//  0x6FAB6628, not a private zero-filled copy - see link_check.py's own
//  "funcmap.DATA global that src/ defines instead of declaring" check
//  and the CObserver::Dispatch/EnsureResource crash it exists because of.
extern SLegacyTrackerRecord* g_legacyTrackerRecord;  // dword_6FAB6628
static const char aEDrive1TempBui_92[] = "e:\\...";

void* __fastcall GetOrCreateLegacyTrackerRecord(unsigned int code, unsigned int flags, void* target)
{
    if (g_legacyTrackerRecord == 0)
    {
        SLegacyTrackerRecord* fresh =
            (SLegacyTrackerRecord*)SMemAlloc(0xC, aEDrive1TempBui_92, 0x9D, flags);
        if (fresh)
        {
            fresh->m_field0 = 0;
            fresh->m_field4 = 0xFFFFFFFF;
            fresh->m_field8 = 0;
        }
        g_legacyTrackerRecord = fresh;
    }

    g_legacyTrackerRecord->m_field0 = code;
    g_legacyTrackerRecord->m_field4 = flags;
    g_legacyTrackerRecord->m_field8 = (unsigned int*)target;
    return g_legacyTrackerRecord;
}

//----------------------------------------------------------------------------
//  0x6F02C740 - re-reads the stream's own version twice (once per bound)
//  and, only when both the low and the high gate hold and the record's
//  own out-pointer is non-null, ReadDword-refreshes the dword it points
//  at.  Falling *below* the low bound still consumes one dword off the
//  stream (discarded) to keep the read cursor aligned; landing strictly
//  *above* the high bound consumes nothing at all - two different "skip"
//  shapes the shipped code keeps apart, not a single shared bail.
//----------------------------------------------------------------------------
void __fastcall NotifyLegacyMigrationDone(CDataStore* store, void* trackerRecord)
{
    SLegacyTrackerRecord* range = (SLegacyTrackerRecord*)trackerRecord;
    int belowLow  = ((unsigned int)store->GetVersion() < range->m_field0);
    int aboveHigh = (range->m_field4 < (unsigned int)store->GetVersion());

    if (!belowLow && !aboveHigh && range->m_field8 != 0)
    {
        unsigned int value;
        store->ReadDword(&value);
        *range->m_field8 = value;
        return;
    }

    if (!aboveHigh)
        return;

    unsigned int discard;
    store->ReadDword(&discard);
}
