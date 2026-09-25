//============================================================================
//  0x6F42C210 - CAgentRegistrySingleton::FinishSingleton.
//
//  The last step of the '+gam' singleton's construction
//  (CItemSlotSingletonHost::GetSlotTable, itemslotsingleton.cpp).  Three
//  things happen, in this order:
//
//   1. the game clock's *initial* time of day is written into the tracked
//      ref at element 2, from Misc/DayHours;
//   2. "is it daytime right now" is decided by comparing that time against
//      Misc/Dawn and Misc/Dusk, published through the handle-bearing flag
//      at +0x80, and the two boundary listeners are (re)built from it;
//   3. a '+qum' agent is made and registered into the optional handle ref
//      at +0x2BC.
//
//  The daytime test is `dawn <= now && now < dusk`, and it is written out
//  as two comparisons rather than one range check because that is what the
//  shipped code emits: the first `fcomp`/`fnstsw`/`test ah,41h` falls
//  through to the second only when `now >= dawn`, and the second's own
//  `test ah,41h`/`jnz` sends `now >= dusk` to the same zero as the first
//  branch.  The result is materialised into eax as 1 or 0 and pushed, so
//  it is an `int` argument and not a branch around two calls.
//
//  The three configured values are read with GetConfigFloat
//  (0x6F009EE0, configfloat.cpp), which returns a CFloat by value through
//  a hidden buffer - hence the three separate stack slots rather than one
//  reused temporary.
//
//  The DayHours value is copied out of its own slot before being handed
//  on (`mov edx,[esp+var_4C]` / `mov [esp+var_4C],edx` - a load and a
//  store back to the same address).  That is the shipped code, and it is
//  what a named `CFloat` local assigned from the call's result produces
//  when the compiler has already materialised the result into that slot;
//  folding it away removes the pair.
//
//  Own translation unit: it makes six real calls into three other modules
//  and is itself called from one.
//============================================================================
#include "agentregistrysingleton.h"
#include "agenttypedslots.h"
#include "unittrackedref.h"
#include "agiletype.h"
#include "game.h"
#include "widgetagentquery.h"
#include "CFloat.h"
#include "itemhandleresolve.h"

//  Reading a CFloat as the IEEE single it is - the same one-line helper
//  half a dozen files in src/Pathfinding already carry, for the same
//  reason: the shipped code compares these two with the x87 unit
//  directly, not through the software-float library.
//  By const reference: taking a CFloat by value copies it into a fresh
//  slot for every comparison, which puts a spill-and-reload pair in front
//  of each `fld`.  See agentdaynight.cpp, which has the same pair of
//  comparisons and the same note.
static inline float AsHardwareFloat(const CFloat& value) { return *(const float*)&value; }

//  0x6F009EE0 - configfloat.cpp's.  Declared rather than included so this
//  TU does not pull the whole config store in for one signature.
CFloat __fastcall GetConfigFloat(const char* section, const char* key, int index);

//  0x6F3DB6A0 - '+qum'.  misc_rawcode_getters.cpp's.
unsigned int GetConstant_6F3DB6A0();

//  0x6F473140 / 0x6F473170 - the published "is it daytime" flag at +0x80.
//  misc_handle_lookups.cpp declares the same struct; see its own note on
//  why the two declarations have to agree exactly.
struct HandleRefFieldOwner_6F473170
{
    char pad[8];
    unsigned int m_handle;   // +8
    int          m_typeTag;  // +0xC

    int QueryField78();
    void SetField78(int value);
};

//  0x6F0419C0 - SOptionalHandleRefResolver::ResolveChained, already
//  reconstructed (handlereref_resolve.cpp), whose own header carries the
//  type; included above through itemhandleresolve.h.
struct SOptionalHandleRefResolver : SOptionalHandleRef
{
    SOptionalHandleRef& ResolveChained(const void* ref);
};

//  The maker's answer, +0x54 again - the same shape GetSlotTable reads.
struct SMadeQumAgent
{
    char   m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

void CAgentRegistrySingleton::FinishSingleton()
{
    //  Step 1 - the clock's starting hour.
    CFloat dayHours = GetConfigFloat("Misc", "DayHours", 0);

    SUnitTrackedRef* clock =
        (SUnitTrackedRef*)((IndexedElementOwner_6F427F40*)this)->At(2);

    //  Both ends of the clock's range at once: low is the shared zero
    //  constant, high is the configured day length.  This is the setter
    //  the other five in unittrackedref.h are one-ended variants of.
    clock->SetRange(&g_CFloatZero, &dayHours);

    //  Read back into the same local: the shipped code hands GetValue the
    //  slot dayHours is already in.
    clock->GetValue(&dayHours);
    CFloat when = dayHours;

    //  Step 2.
    CFloat dawn = GetConfigFloat("Misc", "Dawn", 0);

    int isDaytime;
    if (AsHardwareFloat(when) >= AsHardwareFloat(dawn))
    {
        CFloat dusk = GetConfigFloat("Misc", "Dusk", 0);
        isDaytime = (AsHardwareFloat(when) < AsHardwareFloat(dusk)) ? 1 : 0;
    }
    else
    {
        isDaytime = 0;
    }

    ((HandleRefFieldOwner_6F473170*)m_isDaytime)->SetField78(isDaytime);
    RebuildDayNightListeners();

    //  Step 3.
    SCheckedQumSlot made(0);

    AGILE_TYPE_ID id(GetConstant_6F3DB6A0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_6F3DB6A0(), type->m_allocator);
    query.m_reserved24 = 0xFFFFFFFF;

    made.Assign(((SMadeQumAgent*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    ((SOptionalHandleRefResolver*)m_qumSlot)->ResolveChained(made.m_value);
}
