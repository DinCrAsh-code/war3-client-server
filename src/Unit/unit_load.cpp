//============================================================================
//  0x6F290850 - CUnit's vtable slot 15 (+0x3C), Load.  See unit.h.
//
//  CSelectable::Load(store) first, then CUnit's own record, in the same
//  field order CUnit::Save (unit_save.cpp) writes it - but Load is *not*
//  Save's mechanical mirror.  Six real, dump-confirmed asymmetries:
//
//   * the announcement loops go through CAgent's own slot 21 (+0x54,
//     Method_0x54, agent_slot21.cpp) rather than Save's slot 20 (+0x50,
//     Method_0x50) - a distinct virtual method, not a coincidence of
//     offsets, and the second loop (0x80319..0x80329) is now gated on
//     `store->GetVersion() >= 0x1785` where Save wrote it unconditionally;
//   * the two embedded CAgentTimers (m_timer7C, m_timer180) dispatch
//     through their own vtable slot 9 (+0x24) here, not Save's slot 8
//     (+0x20) - CAgentTimer's own Save/Load pair, reached this way for the
//     first time (see unit_save.cpp's own note on slot 8);
//   * every "handle pair plus one trailing int" triple this function reads
//     back reads the trailing int through ReadDwordAlt3, where Save wrote
//     the same field through WriteDwordAlt2 - a real, systematic
//     read/write asymmetry, confirmed at every one of the eight occurrences
//     and not a transcription slip in one of them.  Written out by hand
//     rather than through the shared ReadHandleTriple helper
//     (handletriple.h, item_load.cpp's own module): calling that here
//     compiles to a real `call`, where the shipped listing has this same
//     three-read, sentinel-prefilled shape fully inlined at every site, so
//     CUnit::Load evidently carries its own copy rather than sharing
//     item_load.cpp's across translation units.  The sentinel prefill
//     order matches ReadHandleTriple's own body exactly - typeTag set to
//     -1 before handle, extra left 0, all three overwritten by the reads
//     that follow;
//   * four of the `void*` fields Save serialises identically through its
//     own inline SaveRefHandlePair (m_pRefA8, m_pRefB4, m_pRefD8, m_pRef110)
//     turn out to be two *distinct* live types on load: m_pRefA8 and
//     m_ref12C.m_p resolve through ReadFloatListenerRef (0x6F05F630,
//     floatlistenerrefread.cpp), while m_pRefB4, m_pRefD8, m_pRef110 and
//     m_ref128.m_p/m_ref224.m_p resolve through ReadFloatModifierRef
//     (0x6F053640, floatpropmodifierrefread.cpp) - real evidence for what
//     these fields hold that Save's uniform {handle,type} serialisation
//     could not distinguish;
//   * every FloatMini this function touches (m_fltAC, m_fltD0, m_fltDC,
//     m_fltC8, and the FloatMiniB at m_fltB204) is read into a scratch
//     CFloat and *published* through FloatMini's own vtable slot 0 with
//     flag 0 (the same NotifyFn floatmini.h already names, and the same
//     shape unit_fieldgroups.cpp's own FloatMiniB::SetPair call uses) -
//     never stored back into the field directly, the same "publish, don't
//     just store" idiom item_load.cpp's own FloatMiniB field already
//     established;
//   * eleven fields Save always writes are read back only when the stream
//     is new enough - see "Version gates" below - and one more (the low
//     byte of +0x280) is read and unconditionally discarded, the same
//     "read to keep the stream aligned, then ignore it" idiom
//     widget_load.cpp's own +0x38 flag already uses (the authoritative
//     value for +0x280 arrives later, gated, as the full dword).
//
//  Version gates
//  -------------
//  Ten fields are wrapped in the inlined LoadVersionedObject-for-a-scalar
//  shape item_load.cpp's own header (versiongate.h) already documents: a
//  file-static SVersionGate*, lazily allocated once through Storm and then
//  overwritten on every use, naming a `since` version (`until` is always -1
//  here - "from this version on, forever") and a target address. Whichever
//  reader is called determines *which* static gets reused - the four
//  distinct readers below correspond to the four distinct statics the dump
//  shows (`dword_6FAB6628/660C/661C/6610`), each shared by every field that
//  reads through it and none of the others, exactly as item_load.cpp's own
//  comment says two CAgentTimer loads there share one static because they
//  go through the same helper. The `since` values themselves are a save-
//  format version counter, not a build number:
//
//      since   field    reader
//      0x1782  +0x1C0   ReadDword        (g_dwordGate)
//      0x1782  +0x1CC   ReadDword        (g_dwordGate)
//      0x1780  +0x27C   ReadDwordAlt2    (g_dwordAlt2Gate)
//      0x1788  +0x300   ReadFloat        (g_floatGate)
//      0x1771  +0x158   ReadDword        (g_dwordGate)
//      0x1771  +0x15C   ReadDword        (g_dwordGate)
//      0x1778  +0x160   ReadDword        (g_dwordGate)
//      0x1779  +0x60    ReadDword        (g_dwordGate)
//      0x1779  +0x1D0   ReadDword        (g_dwordGate)
//      0x1784  +0x280   ReadDwordAlt     (g_dwordAltGate)
//
//  A field whose gate is not satisfied and the stream is *older* than
//  `since` is skipped outright - the target is simply never written, same
//  as LoadVersionedObject's own "before since" outcome.  `until` being a
//  fixed -1 means the "past the upper bound, consume and discard" branch is
//  dead code in this build (there is no upper bound any of these ten fields
//  were ever retired at), but it is transcribed anyway because the shipped
//  listing carries it and it costs nothing to keep.
//
//  Also new here and load-only: CUnit::ResetActivityTimestamps()
//  (0x6F26D6F0, unit_activitytimestamps.cpp) runs once at the very end,
//  stamping the +0x90/+0x94 activity timestamps to "two minutes ago" - Save
//  never touches that pair at all.
//
//  Own translation unit: CSelectable::Load, CAgent::Method_0x54, the
//  CAgentTimer slot-9 dispatch, the four typed reference readers, the
//  CDataStore reader cluster and FloatMini's own vtable slot 0, all already
//  reconstructed elsewhere.
//============================================================================
#include "unit.h"
#include "cdatastore.h"
#include "floatmini.h"
#include "floatlistener.h"
#include "floatpropmodifier.h"
#include "versiongate.h"
#include "storm.h"   // placement new

CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);
//  0x6F05F630/0x6F053640 - floatlistenerrefread.cpp/floatpropmodifierrefread.cpp.
CDataStore* __fastcall ReadFloatListenerRef(CDataStore* store, FloatListener** slot);
CDataStore* __fastcall ReadFloatModifierRef(CDataStore* store, FloatModifier** slot);
//  0x6F05F3A0 - widgetpathablerefread.cpp.
CDataStore* __fastcall ReadWidgetPathableRef(CDataStore* store, CAgent** slot);

//  Storm.dll ordinal 401.
void* __stdcall SMemAlloc(unsigned int size, const char* logfilename,
                          int logline, unsigned int flags);

//  CAgentTimer's own vtable slot 9 (+0x24) - the load-time twin of slot 8
//  (+0x20, unit_save.cpp's own AgentTimerSaveFn).  Same hand-typed dispatch
//  for the same reason: nothing in this repo has reached CAgentTimer's
//  other six unmodelled slots yet, so widening agenttimer.h to a `virtual`
//  would have to invent placeholders for all of them just to keep this
//  slot's offset right.
typedef void (__thiscall *AgentTimerLoadFn)(void*, CDataStore*);

//  FloatMini's own vtable slot 0 (floatmini.h's own NotifyFn, reached the
//  same way here as unit_fieldgroups.cpp's FloatMiniB::SetPair already
//  does): publish a freshly-read value rather than storing it back
//  directly.
typedef void (__thiscall *FloatMiniNotifyFn)(void*, CFloat*, int);

//  The four version-gate statics, one per distinct reader - see the file
//  header.  Truncated identically in the dump (aEDrive1TempBui_92) to
//  several other, textually different strings in this module's
//  neighbourhood; the byte content does not affect the score (an
//  unresolved string symbol canonicalises to SYM regardless - see
//  cdatastore_writerefhandlepair.cpp's own neighbours), so this is written
//  out as a plain, distinctly-named tag rather than guessed at byte for
//  byte.
static const char kGateAllocTag[] =
    "e:\\drive1\\temp\\buildwar3x\\war3\\source\\engine\\saveversion.h";

static SVersionGate* g_dwordGate = 0;       // dword_6FAB6628
static SVersionGate* g_dwordAlt2Gate = 0;   // dword_6FAB660C
static SVersionGate* g_floatGate = 0;       // dword_6FAB661C
static SVersionGate* g_dwordAltGate = 0;    // dword_6FAB6610

//  Raw field stores, not a placement-new constructor call: every field is
//  overwritten immediately after by the macro that calls this anyway (the
//  same reasoning item_load.cpp's own ENSURE_GATE gives), and the shipped
//  listing itself never calls out to a separate constructor here - just
//  the Storm allocation and three stores.
#define ENSURE_GATE(slot)                                                  \
    do {                                                                   \
        if ((slot) == 0)                                                   \
        {                                                                  \
            void* raw = SMemAlloc(12, kGateAllocTag, 0x9D, 0);             \
            if (raw != 0)                                                  \
            {                                                              \
                (slot) = (SVersionGate*)raw;                               \
                (slot)->m_since = 0;                                       \
                (slot)->m_until = -1;                                      \
                (slot)->m_target = 0;                                      \
            }                                                              \
            else                                                           \
                (slot) = 0;                                                \
        }                                                                  \
    } while (0)

//  One block per gated field - see the table in the file header.  Each
//  reader gets its own macro so the call inside stays a real, direct call
//  to that reader rather than an indirect call through a function pointer
//  (the shipped listing never has one here).
#define LOAD_GATED_DWORD(gateVar, sinceVal, targetPtr)                     \
    do {                                                                   \
        ENSURE_GATE(gateVar);                                              \
        (gateVar)->m_since = (sinceVal);                                   \
        (gateVar)->m_until = -1;                                           \
        (gateVar)->m_target = (targetPtr);                                 \
        int atLeastSince = (store->GetVersion() >= (gateVar)->m_since);    \
        int atMostUntil = ((gateVar)->m_until >= store->GetVersion());     \
        void* target = (gateVar)->m_target;                                \
        if (atLeastSince && atMostUntil && target != 0)                    \
            store->ReadDword((unsigned int*)target);                       \
        else if (!atMostUntil)                                             \
        {                                                                  \
            unsigned int scratch;                                          \
            store->ReadDword(&scratch);                                    \
        }                                                                  \
    } while (0)

#define LOAD_GATED_DWORDALT2(gateVar, sinceVal, targetPtr)                 \
    do {                                                                   \
        ENSURE_GATE(gateVar);                                              \
        (gateVar)->m_since = (sinceVal);                                   \
        (gateVar)->m_until = -1;                                           \
        (gateVar)->m_target = (targetPtr);                                 \
        int atLeastSince = (store->GetVersion() >= (gateVar)->m_since);    \
        int atMostUntil = ((gateVar)->m_until >= store->GetVersion());     \
        void* target = (gateVar)->m_target;                                \
        if (atLeastSince && atMostUntil && target != 0)                    \
            store->ReadDwordAlt2((unsigned int*)target);                   \
        else if (!atMostUntil)                                             \
        {                                                                  \
            unsigned int scratch;                                          \
            store->ReadDwordAlt2(&scratch);                                \
        }                                                                  \
    } while (0)

#define LOAD_GATED_DWORDALT(gateVar, sinceVal, targetPtr)                  \
    do {                                                                   \
        ENSURE_GATE(gateVar);                                              \
        (gateVar)->m_since = (sinceVal);                                   \
        (gateVar)->m_until = -1;                                           \
        (gateVar)->m_target = (targetPtr);                                 \
        int atLeastSince = (store->GetVersion() >= (gateVar)->m_since);    \
        int atMostUntil = ((gateVar)->m_until >= store->GetVersion());     \
        void* target = (gateVar)->m_target;                                \
        if (atLeastSince && atMostUntil && target != 0)                    \
            store->ReadDwordAlt((unsigned int*)target);                    \
        else if (!atMostUntil)                                             \
        {                                                                  \
            unsigned int scratch;                                          \
            store->ReadDwordAlt(&scratch);                                 \
        }                                                                  \
    } while (0)

#define LOAD_GATED_FLOAT(gateVar, sinceVal, targetPtr)                     \
    do {                                                                   \
        ENSURE_GATE(gateVar);                                              \
        (gateVar)->m_since = (sinceVal);                                   \
        (gateVar)->m_until = -1;                                           \
        (gateVar)->m_target = (targetPtr);                                 \
        int atLeastSince = (store->GetVersion() >= (gateVar)->m_since);    \
        int atMostUntil = ((gateVar)->m_until >= store->GetVersion());     \
        void* target = (gateVar)->m_target;                                \
        if (atLeastSince && atMostUntil && target != 0)                    \
            store->ReadFloat((float*)target);                             \
        else if (!atMostUntil)                                             \
        {                                                                  \
            float scratch;                                                 \
            store->ReadFloat(&scratch);                                    \
        }                                                                  \
    } while (0)

//  Publish a freshly-read value through a FloatMini/FloatMiniB sub-object's
//  own vtable slot 0 - see the FloatMiniNotifyFn note above.
static void PublishFloatMini(void* obj, CFloat* value)
{
    ((FloatMiniNotifyFn)(*(void***)obj)[0])(obj, value, 0);
}

void CUnit::Load(CDataStore* store)
{
    //  The shared -1/0 sentinels every handle-triple prefill below reuses -
    //  matching the shipped listing's own single `or ebp,-1`/`xor ebx,ebx`
    //  held across the whole function, the same idiom unit_save.cpp's own
    //  ebp/ebx pair uses for its write-side sentinels.
    const unsigned int kMinusOne = (unsigned int)-1;
    const unsigned int kZero = 0;

    CSelectable::Load(store);

    for (unsigned int id = 0x80234; id < 0x80259; ++id)
        Method_0x54(store, id, 0x400);

    if (store->GetVersion() >= 0x1785)
    {
        for (unsigned int id = 0x80319; id < 0x8032A; ++id)
            Method_0x54(store, id, 0x400);
    }

    ((AgentTimerLoadFn)(*(void***)&m_timer7C)[0x24 / 4])(&m_timer7C, store);

    //  +0x130/+0x134 (m_ref130) and +0x138 (m_int138).
    {
        unsigned int typeTag = kMinusOne;
        unsigned int handle = kMinusOne;
        unsigned int extra = kZero;
        store->ReadDword(&handle);
        store->ReadDword(&typeTag);
        store->ReadDwordAlt3(&extra);
        m_ref130.m_t = handle;
        m_ref130.m_b = typeTag;
        m_int138 = extra;
    }
    //  +0x13C/+0x140 (m_ref13C) and both dwords of the +0x144 reserved
    //  gap - the one group with two trailing words, matching Save's own
    //  note on it.  The second (+0x148) reads through ReadDwordAlt2, not
    //  ReadDwordAlt3 like the first - Save's own asymmetry the other way
    //  round (Alt2 first, Alt3 second) confirms this is a real, dump-
    //  transcribed difference and not a copy/paste slip.
    {
        unsigned int typeTag = kMinusOne;
        unsigned int handle = kMinusOne;
        unsigned int extra = kZero;
        store->ReadDword(&handle);
        store->ReadDword(&typeTag);
        store->ReadDwordAlt3(&extra);
        m_ref13C.m_t = handle;
        m_ref13C.m_b = typeTag;
        *(unsigned int*)(m_reserved144 + 0) = extra;
        store->ReadDwordAlt2((unsigned int*)(m_reserved144 + 4));
    }

    //  +0x110 - a FloatModifier reference, not the generic refcounted
    //  pointer Save's own SaveRefHandlePair treats it as; see the file
    //  header.
    ReadFloatModifierRef(store, (FloatModifier**)&m_pRef110);

    //  +0x10C - only the low word, matching Save's own WriteWord.
    {
        unsigned short lo = 0;
        store->ReadWord(&lo);
        m_int10C = lo;
    }

    //  +0x174/+0x178 (m_ref174) and +0x17C (m_int17C).
    {
        unsigned int typeTag = kMinusOne;
        unsigned int handle = kMinusOne;
        unsigned int extra = kZero;
        store->ReadDword(&handle);
        store->ReadDword(&typeTag);
        store->ReadDwordAlt3(&extra);
        m_ref174.m_t = handle;
        m_ref174.m_b = typeTag;
        m_int17C = extra;
    }

    ((AgentTimerLoadFn)(*(void***)&m_timer180)[0x24 / 4])(&m_timer180, store);

    //  +0x19C/+0x1A0 (m_ref19C) and +0x1A4 (m_int1A4).
    {
        unsigned int typeTag = kMinusOne;
        unsigned int handle = kMinusOne;
        unsigned int extra = kZero;
        store->ReadDword(&handle);
        store->ReadDword(&typeTag);
        store->ReadDwordAlt3(&extra);
        m_ref19C.m_t = handle;
        m_ref19C.m_b = typeTag;
        m_int1A4 = extra;
    }
    //  +0x1A8/+0x1AC (m_ref1A8) and +0x1B0 (m_int1B0).
    {
        unsigned int typeTag = kMinusOne;
        unsigned int handle = kMinusOne;
        unsigned int extra = kZero;
        store->ReadDword(&handle);
        store->ReadDword(&typeTag);
        store->ReadDwordAlt3(&extra);
        m_ref1A8.m_t = handle;
        m_ref1A8.m_b = typeTag;
        m_int1B0 = extra;
    }
    //  +0x1DC/+0x1E0 (m_ref1DC) and the first dword of +0x1E4.
    {
        unsigned int typeTag = kMinusOne;
        unsigned int handle = kMinusOne;
        unsigned int extra = kZero;
        store->ReadDword(&handle);
        store->ReadDword(&typeTag);
        store->ReadDwordAlt3(&extra);
        m_ref1DC.m_t = handle;
        m_ref1DC.m_b = typeTag;
        *(unsigned int*)(m_reserved1E4 + 0) = extra;
    }

    //  Four back-to-back triples with no field behind any of them, reading
    //  and discarding the same three-word shape every time - the load-time
    //  twin of Save's own four placeholder {-1,-1,0} writes.  See
    //  unit_save.cpp's own note on what these most likely are.
    for (int slot = 0; slot < 4; ++slot)
    {
        unsigned int typeTag = kMinusOne;
        unsigned int handle = kMinusOne;
        unsigned int extra = kZero;
        store->ReadDword(&handle);
        store->ReadDword(&typeTag);
        store->ReadDwordAlt3(&extra);
    }

    //  +0x250/+0x254 (m_ref250) and +0x258 (m_int258).
    {
        unsigned int typeTag = kMinusOne;
        unsigned int handle = kMinusOne;
        unsigned int extra = kZero;
        store->ReadDword(&handle);
        store->ReadDword(&typeTag);
        store->ReadDwordAlt3(&extra);
        m_ref250.m_t = handle;
        m_ref250.m_b = typeTag;
        m_int258 = extra;
    }
    //  +0x304/+0x308 (m_ref304) and +0x30C, one dword past the class's own
    //  believed end - see unit_save.cpp's own note on it.
    {
        unsigned int typeTag = kMinusOne;
        unsigned int handle = kMinusOne;
        unsigned int extra = kZero;
        store->ReadDword(&handle);
        store->ReadDword(&typeTag);
        store->ReadDwordAlt3(&extra);
        m_ref304.m_t = handle;
        m_ref304.m_b = typeTag;
        *(unsigned int*)((char*)this + 0x30C) = extra;
    }

    //  +0xA8 (m_pRefA8, a FloatListener reference) and +0xB0 (the FloatMini
    //  at +0xAC, published rather than stored - see the file header).
    ReadFloatListenerRef(store, (FloatListener**)&m_pRefA8);
    {
        unsigned int val;
        ReadCFloat(store, (CFloat*)&val);
        PublishFloatMini(m_fltAC, (CFloat*)&val);
    }
    //  +0xB4 (m_pRefB4, FloatModifier) and +0xD4 (the FloatMini at +0xD0).
    ReadFloatModifierRef(store, (FloatModifier**)&m_pRefB4);
    {
        unsigned int val;
        ReadCFloat(store, (CFloat*)&val);
        PublishFloatMini(m_fltD0, (CFloat*)&val);
    }
    //  +0xD8 (m_pRefD8, FloatModifier) and +0xE0 (the FloatMini at +0xDC).
    ReadFloatModifierRef(store, (FloatModifier**)&m_pRefD8);
    {
        unsigned int val;
        ReadCFloat(store, (CFloat*)&val);
        PublishFloatMini(m_fltDC, (CFloat*)&val);
    }
    //  +0xCC - the FloatMini at +0xC8, on its own.
    {
        unsigned int val;
        ReadCFloat(store, (CFloat*)&val);
        PublishFloatMini(m_fltC8, (CFloat*)&val);
    }

    //  +0x58, +0x5C, +0x64.
    store->ReadDword(&m_owningPlayer58);
    store->ReadDword(&m_flags5C);
    store->ReadDword(&m_int64);
    //  +0xE8, +0xEC.
    store->ReadDword(&m_intE8);
    store->ReadDword(&m_intEC);
    //  +0x54.
    store->ReadDword(&m_int54);
    //  +0x114.
    store->ReadDword(&m_int114);
    //  +0x150, +0x154.
    store->ReadDword(&m_int150);
    store->ReadDword(&m_int154);
    //  +0x198.
    store->ReadDword(&m_int198);
    //  +0x1B8, +0x1BC - both unconditional; +0x1C0 is version-gated.
    store->ReadDword(&m_int1B8);
    store->ReadDword(&m_int1BC);
    LOAD_GATED_DWORD(g_dwordGate, 0x1782, &m_int1C0);
    //  +0x1C4, +0x1C8 unconditional; +0x1CC is version-gated.
    store->ReadDword(&m_int1C4);
    store->ReadDword(&m_int1C8);
    LOAD_GATED_DWORD(g_dwordGate, 0x1782, &m_int1CC);
    //  +0x1D4, +0x1D8.
    store->ReadDword(&m_int1D4);
    store->ReadDword(&m_int1D8);
    //  +0x194.
    store->ReadDword(&m_int194);

    //  +0x1FC, +0x200.
    store->ReadDword(&m_int1FC);
    store->ReadDword(&m_int200);
    //  +0x208/+0x20C/+0x210 - the FloatMiniB at +0x204: val is read into a
    //  scratch and published through its own vtable slot 0 with flag 0
    //  (not through SetPair, which would take a {lo,hi} pair and publish
    //  with flag 1); min (+0x20C) and max (+0x210, m_bound210) are each
    //  read directly into the field, the same "direct address, no copy"
    //  shape Save's own note on this triple already records.
    {
        unsigned int val;
        ReadCFloat(store, (CFloat*)&val);
        ReadCFloat(store, (CFloat*)(m_fltB204 + 8));
        ReadCFloat(store, &m_bound210);
        PublishFloatMini(m_fltB204, (CFloat*)&val);
    }

    //  +0x248, +0x24C, +0xE4 (with a version fixup - see below), +0x240,
    //  +0x244 - not in offset order, matching Save's own order exactly.
    store->ReadDword(&m_int248);
    store->ReadDword(&m_int24C);
    store->ReadDword(&m_intE4);
    //  A save-format compatibility fixup nothing in Save has a counterpart
    //  for: streams older than 0x1770 stored +0xE4 one higher than this
    //  build's own convention, so a nonzero value read from an old stream
    //  is nudged back down by one.
    if (store->GetVersion() < 0x1770 && m_intE4 != 0)
        m_intE4 += (unsigned int)-1;
    store->ReadDword(&m_int240);
    store->ReadDword(&m_int244);

    //  +0x128 (m_ref128.m_p, FloatModifier), +0x12C (m_ref12C.m_p,
    //  FloatListener), +0x224 (m_ref224.m_p, FloatModifier) - three of the
    //  fields Save's own uniform SaveRefHandlePair could not tell apart;
    //  see the file header.
    ReadFloatModifierRef(store, (FloatModifier**)&m_ref128.m_p);
    ReadFloatListenerRef(store, (FloatListener**)&m_ref12C.m_p);
    ReadFloatModifierRef(store, (FloatModifier**)&m_ref224.m_p);

    //  +0x228, and +0x230/+0x234/+0x238 - four CFloats out of the +0x228
    //  reserved gap, the same four offsets Save writes.
    ReadCFloat(store, (CFloat*)(m_reserved228 + 0x00));
    ReadCFloat(store, (CFloat*)(m_reserved228 + 0x08));
    ReadCFloat(store, (CFloat*)(m_reserved228 + 0x0C));
    ReadCFloat(store, (CFloat*)(m_reserved228 + 0x10));

    //  +0x26C, +0x270 - two flag bytes out of the +0x264 reserved gap.
    {
        unsigned char b0 = 0, b1 = 0;
        store->ReadByte(&b0);
        *(unsigned int*)(m_reserved264 + 0x08) = b0;
        store->ReadByte(&b1);
        *(unsigned int*)(m_reserved264 + 0x0C) = b1;
    }
    //  +0x27C - version-gated (since 0x1780), through ReadDwordAlt2 where
    //  Save wrote it through WriteDwordAlt3.
    LOAD_GATED_DWORDALT2(g_dwordAlt2Gate, 0x1780, &m_int27C);

    //  +0x2B8/+0x2BC/+0x2C0 - ClearMotionState's own three floats, as
    //  hardware floats, unconditional.
    store->ReadFloat(&m_motion2B8);
    store->ReadFloat(&m_motion2BC);
    store->ReadFloat(&m_motion2C0);

    //  +0x2C4 - resolved through the type-checked widget slot, not a plain
    //  {handle,type} read-back; see widgetpathablerefread.cpp.
    ReadWidgetPathableRef(store, (CAgent**)&m_pRef2C4);

    //  +0x280 - only bit 0, read and unconditionally *discarded*: the
    //  authoritative value for this field arrives later, version-gated, as
    //  the full dword.  Same "read to keep the stream aligned, then
    //  ignore it" idiom as CWidget::Load's own +0x38 flag (widget_load.cpp).
    {
        unsigned char discarded = 0;
        store->ReadByte(&discarded);
    }

    //  +0x2D4, +0x2D8.
    store->ReadDword(&m_int2D4);
    store->ReadDwordAlt(&m_animMode2D8);

    //  +0x2E0..+0x2F4 - six consecutive dwords, a real loop (matching
    //  Save's own note that the loop shape itself is part of what has to
    //  match here too), through ReadDwordAlt2 where Save wrote the same
    //  run through WriteDwordAlt3.
    {
        unsigned int* p = (unsigned int*)((char*)this + 0x2E0);
        for (int i = 0; i < 6; ++i)
            store->ReadDwordAlt2(&p[i]);
    }

    //  +0x2F8, +0x2FC unconditional; +0x300 is version-gated (since
    //  0x1788).
    store->ReadFloat((float*)((char*)this + 0x2F8));
    store->ReadFloat((float*)((char*)this + 0x2FC));
    LOAD_GATED_FLOAT(g_floatGate, 0x1788, (char*)this + 0x300);

    //  +0x158, +0x15C, +0x160 - each individually version-gated.
    LOAD_GATED_DWORD(g_dwordGate, 0x1771, &m_int158);
    LOAD_GATED_DWORD(g_dwordGate, 0x1771, &m_int15C);
    LOAD_GATED_DWORD(g_dwordGate, 0x1778, &m_int160);
    //  +0x60 - version-gated.
    LOAD_GATED_DWORD(g_dwordGate, 0x1779, &m_int60);
    //  +0x1D0 - version-gated.
    LOAD_GATED_DWORD(g_dwordGate, 0x1779, &m_int1D0);
    //  +0x280 again, the authoritative full-dword value this time -
    //  version-gated, through ReadDwordAlt matching Save's own WriteDwordAlt
    //  for the same second write.
    LOAD_GATED_DWORDALT(g_dwordAltGate, 0x1784, &m_flags280);

    ResetActivityTimestamps();
}
