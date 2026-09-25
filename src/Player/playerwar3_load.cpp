//============================================================================
//  0x6F419720 - CPlayerWar3::Load, vtable slot 15 (+0x3C).  See playerwar3.h.
//
//  Save's own exact mirror, field for field, in the same order - down to
//  reusing Save's own three-loop-then-two-singles message sweep through the
//  base class's inherited slot 21 (+0x54, agentwar3_loadmessages.cpp),
//  except the second and third loops here are additionally gated on the
//  stream's own version (>= 0x178C) where Save writes them unconditionally.
//  Then a long, mechanical run of field reads and two sub-object ::Load
//  calls (CSelectionWar3, CTechTree) in the object's own address order,
//  the same "flat field walk" shape Save already established, reading off
//  the wire instead of writing to it - and, at the very end, six
//  individually version-gated single dwords (playerwar3_loadversiongate.cpp
//  fold: written out here per field, the same repeated-inline shape
//  item_load.cpp's own ENSURE_GATE macro already documents for this exact
//  ".\\...\\SaveVersion.h"-tagged idiom) that Save writes unconditionally.
//
//  Own translation unit: every call in it (CAgentWar3::Load, the six new
//  per-field readers in playerwar3_loadhelpers.cpp, CSelectionWar3::Load
//  and CTechTree::Load) is real, save the one thunk
//  playerwar3_selectionwar3loadthunk.cpp's own header explains.
//============================================================================
#include "playerwar3.h"
#include "cdatastore.h"
#include "integerlistener.h"
#include "versiongate.h"
#include "CFloat.h"
#include "jassnatives.h"   // STStringField
#include "storm.h"   // placement new

//  0x6F6EED50 - Net/cdatastore_readcfloat.cpp.
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);
//  Storm.dll ordinal 401.
void* __stdcall SMemAlloc(unsigned int size, const char* logfilename,
                          int logline, unsigned int flags);

//  Slot 21 (+0x54), reached through the vtable rather than by name - the
//  same base-class body agentwar3_loadmessages.cpp's own call site
//  declares.
typedef void (__thiscall *Slot0x54Fn)(void* self, CDataStore* store,
                                      unsigned int msgId, int count);

//  playerwar3_loadhelpers.cpp.
CDataStore* __fastcall ReadAllySlotRef(CDataStore* store, SCheckedAllySlot* out);
CDataStore* __fastcall ReadFixedIntMiniArray(CDataStore* store,
                                             unsigned int* countField);
CDataStore* __fastcall ReadIntegerListenerField(CDataStore* store, void** out);
CDataStore* __fastcall ReadDeadHeroArray(CDataStore* store,
                                         TSGrowableArray<SCheckedUnitSlot>* array);
struct PlayerWar3IntMiniArrayHeader;
CDataStore* __fastcall ReadIntMiniArray(CDataStore* store,
                                        PlayerWar3IntMiniArrayHeader* header);
CDataStore* __fastcall ReadBestHeroDataArray(CDataStore* store,
                                             TSFixedArray<BestHeroData>* header);

//  The allocation tag the six trailing version gates share (all six inline
//  instantiations of the same header helper item_load.cpp's own
//  ENSURE_GATE macro already names) - `.\...\SaveVersion.h`, at its own
//  line 0x9D, exactly as every other gate in this codebase carries it.
static const char kGateHeader[] =
    "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\Engine\\SaveVersion.h";

//  Five of the six trailing gates share one static (dword_6FAB6628); the
//  +0x328 gate alone uses a second (dword_6FAB6618) - not a guess, the
//  shipped stream really does write two distinct globals, matching
//  item_load.cpp's own note that a static is shared per *helper instance*
//  the compiler actually emitted, not per call site.
static SVersionGate* g_dwordGateA = 0;
static SVersionGate* g_dwordGateB = 0;

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

//  Read one version-gated trailing dword through `gate`: in range with a
//  live target, read straight into it; past the upper bound (never true
//  here - every one of these six gates' own `until` is -1), consume and
//  discard instead; below `since`, do nothing at all.  Written out at each
//  of the six call sites below rather than as a callable helper - the same
//  choice item_load.cpp's own dword/dwordAlt gate blocks make, matching
//  what the shipped stream actually is: this exact seven-line shape
//  inlined six times out of a header, not six calls to one out-of-line
//  function.
#define READ_VERSION_GATED_DWORD(gate)                                     \
    do {                                                                    \
        int atLeastSince = (store->GetVersion() >= (gate)->m_since);        \
        int atMostUntil = ((gate)->m_until >= store->GetVersion());         \
        void* target = (gate)->m_target;                                    \
        if (atLeastSince && atMostUntil && target != 0)                     \
        {                                                                   \
            store->ReadDword(&fieldTmp);                                    \
            *(unsigned int*)target = fieldTmp;                              \
        }                                                                   \
        else if (!atMostUntil)                                              \
        {                                                                   \
            store->ReadDword(&fieldTmp);                                    \
        }                                                                   \
    } while (0)

void CPlayerWar3::Load(CDataStore* store)
{
    CAgentWar3::Load(store);

    for (unsigned int msgId = 0x8020B; msgId < 0x80234; msgId++)
        ((Slot0x54Fn)(*(void***)this)[0x54 / 4])(this, store, msgId, 0x400);

    if (store->GetVersion() >= 0x178C)
    {
        for (unsigned int msgId = 0x80305; msgId < 0x8030D; msgId++)
            ((Slot0x54Fn)(*(void***)this)[0x54 / 4])(this, store, msgId, 0x400);
        for (unsigned int msgId = 0x8030D; msgId < 0x80319; msgId++)
            ((Slot0x54Fn)(*(void***)this)[0x54 / 4])(this, store, msgId, 0x400);
    }

    ((Slot0x54Fn)(*(void***)this)[0x54 / 4])(this, store, 0x80260, 0x400);
    ((Slot0x54Fn)(*(void***)this)[0x54 / 4])(this, store, 0x80261, 0x400);

    //  +0x24: RCString.  Kept as raw bytes in the header (see its own
    //  comment) - STStringField::Assign (0x6F4C5CF0, rcstringassigncstr.cpp)
    //  is called only when the read text is non-empty; an empty stream
    //  leaves the field exactly as it was, unlike Save's own empty-string
    //  fallback.
    char text[0x800];
    store->ReadString(text, 0x800);
    if (text[0] != 0)
        ((STStringField*)m_valueNameStorage)->Assign(text);

    store->ReadByte(&m_flag30);
    //  The load-side field this session's Save work never surfaced: the
    //  just-read flag is also propagated into the CSelectionWar3
    //  sub-object's own +0x1AC (a field this class's own opaque treatment
    //  leaves unnamed - see playerwar3.h's own class comment).
    *(unsigned int*)((char*)m_selectionWar3 + 0x1AC) = (unsigned int)m_flag30;

    m_selectionWar3->Load(store);

    ReadAllySlotRef(store, (SCheckedAllySlot*)m_allySlotStorage);

    ReadFixedIntMiniArray(store, &m_dumpArrayACount);
    ReadFixedIntMiniArray(store, &m_dumpArrayBCount);

    //  +0x248..+0x278: twelve individual SIntMiniValue fields, matching
    //  Save's own write order exactly (including the +0x26C field Save
    //  skips here and reads again in its own trailing block below).  Read
    //  into one shared local and copied into each field in turn - the
    //  shipped stream reuses a single stack slot for all twelve
    //  (`lea eax,[esp+N]` / `call ReadDword` / `mov [field],eax`) rather
    //  than reading straight into each field's own address.
    unsigned int fieldTmp;
    store->ReadDword(&fieldTmp); m_reserved248.m_value = fieldTmp;
    store->ReadDword(&fieldTmp); m_reserved24C.m_value = fieldTmp;
    store->ReadDword(&fieldTmp); m_reserved250.m_value = fieldTmp;
    store->ReadDword(&fieldTmp); m_reserved254.m_value = fieldTmp;
    store->ReadDword(&fieldTmp); m_reserved258.m_value = fieldTmp;
    store->ReadDword(&fieldTmp); m_reserved25C.m_value = fieldTmp;
    store->ReadDword(&fieldTmp); m_reserved260.m_value = fieldTmp;
    store->ReadDword(&fieldTmp); m_reserved264.m_value = fieldTmp;
    store->ReadDword(&fieldTmp); m_reserved268.m_value = fieldTmp;
    store->ReadDword(&fieldTmp); m_reserved270.m_value = fieldTmp;
    store->ReadDword(&fieldTmp); m_reserved274.m_value = fieldTmp;
    store->ReadDword(&fieldTmp); m_reserved278.m_value = fieldTmp;

    //  +0x27C..+0x294: seven IntegerListener* fields.
    ReadIntegerListenerField(store, &m_reserved27C);
    ReadIntegerListenerField(store, &m_reserved280);
    ReadIntegerListenerField(store, &m_reserved284);
    ReadIntegerListenerField(store, (void**)((char*)this + 0x288));
    ReadIntegerListenerField(store, (void**)((char*)this + 0x28C));
    ReadIntegerListenerField(store, &m_reserved290);
    ReadIntegerListenerField(store, &m_reserved294);

    //  +0x298/+0x2A0: read into a scratch CFloat and *published* through
    //  FloatMini's own vtable slot 0 with flag 0 (not stored back through
    //  DumpTo/WriteCFloat the way Save reads it) - same shape item_load.cpp's
    //  own CItem::Load already uses for its FloatMiniB range.
    {
        CFloat scratch;
        ReadCFloat(store, &scratch);
        ((FloatMiniB::NotifyFn)(*(void***)&m_reserved298)[0])(
                &m_reserved298, &scratch, 0);
    }
    {
        CFloat scratch;
        ReadCFloat(store, &scratch);
        ((FloatMiniB::NotifyFn)(*(void***)&m_reserved2A0)[0])(
                &m_reserved2A0, &scratch, 0);
    }

    ReadDeadHeroArray(store, (TSGrowableArray<SCheckedUnitSlot>*)m_deadHeroArrayStorage);

    ReadIntMiniArray(store, (PlayerWar3IntMiniArrayHeader*)&m_intMiniAlloc);

    store->ReadDword(&m_reserved2CC.m_value);

    m_techTree->Load(store);

    //  +0x2D4/+0x2D8: the SRefMiniValue pair, both plain ReadDword - +0x2DC:
    //  the raw pad field right after it, read via ReadDwordAlt3 (not the
    //  ReadDwordAlt2 Save's own WriteDwordAlt2 mirrors - a real asymmetry,
    //  transcribed as read).
    {
        unsigned int t = 0xFFFFFFFF, b = 0xFFFFFFFF, pad = 0;
        store->ReadDword(&t);
        store->ReadDword(&b);
        store->ReadDwordAlt3(&pad);
        m_reserved2D4.m_t = t;
        m_reserved2D4.m_b = b;
        *(unsigned int*)((char*)this + 0x2DC) = pad;
    }

    store->ReadDword(&m_reserved2E0.m_value);

    ReadBestHeroDataArray(store, &m_bestHeroData);

    //  +0x2F0/+0x2F4/+0x2F8: same {plain, plain, Alt3} shape as +0x2D4
    //  above, again asymmetric with Save's own {plain, plain, Alt2} tail.
    {
        unsigned int a = 0xFFFFFFFF, b = 0xFFFFFFFF, c = 0;
        store->ReadDword(&a);
        store->ReadDword(&b);
        store->ReadDwordAlt3(&c);
        m_reserved2F0 = a;
        m_reserved2F4 = b;
        m_reserved2F8 = c;
    }

    //  +0x2FC: CAgentTimer, own vtable slot 9 (+0x24) - Save's own call
    //  through this same sub-object uses slot 8 (+0x20); load and save go
    //  through two different slots, matching CAgentWar3::Load/Save's own
    //  slot-21-vs-slot-20 split one level up.  Kept as a raw slot dispatch
    //  for the same reason playerwar3_save.cpp's own call is.
    typedef void (__thiscall *AgentTimerSlot9Fn)(void* self, CDataStore* store);
    void* timer = m_bestHeroTimerStorage;
    ((AgentTimerSlot9Fn)(*(void***)timer)[0x24 / 4])(timer, store);

    //  +0x31C/+0x320/+0x324: three raw dwords via ReadDwordAlt2 - Save's
    //  own WriteDwordAlt3 mirror, same asymmetry as above.
    store->ReadDwordAlt2((unsigned int*)((char*)this + 0x31C));
    store->ReadDwordAlt2((unsigned int*)((char*)this + 0x320));
    store->ReadDwordAlt2((unsigned int*)((char*)this + 0x324));

    //  +0x310: a second RCString, same shape as +0x24 above.
    {
        char text2[0x800];
        store->ReadString(text2, 0x800);
        if (text2[0] != 0)
            ((STStringField*)m_reserved310Storage)->Assign(text2);
    }

    //  Six trailing fields, each individually version-gated - Save writes
    //  all six unconditionally at the very end (+0x26C, +0x2A8, +0x32C,
    //  +0x328, +0x330, +0x334 in that order); Load reads them back in a
    //  different order (+0x26C, +0x2A8, +0x32C, +0x330, +0x328, +0x334)
    //  with a real version window apiece, matching a save-format history
    //  where these six fields were added one at a time.
    ENSURE_GATE(g_dwordGateA);
    g_dwordGateA->m_since = 0x11DC;
    g_dwordGateA->m_until = -1;
    g_dwordGateA->m_target = &m_reserved26C.m_value;
    READ_VERSION_GATED_DWORD(g_dwordGateA);

    ENSURE_GATE(g_dwordGateA);
    g_dwordGateA->m_since = 0x1770;
    g_dwordGateA->m_until = -1;
    g_dwordGateA->m_target = &m_reserved2A8.m_value;
    READ_VERSION_GATED_DWORD(g_dwordGateA);

    ENSURE_GATE(g_dwordGateA);
    g_dwordGateA->m_since = 0x1770;
    g_dwordGateA->m_until = -1;
    g_dwordGateA->m_target = &m_reserved32C.m_value;
    READ_VERSION_GATED_DWORD(g_dwordGateA);

    ENSURE_GATE(g_dwordGateA);
    g_dwordGateA->m_since = 0x1770;
    g_dwordGateA->m_until = -1;
    g_dwordGateA->m_target = &m_reserved330.m_value;
    READ_VERSION_GATED_DWORD(g_dwordGateA);

    ENSURE_GATE(g_dwordGateB);
    g_dwordGateB->m_since = 0x1788;
    g_dwordGateB->m_until = -1;
    g_dwordGateB->m_target = (char*)this + 0x328;
    READ_VERSION_GATED_DWORD(g_dwordGateB);

    ENSURE_GATE(g_dwordGateA);
    g_dwordGateA->m_since = 0x179F;
    g_dwordGateA->m_until = -1;
    g_dwordGateA->m_target = &m_reserved334.m_value;
    READ_VERSION_GATED_DWORD(g_dwordGateA);
}
