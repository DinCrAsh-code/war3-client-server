//============================================================================
//  0x6F040AE0 - CAbility::LoadFlags, slot 19 (named LoadFlags in ability.h,
//  not for what it does - CAgentWar3::LoadFlags already owns this exact
//  slot, and only an identical name+signature override lands here rather
//  than appending a brand new vtable slot; see ability.h's own comment).
//  A legacy type-id migration in truth: read-side of the
//  ability record's own compatibility layer: pulls a handful of legacy
//  fields off the save stream through seven CDataStore::ReadDword calls
//  (CAgentWar3's own m_flags, then m_field3C/m_field40/m_field44/m_field48/
//  m_field4C/m_typeId - the ability's own type rawcode, going in raw
//  before this function decides whether to rewrite it) and, only for a
//  stream older than format version 0x1770 (6000), runs a `switch` over
//  the legacy FourCC tag just read into m_typeId, replacing a handful of
//  now-retired tags with their modern equivalents so the rest of load can
//  treat every save the same way.
//
//  Every disambiguating case body compares a handful of `FloatMini`
//  scratch locals (default-constructed to g_CFloatZero) that this
//  function never writes again - none of the five the switch actually
//  reads (slot140/124/108/EC/D0) is among the ten headers the preamble
//  populates - so every Nearly/AtMost test against them really tests
//  "is this named legacy constant approximately g_CFloatZero", exactly
//  as the shipped /O2 build itself left it; transcribed literally rather
//  than folded away, per CLAUDE.md's own "check the side-by-side, don't
//  assume a difference is semantic" (here the *shipped* code is the one
//  that looks semantically odd, and this reconstruction is not the place
//  to silently "fix" it).
//
//  Own translation unit: 0x6F040xxx is its own shipped module, distinct
//  from ability_slots.cpp's 0x6F02Exxx/0x6F0522xx and from
//  ability_typemigrate_readers.cpp's own 0x6F029xxx/0x6F03Exxx readers.
//============================================================================
#include "ability.h"
#include "cdatastore.h"
#include "floatmini.h"
#include "CFloat.h"

//  ability_typemigrate_readers.cpp - the four load-side helpers this
//  function's own preamble calls.
struct SVersionedFloatMiniTriple { unsigned int m_count; FloatMini m_values[3]; };
struct SVersionedDwordTriple      { unsigned int m_count; unsigned int m_values[3]; };

extern void __fastcall ConstructThreeFloatMiniHeader(SVersionedFloatMiniTriple* dest);     // 0x6F029970
extern CDataStore* __fastcall ReadBoundedFloatMiniArray(CDataStore*, SVersionedFloatMiniTriple*); // 0x6F03EB80
extern CDataStore* __fastcall ReadBoundedDwordArray(CDataStore*, SVersionedDwordTriple*);         // 0x6F03EC10
extern CDataStore* __fastcall ReadBoundedFloatMiniBArray(CDataStore*, SVersionedFloatMiniTriple*);// 0x6F03EC90
extern void* __fastcall GetOrCreateLegacyTrackerRecord(unsigned int code, unsigned int flags, void* target); // 0x6F0384B0
extern void __fastcall NotifyLegacyMigrationDone(CDataStore* store, void* ability); // 0x6F02C740

extern int __fastcall FloatMiniNearlyEquals(const FloatMini* self, const CFloat* target);
extern int __fastcall FloatMiniDiffers(const FloatMini* self, const CFloat* target);
extern int __fastcall FloatMiniAtMost(const FloatMini* self, const CFloat* target);

//  The CFloat constants this switch's guards compare against - mostly
//  reused addresses from other subsystems' own constant pools (this
//  binary keeps one shared .rdata run of small CFloat literals rather
//  than per-module copies; docs/msvc-vc8-idioms.md), plus a dozen that
//  had no name anywhere in this codebase yet.
extern const CFloat g_slopeThreshold1;   // dword_6FAAE4F4
extern const CFloat g_slopeExpandLarge;  // dword_6FAAE504
extern const CFloat g_CFloatHalf;        // dword_6FAAE4D0
extern const CFloat g_jitterSearchStep;  // dword_6FAAE510
extern CFloat        g_unk6FAAE574;      // dword_6FAAE574 - not const at its other site
extern const CFloat g_unk6FAAE50C;
extern const CFloat g_unk6FAAE514;
extern const CFloat g_unk6FAAE518;
extern const CFloat g_unk6FAAE524;
extern const CFloat g_unk6FAAE530;
extern const CFloat g_unk6FAAE53C;
extern const CFloat g_unk6FAAE57C;
extern const CFloat g_unk6FAAE580;
extern const CFloat g_unk6FAAE584;
extern const CFloat g_unk6FAAE588;
extern const CFloat g_unk6FAAE594;
extern const CFloat g_unk6FAAE59C;
extern const CFloat g_unk6FAAE4E8;

void CAbility::LoadFlags(CDataStore* store)
{
    //  Seven plain ReadDword calls; the first lands in CAgentWar3::m_flags
    //  (agentwar3.h), inherited rather than CAbility's own.
    store->ReadDword(&m_flags);
    store->ReadDword(&m_field3C);
    store->ReadDword(&m_field40);
    store->ReadDword(&m_field44);
    store->ReadDword(&m_field48);
    store->ReadDword(&m_field4C);
    store->ReadDword(&m_typeId);

    if (store->GetVersion() >= 0x1770)
        goto notify;

    {
    SVersionedFloatMiniTriple field54;
    field54.m_count = 3;
    ConstructThreeFloatMiniHeader(&field54);

    SVersionedFloatMiniTriple slotA8 = {0}, slot144 = {0}, slot128 = {0};
    SVersionedFloatMiniTriple slot10C = {0}, slotF0 = {0}, slotD4 = {0};
    SVersionedFloatMiniTriple slot54b = {0}, slot8C = {0}, slot70 = {0};
    SVersionedDwordTriple slotB8 = {0};

    ReadBoundedFloatMiniArray(store, &slotA8);
    ReadBoundedFloatMiniArray(store, &slot144);
    ReadBoundedFloatMiniArray(store, &slot128);
    ReadBoundedFloatMiniArray(store, &slot10C);
    ReadBoundedFloatMiniArray(store, &slotF0);
    ReadBoundedFloatMiniArray(store, &slotD4);
    ReadBoundedFloatMiniArray(store, &slot54b);
    ReadBoundedFloatMiniArray(store, &slot8C);
    ReadBoundedFloatMiniArray(store, &slot70);
    ReadBoundedDwordArray(store, &slotB8);
    ReadBoundedFloatMiniBArray(store, &field54);

    //  Every gate slot the switch below reads by address: default-
    //  constructed to CFloatZero and never populated - see the header
    //  comment.
    FloatMini slot140, slot124, slot108, slotEC, slotD0;

    switch (m_typeId)
    {
    case 0x41496173:
        m_typeId = 0x41497378;
        break;

    case 0x41496162:
        if (FloatMiniNearlyEquals(&slot140, &g_slopeThreshold1) &&
            FloatMiniNearlyEquals(&slot124, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot108, &g_CFloatZero))
            m_typeId = 0x41496131;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE50C) &&
            FloatMiniNearlyEquals(&slot124, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot108, &g_CFloatZero))
            m_typeId = 0x41496133;
        if (FloatMiniNearlyEquals(&slot140, &g_jitterSearchStep) &&
            FloatMiniNearlyEquals(&slot124, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot108, &g_CFloatZero))
            m_typeId = 0x41496134;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE518) &&
            FloatMiniNearlyEquals(&slot124, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot108, &g_CFloatZero))
            m_typeId = 0x41496136;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE514) &&
            FloatMiniNearlyEquals(&slot124, &g_unk6FAAE514) &&
            FloatMiniNearlyEquals(&slot108, &g_unk6FAAE514))
            m_typeId = 0x41497835;
        if (FloatMiniNearlyEquals(&slot140, &g_slopeThreshold1) &&
            FloatMiniNearlyEquals(&slot124, &g_slopeThreshold1) &&
            FloatMiniNearlyEquals(&slot108, &g_slopeThreshold1))
            m_typeId = 0x41497831;
        if (FloatMiniNearlyEquals(&slot140, &g_slopeExpandLarge) &&
            FloatMiniNearlyEquals(&slot124, &g_slopeExpandLarge) &&
            FloatMiniNearlyEquals(&slot108, &g_slopeExpandLarge))
            m_typeId = 0x41497832;
        if (FloatMiniNearlyEquals(&slot140, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot124, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot108, &g_slopeThreshold1))
            m_typeId = 0x41497331;
        if (FloatMiniNearlyEquals(&slot140, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot124, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot108, &g_unk6FAAE50C))
            m_typeId = 0x41497333;
        if (FloatMiniNearlyEquals(&slot140, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot124, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot108, &g_jitterSearchStep))
            m_typeId = 0x41497334;
        if (FloatMiniNearlyEquals(&slot140, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot124, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot108, &g_unk6FAAE518))
            m_typeId = 0x41497336;
        if (FloatMiniNearlyEquals(&slot140, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot124, &g_slopeThreshold1) &&
            FloatMiniNearlyEquals(&slot108, &g_CFloatZero))
            m_typeId = 0x41496931;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE50C) &&
            FloatMiniNearlyEquals(&slot124, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot108, &g_CFloatZero))
            m_typeId = 0x41496933;
        if (FloatMiniNearlyEquals(&slot140, &g_jitterSearchStep) &&
            FloatMiniNearlyEquals(&slot124, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot108, &g_CFloatZero))
            m_typeId = 0x41496934;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE518) &&
            FloatMiniNearlyEquals(&slot124, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot108, &g_CFloatZero))
            m_typeId = 0x41496936;
        break;

    case 0x41496174:
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE50C))
            m_typeId = 0x41496174;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE518))
            m_typeId = 0x41497436;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE524))
            m_typeId = 0x41497439;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE530))
            m_typeId = 0x41497463;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE53C))
            m_typeId = 0x41497466;
        break;

    case 0x41496465:
        if (FloatMiniNearlyEquals(&slot140, &g_slopeThreshold1))
            m_typeId = 0x41496431;
        if (FloatMiniNearlyEquals(&slot140, &g_slopeExpandLarge))
            m_typeId = 0x41496432;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE50C))
            m_typeId = 0x41496433;
        if (FloatMiniNearlyEquals(&slot140, &g_jitterSearchStep))
            m_typeId = 0x41496434;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE514))
            m_typeId = 0x41496435;
        break;

    case 0x41496865:
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE588))
            m_typeId = 0x41496831;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE59C))
            m_typeId = 0x41496832;
        break;

    case 0x41496D61:
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE57C))
            m_typeId = 0x41496D31;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE594))
            m_typeId = 0x41496D32;
        break;

    case 0x41496D6C:
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE57C))
            m_typeId = 0x41496D62;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE594))
            m_typeId = 0x4149626D;
        break;

    case 0x41496D6D:
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE574))
            m_typeId = 0x41496C66;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE584))
            m_typeId = 0x41496C31;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE594))
            m_typeId = 0x41496C32;
        break;

    case 0x4149726D:
        if (FloatMiniNearlyEquals(&slot140, &g_CFloatHalf))
            m_typeId = 0x4149726D;
        if (FloatMiniNearlyEquals(&slot140, &g_unk6FAAE4E8))
            m_typeId = 0x4149726E;
        break;

    case 0x41497669:
    {
        CFloat converted = CFloatFromInt(0x78);
        if (FloatMiniNearlyEquals(&slotD0, &converted))
            m_typeId = 0x41497631;
        if (FloatMiniNearlyEquals(&slotD0, &g_unk6FAAE580))
            m_typeId = 0x41497632;
        break;
    }

    case 0x41617031:
        m_typeId = 0x41617031;
        break;

    case 0x41626C69:
    {
        CFloat streamVersion = CFloatFromInt(0x300);
        if (FloatMiniNearlyEquals(&slotEC, &g_CFloatZero))
        {
            if (FloatMiniAtMost(&slot108, &streamVersion))
                m_typeId = 0x41626473;
        }
        else if (FloatMiniNearlyEquals(&slotEC, &g_CFloatZero))
        {
            m_typeId = 0x4162646C;
        }
        if (FloatMiniDiffers(&slotEC, &g_CFloatZero))
        {
            if (FloatMiniAtMost(&slot108, &streamVersion))
                m_typeId = 0x41626773;
            break;
        }
        if (FloatMiniDiffers(&slotEC, &g_CFloatZero))
            m_typeId = 0x4162676C;
        break;
    }

    case 0x414E7265: m_typeId = 0x414E7265; break;
    case 0x53636833: m_typeId = 0x53636833; break;
    case 0x41646574: m_typeId = 0x41647431; break;
    case 0x416D7463: m_typeId = 0x53636832; break;

    case 0x416E6865: m_typeId = 0x416E6831; break;
    case 0x4172656C:
        if (FloatMiniNearlyEquals(&slot140, &g_slopeExpandLarge))
            m_typeId = 0x4172656C;
        if (FloatMiniNearlyEquals(&slot140, &g_slopeThreshold1))
            m_typeId = 0x41726C6C;
        break;

    case 0x41726F6F:
        m_typeId = 0x41726F32 - FloatMiniNearlyEquals(&slot140, &g_CFloatZero);
        break;

    case 0x4172746E:
        if (FloatMiniDiffers(&slot140, &g_CFloatZero) &&
            FloatMiniNearlyEquals(&slot124, &g_CFloatZero))
            m_typeId = 0x41726764;
        if (FloatMiniDiffers(&slot140, &g_CFloatZero) &&
            FloatMiniDiffers(&slot124, &g_CFloatZero))
            m_typeId = 0x4172676C;
        if (FloatMiniNearlyEquals(&slot140, &g_CFloatZero) &&
            FloatMiniDiffers(&slot124, &g_CFloatZero))
            m_typeId = 0x41726C6D;
        break;
    }
    }

notify:
    //  Shared tail: a virtual notify through m_timer's own vtable+0x24,
    //  then the get-or-create tracker record and the load-observer notify
    //  the whole family (0x6F052AF0/0x6F041700/0x6F041720) shares.
    ((void(__thiscall*)(void*))(*(void***)&m_timer)[0x24 / 4])(&m_timer);
    void* trackerRecord = GetOrCreateLegacyTrackerRecord(0x1795, 0xFFFFFFFF, &m_timer);
    NotifyLegacyMigrationDone(store, trackerRecord);
}
