//============================================================================
//  0x6F2B7860 - CItem's override of vtable slot 15 (+0x3C), CItem::Save's
//  counterpart (item_save.cpp).
//
//  Field for field with Save, with four asymmetries that are real behaviour
//  and not reconstruction noise:
//
//   * the FloatMiniB value is not stored back into the field - it is read
//     into a scratch CFloat and *published* through FloatMini's own vtable
//     slot 0 with flag 0, so everything watching the value learns about the
//     load the same way it learns about a gameplay change;
//   * the record array's count is forced back to 4 after the read, and any
//     entry the save did not carry has its first two words reset to -1
//     (its third is left alone) - a short save loads short and is repaired,
//     it does not shrink the array;
//   * the last four fields are *version-gated*: each one builds a
//     three-word descriptor on the heap (versiongate.h) naming the stream
//     versions it exists in and where to put it, and hands that to a
//     helper.  The four gates use three heap objects between them, because
//     the two CAgentTimer loads go through the same helper and therefore
//     share its static;
//   * two of those four consume-and-discard the bytes when the stream is
//     *newer* than the field's upper bound, which is how the loader stays
//     aligned across a field a later version dropped.
//
//  The four version numbers are 0x1770, 0x177B, 0x1795 and 0x17A9 (6000,
//  6011, 6037, 6057) - a save-format version counter, not a build number.
//
//  Own translation unit: fifteen real calls out of it.
//============================================================================
#include "item.h"
#include "cdatastore.h"
#include "handletriple.h"
#include "versiongate.h"
#include "storm.h"   // placement new

//  0x6F6EED50 - cdatastore_readcfloat.cpp.
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);
//  0x6F2B30B0 / 0x6F051F00 - item_loadrecords.cpp / versionedload.cpp.
CDataStore* __fastcall ReadRecordTriples(CDataStore* store, void* array);
CDataStore* __fastcall LoadVersionedObject(CDataStore* store,
                                           const SVersionGate* gate);

//  Storm.dll ordinal 401.
void* __stdcall SMemAlloc(unsigned int size, const char* logfilename,
                          int logline, unsigned int flags);

//  The allocation tag every one of the four gates carries: not `.\CItem.cpp`
//  but the full path of the header the inline helper lives in, at its line
//  157 - which is the evidence that the gate construction is inlined out of
//  a header and not written out here four times.
static const char kGateHeader[] =
    "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\Engine\\SaveVersion.h";

//  One static per *helper*, not per call site: the two CAgentTimer loads
//  share g_objectGate because they go through the same helper, while the
//  two field loads each have their own.  That sharing is exactly what the
//  shipped code shows - dword_6FAB6634 is written twice in this one
//  function.
static SVersionGate* g_objectGate = 0;      // dword_6FAB6634
static SVersionGate* g_dwordGate = 0;       // dword_6FAB6628
static SVersionGate* g_dwordAltGate = 0;    // dword_6FAB660C

//  Written out at each site rather than through a helper that hands the
//  pointer back: the shipped code re-loads the global before *every* field
//  store (`mov edx,SYM` / `mov [edx+4],ebx` / `mov ecx,SYM` / ...), which
//  is what writing through the global itself gives - the compiler cannot
//  prove a store through it does not change it.  A local holding the
//  pointer caches it and loses three loads per gate.
#define ENSURE_GATE(slot)                                                   \
    do {                                                                    \
        if ((slot) == 0)                                                    \
        {                                                                   \
            void* raw = SMemAlloc(12, kGateHeader, 0x9D, 0);                \
            if (raw != 0)                                                   \
                (slot) = new (raw) SVersionGate();                          \
            else                                                            \
                (slot) = 0;                                                 \
        }                                                                   \
    } while (0)

void CItem::Load(CDataStore* store)
{
    CSelectable::Load(store);

    unsigned int scratch;

    //  Read into a scratch and published, not stored - see the file header.
    //  Uninitialised: the reader fills it in, and the zeroing default
    //  constructor costs a dead store the shipped code does not have.
    //  All four accesses go through one pointer to the member, which is
    //  what produces the shipped `lea ebx,[edi+54h]` / `[ebx+8]` /
    //  `[ebx+0Ch]` / `[ebx]` chain instead of four `lea`s off `this`.
    //  One scratch dword for the whole function - the CFloat that gets
    //  published and all three plain dword reads below.  The shipped code
    //  parks every one of them in the dead `store` parameter's own home
    //  slot and allocates no frame at all; two differently typed locals get
    //  two slots and cost the function a `push`.
    FloatMiniB* range = &m_floatB;
    CFloat* lo = &range->m_min;
    ReadCFloat(store, (CFloat*)&scratch);
    ReadCFloat(store, lo);
    ReadCFloat(store, lo + 1);
    ((FloatMiniB::NotifyFn)(*(void***)range)[0])(range, (CFloat*)&scratch, 0);

    store->ReadDword(&scratch);
    m_position.m_reserved84 = scratch;
    store->ReadDword(&scratch);
    m_position.m_reserved88 = scratch;

    ReadHandleTriple(store, (SHandleTriple*)&m_position.m_placementRef);

    store->ReadDword(&scratch);
    m_position.m_reserved74 = scratch;

    ReadRecordTriples(store, &m_position.m_reserved98);

    //  Repair, not truncate: the array is always four entries long, and the
    //  ones the save did not carry go back to their -1 sentinel.  The third
    //  word of each is deliberately left as it was - the shipped loop only
    //  writes the first two.
    int loaded = (int)m_position.m_reserved98;
    if (loaded != 4)
    {
        m_position.m_reserved98 = 4;
        if (loaded < 4)
        {
            //  A do/while, not a `for`: `loaded < 4` already guarantees at
            //  least one pass, and a `for` makes MSVC emit an entry guard
            //  the shipped code does not have.
            SHandleTriple* p = (SHandleTriple*)&m_position.m_reserved9C + loaded;
            int n = 4 - loaded;
            do
            {
                p->m_typeTag = 0xFFFFFFFF;
                p->m_handle = 0xFFFFFFFF;
                p++;
            } while (--n != 0);
        }
    }

    {
        ENSURE_GATE(g_objectGate);
        g_objectGate->m_since = 0x1770;
        g_objectGate->m_until = -1;
        g_objectGate->m_target = &m_timer1;
        LoadVersionedObject(store, g_objectGate);
    }

    {
        ENSURE_GATE(g_dwordGate);
        g_dwordGate->m_since = 0x177B;
        g_dwordGate->m_until = -1;
        g_dwordGate->m_target = &m_position.m_reservedCC;

        int atLeastSince = (store->GetVersion() >= g_dwordGate->m_since);
        int atMostUntil = (g_dwordGate->m_until >= store->GetVersion());

        //  The target read *once*, into a local, and used for both the
        //  null test and the store: the shipped code has a single
        //  `mov ebp,[ebp+8]` where reading `g_dwordGate->m_target` twice
        //  emits a `cmp [ebp+8], 0` and then a second load.  Everything
        //  else in this function reaches its gate through the global on
        //  purpose - the shipped code re-loads it before every field
        //  store - but the target pointer is the one value it does hold.
        void* target = g_dwordGate->m_target;
        if (atLeastSince && atMostUntil && target != 0)
        {
            store->ReadDword(&scratch);
            *(unsigned int*)target = scratch;
        }
        else if (!atMostUntil)
        {
            store->ReadDword(&scratch);
        }
    }

    {
        ENSURE_GATE(g_dwordAltGate);
        g_dwordAltGate->m_since = 0x1795;
        g_dwordAltGate->m_until = -1;
        g_dwordAltGate->m_target = &m_position.m_reserved80;

        int atLeastSince = (store->GetVersion() >= g_dwordAltGate->m_since);
        int atMostUntil = (g_dwordAltGate->m_until >= store->GetVersion());

        //  Read once - see the block above.
        void* target = g_dwordAltGate->m_target;
        if (atLeastSince && atMostUntil && target != 0)
            store->ReadDwordAlt2((unsigned int*)target);
        else if (!atMostUntil)
            store->ReadDwordAlt2(&scratch);
    }

    {
        ENSURE_GATE(g_objectGate);
        g_objectGate->m_since = 0x17A9;
        g_objectGate->m_until = -1;
        g_objectGate->m_target = &m_timer2;
        LoadVersionedObject(store, g_objectGate);
    }
}
