//============================================================================
//  createunit-G3's own closing pair: find the nearest creep spot with a
//  matching occupant (0x6F2E38A0) and, from CUnit::NotifyAbilityHostChanged
//  (unit_abilityhostnotify.cpp, only for a neutral-hostile unit, player
//  index 0x0C), either join that spot or register a brand-new one
//  (0x6F2E51D0).  This is the piece that ties creepcampregistry.cpp and
//  creepspot.cpp together with the earlier checked-slot cluster
//  (Agent/checkedslot_createunit_cluster.cpp): 0x6F2E51D0 tail-jumps into
//  that cluster's own Register_6F250FD0 twice.
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include "game.h"
#include "CFloat.h"
#include "widgetagentquery.h"

int __fastcall GetConfigInt(const char* section, const char* key, int index);

//  The global creep-spot table: dword_6FAB4E78 (count), dword_6FAB4EC8
//  (the TSGrowableArray<SCheckedSlot_6F24F580> at dword_6FAB4EC0's own
//  +0x08 data pointer, creepcampregistry.cpp), dword_6FAB4EBC (the cached,
//  squared CreepCampPathingCellDistance threshold - CFloat, recomputed
//  once and reused after).
extern unsigned int g_creepSpotCount;          // dword_6FAB4E78
extern CAgent**      g_creepSpotData;           // dword_6FAB4EC8
extern int           g_creepSpotDistanceSq;     // dword_6FAB4EBC (CFloat bits)

struct CCreepSpot
{
    char         m_reserved00[0x24];
    unsigned int m_occupantCount;   // +0x24

    void AddOccupant(void* unit);        //  0x6F2E1AC0, Unit/creepspot.cpp
    CAgent* FindOccupantOfType();        //  0x6F2E1B70, Unit/creepspot.cpp -
                                          //  minimal redeclaration, same rule
                                          //  SCheckedSlot_6F24F580 below uses
};

void* __fastcall ComputeCreepSpotDistance(void* self, void* out, void* other,
                                          int useSmartPtr);      // 0x6F477070, creepcampdistance.cpp

//----------------------------------------------------------------------------
//  0x6F2E38A0 - find the index of the nearest registered creep spot that
//  (a) is not already full (12 occupants) and (b) has an occupant whose
//  type matches CCreepSpot::FindOccupantOfType's own gate, and whose
//  distance to `unit` is within CreepCampPathingCellDistance (a config
//  value, squared once and cached in dword_6FAB4EBC).  Returns -1 if none
//  qualifies.  `unit`'s own +0x164 (the same offset SUnitMovement sits at,
//  unit.h) is the position sub-object the distance probe reads from both
//  sides.
//
//  `this` (ecx) is never read anywhere in the shipped body - the one
//  caller in this closure (0x6F2E51D0) passes the same CUnit* both ways
//  (unchanged in ecx from its own entry, and explicitly as the one stack
//  argument), so this is written as a member with an unused `this` for
//  the calling convention alone (a plain `retn 4` __thiscall, not
//  `__fastcall` - the real argument is on the stack, not in edx), the
//  same shape RoundDownToPowerOfTwoCapped8/64
//  (Containers/roundtopoweroftwo.cpp) already use for an unused ecx
//  under `__fastcall`.  Not a real class - a bare wrapper purely so
//  MSVC accepts a `this`-taking member here at all.
//----------------------------------------------------------------------------
struct SCreepSpotFinder
{
    int GetNearestIndex(void* unit);
};

int SCreepSpotFinder::GetNearestIndex(void* unit)
{
    if (*(float*)&g_creepSpotDistanceSq == 0.0f)
    {
        int side = GetConfigInt("Misc", "CreepCampPathingCellDistance", 0);
        int side2 = GetConfigInt("Misc", "CreepCampPathingCellDistance", 0);
        g_creepSpotDistanceSq = CFloatFromInt(side2 * side).m_bits;
    }

    void* unitPos = (char*)unit + 0x164;

    int best = -1;
    for (unsigned int i = 0; i < g_creepSpotCount; i++)
    {
        CCreepSpot* spot = (CCreepSpot*)g_creepSpotData[i];
        if (spot->m_occupantCount == 12)
            continue;

        CAgent* occupant = spot->FindOccupantOfType();
        if (!occupant)
            continue;

        int distSq;
        ComputeCreepSpotDistance((char*)occupant + 0x164, &distSq, unitPos, 1);

        if (*(float*)&distSq >= *(float*)&g_creepSpotDistanceSq)
            continue;

        best = i;
        g_creepSpotDistanceSq = distSq;
    }

    return best;
}

//----------------------------------------------------------------------------
//  createunit-H3 follow-up, PART 2: the real callee the prior round's own
//  "register a brand-new CCreepSpot" branch was missing was never a
//  separate function at all.  It is the exact same six-step
//  GetFourCC/Hash/Ptr/InitWidgetAgentQuery/SubmitWidgetAgentQuery/+0x54
//  sequence every `MakeAbilityAgent_*` factory in
//  Unit/cunit_agent8_abilityfactories.cpp and
//  SCheckedSlot_6F24F510::MakeAndAssign (Agent/checkedslot_createunit_
//  cluster.cpp) already use, inlined straight into 0x6F2E51D0 rather than
//  reached through a shared helper - typed against '+ccp'
//  (GetFourCC_6F2CFBD0, already reconstructed, misc_rawcode_getters.cpp)
//  and g_pGameData's own AGILE_TYPE_TABLE (game.h) rather than a
//  CGameData-external one.  Confirmed by literally following the raw
//  disassembly's own call targets instead of assuming the prior round's
//  own guess at what the branch reached.
//
//  Result lands straight into g_creepSpotData[newIndex] through
//  SCheckedSlot_6F2CFBD0::Assign - the array element *is* one of these
//  checked slots (a bare `CAgent*`), the same overlay
//  Agent/checkedslot_createunit_cluster.cpp already documents for its own
//  '+ccp'-keyed sibling.  The +0x24 query-flag write is a plain -1 here
//  (MakeAndAssign's own formula, `-1 - (mode == 2)`, at mode 0).
//----------------------------------------------------------------------------
extern unsigned int GetFourCC_6F2CFBD0();     //  'AIcs' - misc_rawcode_getters.cpp

//  Minimal redeclaration matching SCheckedSlot_6F2CFBD0::Assign's own
//  definition (Agent/checkedslot_createunit_cluster.cpp) - same rule
//  SUnitAbilityHost/CCreepSpot above already follow.
struct SCheckedSlot_6F2CFBD0
{
    SCheckedSlot_6F2CFBD0* Assign(CAgent* candidate);   //  0x6F24F6F0
    CAgent* m_value;
};

//  Minimal redeclaration matching CreepSpotArray::EnsureIndex's own
//  definition (Unit/creepcampregistry.cpp) - the array object itself,
//  dword_6FAB4EC0, had no C++-level binding anywhere in this closure
//  before now (only its own +0x08 data pointer, g_creepSpotData, did).
struct CreepSpotArray
{
    void EnsureIndex(unsigned int index, int clear);   //  0x6F2E4B00
};
extern CreepSpotArray g_creepSpotArray;                //  dword_6FAB4EC0

extern void __fastcall Register_6F250FD0(CAgent* candidate);   //  Agent/checkedslot_createunit_cluster.cpp

//----------------------------------------------------------------------------
//  0x6F2E51D0 - CUnit::NotifyAbilityHostChanged's own creep-spot-membership
//  helper for a neutral-hostile unit (unit_abilityhostnotify.cpp: called
//  only when GetOwningPlayerIndex()==0x0C).
//----------------------------------------------------------------------------
void __fastcall NotifyCreepSpotMembership(void* self)
{
    int index = ((SCreepSpotFinder*)self)->GetNearestIndex(self);

    if (index >= 0)
    {
        CCreepSpot* spot = (CCreepSpot*)g_creepSpotData[index];
        spot->AddOccupant(self);

        if (!(*((unsigned char*)self + 0x60) & 0x10))
            Register_6F250FD0((CAgent*)spot);
        return;
    }

    //  No existing spot qualified - register a brand-new one.
    //
    //  `this`'s own current world position is resolved and then thrown
    //  away - the same "call it anyway" side-effect-only shape already
    //  established for a resolved candidate's position in this closure's
    //  own distance probes: LookupHandle/PointAtDistance underneath
    //  CPathRef::ToWorldVec3 has effects this branch needs even though the
    //  coordinates themselves are never read again.
    typedef CPathRef* (__thiscall *GetPositionRefFn)(void*);
    CPathRef* ref = ((GetPositionRefFn)(*(void***)self)[0x2E])(self);
    ref->SmartPtrToWorldVec3();

    unsigned int newIndex = g_creepSpotCount;
    g_creepSpotCount = newIndex + 1;
    g_creepSpotArray.EnsureIndex(newIndex, 0);

    //  Cached across the whole factory call sequence below (the shipped
    //  stream keeps it in a callee-saved register the same way) - the
    //  address of the array *slot*, not the CCreepSpot* it will hold,
    //  which is only valid after Assign() below actually writes it.
    CAgent** newSlot = &g_creepSpotData[newIndex];

    unsigned int typeId = GetFourCC_6F2CFBD0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F2CFBD0(), pool);
    *(int*)((char*)&query + 0x24) = -1;

    void* madeHolder = SubmitWidgetAgentQuery(&query, 1, 1);
    CAgent* made = *(CAgent**)((char*)madeHolder + 0x54);

    ((SCheckedSlot_6F2CFBD0*)newSlot)->Assign(made);

    CCreepSpot* newSpot = (CCreepSpot*)g_creepSpotData[newIndex];
    newSpot->m_occupantCount = 0;
    newSpot->AddOccupant(self);

    if (!(*((unsigned char*)self + 0x60) & 0x10))
        Register_6F250FD0((CAgent*)newSpot);
}
