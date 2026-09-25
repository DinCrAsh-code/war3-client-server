//============================================================================
//  0x6F41F410 - CPlayerWar3::Method_0x0C, vtable slot 3 (+0x0C).  See
//  playerwar3.h.
//
//  Despite the name CLAUDE.md's own standing instruction for this session
//  used ("the message dispatcher"), this is not CUnit::Dispatch's own
//  flat switch-over-wire-message-id shape (src/Unit/unit_dispatch.cpp) -
//  it is an 8-way switch on the incoming `SAgentMessage::m_id` used here
//  as a small internal selector (0..7), not a wire message id.  Six arms
//  read one of the ability-entry array's own `{handle, typeTag}`-bearing
//  elements (m_abilityEntries, playerwar3.h - the same array Method_0x24/
//  Method_0x20/slot 30 already walk) through the already-reconstructed
//  `HandleRefFieldOwner_6F473170::QueryField78`
//  (Misc/misc_handle_lookups.cpp), builds a `CPlayerStateChanged` and
//  publishes it through the base class's own inherited slot 4
//  (`Method_0x10`, observer.h); two arms (6/7) write instead, through the
//  matching `SetField78`.
//
//  Field-to-array-element map (offset from `this`, `m_abilityEntries`
//  stride 0x10 starting at +0x40 - each element's own `{handle, typeTag}`
//  pair sits at its own +8/+0xC, exactly where HandleRefFieldOwner_6F473170
//  expects it, so a plain reinterpret-cast of `&m_abilityEntries[N]` reads
//  it):
//    case 0 (id 0x8026B): element 1 (+0x50), QueryField78()/10
//    case 1 (id 0x8026C): element 2 (+0x60), QueryField78()/10
//    case 2 (id 0x8026D): element 13 (+0x110), QueryField78()
//    case 3 (id 0x8026E): element 14 (+0x120), QueryField78()
//    case 4 (id 0x8026F): element 5 (+0x90), QueryField78()
//    case 5 (id 0x80270): min(element 4's QueryField78(), element 6's,
//                          a cached "FoodCeiling" Misc-config difficulty
//                          tunable, GetFoodCeilingTunable below)
//    case 6: write-only, kept a naked thunk (see below)
//    case 7: write-only, kept a naked thunk (see below)
//
//  Own translation unit: QueryField78, GetFoodCeilingTunable and the base
//  class's own inherited Method_0x10 are all real calls.
//============================================================================
#include "playerwar3.h"
#include "game.h"
#include "storm.h"
#include "CMiscCustom.h"
#include "boxmath.h"

//----------------------------------------------------------------------------
//  0x6F473170 - HandleRefFieldOwner_6F473170::QueryField78, already
//  reconstructed (Misc/misc_handle_lookups.cpp).  Redeclared identically
//  here (no shared header exists for it yet) so this translation unit
//  reaches it with a real call under the same mangled name.
//----------------------------------------------------------------------------
struct HandleRefFieldOwner_6F473170
{
    char pad[8];
    unsigned int m_handle;   // +8
    int          m_typeTag;  // +0xC

    int QueryField78();
    void SetField78(int value);
};

//----------------------------------------------------------------------------
//  A layout-compatible overlay of `SAgentMessage` (observer.h) - not a real
//  inheritance relationship, the same non-inheriting overlay
//  Net/netcommand.h's own `CNetCommandBase` documents for the identical
//  reason: `CObserver::Method_0x10` and friends read any of these
//  generically as an `SAgentMessage*` without knowing the concrete type.
//  `virtual ~CPlayerStateChanged() {}` is what makes the compiler actually
//  stamp `??_7CPlayerStateChanged@@6B@` at +0 rather than the store being
//  invented - the same reasoning `CNetCommandBase`'s own trivial virtual
//  destructor is there for.
//----------------------------------------------------------------------------
class CPlayerStateChanged
{
public:
    virtual ~CPlayerStateChanged() {}

    CPlayerStateChanged(unsigned int id, int value)
        : m_field4(0), m_id(id), m_field0C(0xFFFFFFFF), m_value(value)
    {
    }

    int          m_field4;    // +0x04 (always 0)
    unsigned int m_id;        // +0x08 - SAgentMessage::m_id
    unsigned int m_field0C;   // +0x0C (always -1 - never overwritten by any of the six read arms)
    int          m_value;     // +0x10
};

//----------------------------------------------------------------------------
//  0x6F00A210 - a cached "Misc"/"FoodCeiling" config-int difficulty
//  tunable, clamped to [1, 300] once looked up, or read straight out of a
//  fixed per-difficulty table (`off_6F876198`) when the config key is not
//  present at all.  Same `GetMiscCustom`/`HasValue`/`GetInt` triple every
//  other "Misc" config tunable in this image goes through
//  (Config/CMiscCustom.h) - own translation unit for the same "shared leaf,
//  own address neighbourhood" reason CLAUDE.md's own house rules ask for.
//----------------------------------------------------------------------------
static int g_foodCeilingCache = -1;   // dword_6FA4F73C

CMiscCustom* GetMiscCustom();

extern const int g_foodCeilingByDifficulty[];   // dword_6F876198, funcmap.py DATA
//  dword_6FA4F6E8 - already named g_rowTableCurrentRow elsewhere in this
//  codebase (Pathfinding/rowtable.cpp and others), declared `int` there
//  ("-1 = none selected") - redeclared identically here rather than typed
//  `unsigned int`, so this stays the same symbol.
extern int g_rowTableCurrentRow;

int GetFoodCeilingTunable()
{
    if (g_foodCeilingCache != -1)
        return g_foodCeilingCache;

    CMiscCustom* misc = GetMiscCustom();

    if ((misc->m_pPrimary != 0 || misc->m_pSecondary != 0) &&
        misc->HasValue("Misc", "FoodCeiling"))
    {
        int value = misc->GetInt("Misc", "FoodCeiling", 0);
        g_foodCeilingCache = value;
        if (value < 1)
        {
            g_foodCeilingCache = 1;
            return 1;
        }
        if (value > 0x12C)
        {
            g_foodCeilingCache = 0x12C;
            return 0x12C;
        }
        return value;
    }

    g_foodCeilingCache = g_foodCeilingByDifficulty[g_rowTableCurrentRow];
    return g_foodCeilingCache;
}

//----------------------------------------------------------------------------
//  0x6F340A60 / 0x6F340A80 - two byte-identical bodies at separate
//  addresses (this image's own no-`/OPT:ICF` duplication,
//  agentdefaults.cpp), each `QueryField78()` over one fixed ability-entry
//  element divided by 10 (the classic MSVC signed-divide-by-10 magic
//  constant, `0x66666667`/`sar 2`/sign-fixup - docs/msvc-vc8-idioms.md).
//----------------------------------------------------------------------------
static int __fastcall QueryElement1Field78DivTen(void* self)
{
    return ((HandleRefFieldOwner_6F473170*)((char*)self + 0x50))->QueryField78() / 10;
}

static int __fastcall QueryElement2Field78DivTen(void* self)
{
    return ((HandleRefFieldOwner_6F473170*)((char*)self + 0x60))->QueryField78() / 10;
}

//----------------------------------------------------------------------------
//  0x6F40F5D0 - a static adjustor tail-jump into QueryField78, the same
//  shape `QueryField78At0xC8` (misc_handle_lookups.cpp) already
//  establishes for this exact idiom.  `__fastcall`, matching that
//  precedent's own reasoning: called with the object in ecx and nothing on
//  the stack.
//----------------------------------------------------------------------------
__declspec(naked) int __fastcall QueryElement5Field78(void*)
{
    __asm
    {
        add     ecx, 90h
        jmp     HandleRefFieldOwner_6F473170::QueryField78
    }
}

//----------------------------------------------------------------------------
//  0x6F40F850 - min(element 4's QueryField78(), element 6's,
//  GetFoodCeilingTunable()).
//----------------------------------------------------------------------------
static int __fastcall QueryElement46FoodCeilingMin(void* self)
{
    int a = ((HandleRefFieldOwner_6F473170*)((char*)self + 0x80))->QueryField78();
    int b = ((HandleRefFieldOwner_6F473170*)((char*)self + 0xA0))->QueryField78();
    int c = GetFoodCeilingTunable();

    int m = (a < b) ? a : b;
    return (m >= c) ? c : m;
}

//----------------------------------------------------------------------------
//  0x6F00A2A0 - "Misc"/"UpkeepUsage": ten food-usage tier thresholds,
//  cached exactly like GetFoodCeilingTunable above but as a whole 10-entry
//  table read in one pass rather than a single scalar - the caller
//  (CPlayerWar3UpkeepRecompute below) walks it as a threshold ladder.  An
//  index past the end of the table (>= 10) is simply out of range and
//  returns 0 *without* touching the cache at all - no clamp, unlike the two
//  CFloat siblings below.  `ecx` carries the index directly (no hidden
//  return pointer - this one returns a plain `int`), unlike those two.
//----------------------------------------------------------------------------
static int g_upkeepUsageCache[10] = { -1 };   // dword_6FA4F740[0..9]

//  dword_6F876148 - the per-difficulty fallback: one 10-int row per
//  difficulty, laid out as one flat table (`g_rowTableCurrentRow * 10 + i`),
//  the same "single flat array, row-multiplied index" shape
//  `g_foodCeilingByDifficulty` above already establishes, just wider.
extern const int g_upkeepUsageByDifficulty[][10];   // funcmap.py DATA

int __fastcall GetUpkeepUsageThreshold(int tier)
{
    if ((unsigned int)tier >= 0xA)
        return 0;

    if (g_upkeepUsageCache[0] != -1)
        return g_upkeepUsageCache[tier];

    CMiscCustom* misc = GetMiscCustom();

    if ((misc->m_pPrimary != 0 || misc->m_pSecondary != 0) &&
        misc->HasValue("Misc", "UpkeepUsage"))
    {
        for (int i = 0; i < 10; i++)
            g_upkeepUsageCache[i] = misc->GetInt("Misc", "UpkeepUsage", i);
    }
    else
    {
        const int* row = g_upkeepUsageByDifficulty[g_rowTableCurrentRow];
        for (int i = 0; i < 10; i++)
            g_upkeepUsageCache[i] = row[i];
    }

    return g_upkeepUsageCache[tier];
}

//----------------------------------------------------------------------------
//  0x6F00A3B0 / 0x6F00A4D0 - "Misc"/"UpkeepGoldTax" and "Misc"/
//  "UpkeepLumberTax": a per-upkeep-tier CFloat multiplier each, same
//  two-source config shape as every other "Misc" tunable in this module,
//  but returning a `CFloat` by hidden pointer (so `edx`, not `ecx`, carries
//  the index - `ecx` is the hidden return buffer) and clamping an
//  out-of-range tier to the last entry (9) instead of returning a sentinel.
//  Unlike GetUpkeepUsageThreshold's fallback, neither of these two
//  fallback tables is difficulty-indexed - one fixed 10-entry default row
//  each, used regardless of difficulty.  The "already cached" sentinel is
//  `-1.0f`, compared as a raw reinterpreted `float` the same way
//  `CFloat::IsLess`/`IsGreater` do (floatcompare.cpp) rather than through
//  any `CFloat` member - there is no `operator!=` on `CFloat` to spell it
//  with directly.
//----------------------------------------------------------------------------
static CFloat g_upkeepGoldTaxCache[10];     // dword_6FAAE3C8[0..9]
static CFloat g_upkeepLumberTaxCache[10];   // dword_6FAAE3F0[0..9]

extern const CFloat g_upkeepGoldTaxDefault[10];     // dword_6FAAE378, funcmap.py DATA
extern const CFloat g_upkeepLumberTaxDefault[10];   // dword_6FAAE3A0, funcmap.py DATA

CFloat __fastcall GetUpkeepGoldTaxTunable(int tier)
{
    int clamped = tier;
    if ((unsigned int)tier >= 0xA)
        clamped = 9;

    if (*(const float*)&g_upkeepGoldTaxCache[0].m_bits ==
        *(const float*)&g_CFloatMinusOne.m_bits)
    {
        CMiscCustom* misc = GetMiscCustom();

        if ((misc->m_pPrimary != 0 || misc->m_pSecondary != 0) &&
            misc->HasValue("Misc", "UpkeepGoldTax"))
        {
            for (int i = 0; i < 10; i++)
                g_upkeepGoldTaxCache[i] = misc->GetFloat("Misc", "UpkeepGoldTax", i);
        }
        else
        {
            for (int i = 0; i < 10; i++)
                g_upkeepGoldTaxCache[i] = g_upkeepGoldTaxDefault[i];
        }
    }

    return g_upkeepGoldTaxCache[clamped];
}

CFloat __fastcall GetUpkeepLumberTaxTunable(int tier)
{
    int clamped = tier;
    if ((unsigned int)tier >= 0xA)
        clamped = 9;

    if (*(const float*)&g_upkeepLumberTaxCache[0].m_bits ==
        *(const float*)&g_CFloatMinusOne.m_bits)
    {
        CMiscCustom* misc = GetMiscCustom();

        if ((misc->m_pPrimary != 0 || misc->m_pSecondary != 0) &&
            misc->HasValue("Misc", "UpkeepLumberTax"))
        {
            for (int i = 0; i < 10; i++)
                g_upkeepLumberTaxCache[i] = misc->GetFloat("Misc", "UpkeepLumberTax", i);
        }
        else
        {
            for (int i = 0; i < 10; i++)
                g_upkeepLumberTaxCache[i] = g_upkeepLumberTaxDefault[i];
        }
    }

    return g_upkeepLumberTaxCache[clamped];
}

//----------------------------------------------------------------------------
//  dword_6FAAE574 - a CFloat global filled in at start-up (Player/
//  playerscore.h has the full note); redeclared identically here rather
//  than pulling that header in, the same "own translation unit, own
//  redeclaration" rule the rest of this file already follows for
//  g_rowTableCurrentRow above.
//----------------------------------------------------------------------------
extern CFloat g_unk6FAAE574;

//  0x6F6EF330 - "add half a unit and floor", i.e. round-half-up - its own
//  translation unit (Math/cfloatround.cpp, own address neighbourhood, so
//  /Ob2 does not inline it into either of the two call sites below).
CFloat __fastcall RoundCFloatHalfUp(const CFloat& value);

//----------------------------------------------------------------------------
//  0x6F40F620 - case 6's write-only arm: recompute the player's cached gold
//  and lumber upkeep tax multipliers (elements 13/14, +0x110/+0x120) from
//  the current food usage (element 5, +0x90).  `tier` is the number of
//  UpkeepUsage thresholds the player's own food usage has climbed past - a
//  plain ladder search, not a binary search, matching the two redundant
//  per-iteration calls to GetUpkeepUsageThreshold the shipped loop makes
//  (the first call of each iteration only feeds the `while` condition; the
//  compiler does not - and given an opaque call, cannot - reuse that result
//  for the `if` inside the body, so the same tier is queried twice).  Each
//  tax multiplier is applied to the same dword_6FAAE574 global (an
//  as-yet-unidentified scale, see its own declaration above), rounded
//  half-up and truncated to an int before being published through
//  SetField78.
//----------------------------------------------------------------------------
void __fastcall CPlayerWar3UpkeepRecompute(CPlayerWar3* self)
{
    int foodUsage = QueryElement5Field78(self);

    int tier = 0;
    while (GetUpkeepUsageThreshold(tier) != 0)
    {
        if (GetUpkeepUsageThreshold(tier) >= foodUsage)
            break;
        tier++;
    }

    int goldValue = CFloatToInt(RoundCFloatHalfUp(
        GetUpkeepGoldTaxTunable(tier) * g_unk6FAAE574));
    ((HandleRefFieldOwner_6F473170*)((char*)self + 0x110))->SetField78(goldValue);

    int lumberValue = CFloatToInt(RoundCFloatHalfUp(
        GetUpkeepLumberTaxTunable(tier) * g_unk6FAAE574));
    ((HandleRefFieldOwner_6F473170*)((char*)self + 0x120))->SetField78(lumberValue);
}

//  0x6F41F340 - CPlayerWar3HandlePlayerLeftNotify, a real reconstruction
//  now (playerwar3_handleplayerleftnotify.cpp - own translation unit, own
//  address neighbourhood, and a large enough closure of its own to
//  warrant it).
void __fastcall CPlayerWar3HandlePlayerLeftNotify(CPlayerWar3* self);

//  The local `CPlayerStateChanged` is constructed once, unconditionally,
//  ahead of the switch - matching the shipped stream, which stamps the
//  whole struct (vtable included) before the range check and jump table
//  even run, so cases 6/7 and the default arm build (and silently
//  discard) it too.  A first draft constructed one per read-arm instead
//  and cost the function roughly 40 instructions: six duplicated
//  construct-then-call sequences where the shipped stream shares one.
int CPlayerWar3::Method_0x0C(const SAgentMessage* msg)
{
    CPlayerStateChanged evt(0xFFFFFFFF, 0);

    switch (msg->m_id)
    {
    case 0:
        evt.m_id = 0x8026B;
        evt.m_value = QueryElement1Field78DivTen(this);
        return Method_0x10((SAgentMessage*)&evt);
    case 1:
        evt.m_id = 0x8026C;
        evt.m_value = QueryElement2Field78DivTen(this);
        return Method_0x10((SAgentMessage*)&evt);
    case 2:
        evt.m_id = 0x8026D;
        evt.m_value = ((HandleRefFieldOwner_6F473170*)((char*)this + 0x110))->QueryField78();
        return Method_0x10((SAgentMessage*)&evt);
    case 3:
        evt.m_id = 0x8026E;
        evt.m_value = ((HandleRefFieldOwner_6F473170*)((char*)this + 0x120))->QueryField78();
        return Method_0x10((SAgentMessage*)&evt);
    case 5:
        evt.m_id = 0x80270;
        evt.m_value = QueryElement46FoodCeilingMin(this);
        return Method_0x10((SAgentMessage*)&evt);
    case 4:
        evt.m_id = 0x8026F;
        evt.m_value = QueryElement5Field78(this);
        return Method_0x10((SAgentMessage*)&evt);
    case 6:
        CPlayerWar3UpkeepRecompute(this);
        return 1;
    case 7:
        CPlayerWar3HandlePlayerLeftNotify(this);
        return 1;
    default:
        return 0;
    }
}
