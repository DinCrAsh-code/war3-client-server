//============================================================================
//  0x6F4193E0 - CPlayerWar3::CPlayerWar3().  See playerwar3.h and
//  docs/targets/CPlayerWar3.md for the field-layout findings this
//  constructor establishes.
//
//  Vtable-completion gate (CLAUDE.md / ctor_vtable_audit.py): only 10 of 33
//  slots (plus the destructor) are real reconstructions as of this commit,
//  so installing this class's own vtable here is a known, accepted RISKY
//  finding rather than a fixed one - the same state CCameraWar3's own
//  constructor (GameUI/cameraconstructor.cpp, missing=48) and CEscMenu's/
//  CSimpleTop's are already in.  A first draft here tried the "store the
//  literal shipped address instead" idiom the audit's own docstring
//  suggests (`*(void**)this = (void*)0x6F94E9D4;`), and it does not work:
//  once a class has ANY declared virtual, the C++ standard requires the
//  compiler to stamp *this class's own* vtable pointer automatically, the
//  instant the immediate base's constructor returns and before the user's
//  own constructor body runs at all - `build/playerwar3_ctor.asm` shows
//  the real `mov DWORD PTR [esi], OFFSET ??_7CPlayerWar3@@6B@` landing
//  right after `call ??0CAgent@@QAE@XZ`, with the literal-store attempt
//  only adding a second, redundant, wrong-shaped instruction after it (and
//  costing the score for it).  There is no portable-C++ way to suppress
//  that automatic stamp for a class that already has real virtual
//  overrides declared - only a wholly `__declspec(naked)` constructor
//  could, which is a different, much larger undertaking than this
//  session's scope.  So this constructor takes the same accepted-gap shape
//  CCameraWar3's own already does: the vtable stamp "comes for free" from
//  `: public CAgentWar3`, `mix_config.json` never arms it (new keys default
//  false and nothing here turns one on), and the fix - the one
//  `ctor_vtable_audit.py` names - is finishing the remaining 23 slots, not
//  a source-level workaround.
//
//  Base construction (the implicit CAgentWar3()->CAgent() chain, matching
//  the shipped `call sub_6F001730`) is ordinary C++: CAgentWar3 declares no
//  constructor of its own (agentwar3.h), so the compiler's own implicit one
//  runs CAgent::CAgent() (agent_ctor.cpp) for free, before this body starts,
//  and the vtable stamp above comes from being a properly-shaped derived
//  class rather than from any statement in this file.
//
//  Field construction after the vtable stamp, in the shipped order: +0x24
//  (RCString::Construct(0)), +0x34 (a fresh CSelectionWar3 via a raw
//  SMemAlloc + placement-construct, or null if the allocation failed -
//  playerwar3_ctorsubobjects.cpp), +0x38 (SCheckedAllySlot(0),
//  agenttypedslots.h), the 26-entry AbilityEntry array at +0x40 (an
//  `eh vector constructor iterator` under the shipped compiler; this
//  build's fixed /EHs-c- has no exception model for that helper to guard
//  and emits a plain loop instead - the same "array-member iterator becomes
//  a loop" gap playerwar3_dtor.cpp's own teardown already documents), the
//  long run of zeroed SIntMiniValue-shaped fields from +0x1E0 through
//  +0x294 (including the two 12-entry SIntMiniValue arrays DumpState
//  already walks), two embedded FloatMini objects at +0x298/+0x2A0
//  (floatmini.h's own placement-new constructor, the same idiom item.cpp
//  uses), the +0x2AC/+0x2BC TSGrowableArray-shaped headers (zeroed here,
//  then reserved to 7 elements each at the very end via
//  TSGrowableArray<SCheckedUnitSlot>::SetCount (already reconstructed,
//  Containers/unitagentptrarraycount.cpp) / PlayerWar3IntMiniArrayReserve),
//  a fresh
//  CTechTree at +0x2D0 (same raw-alloc-then-placement-construct shape as
//  +0x34), two more raw dwords at +0x2D4/+0x2D8 set to -1 and +0x2DC
//  through +0x2EC zeroed (+0x2EC is the BestHeroData pointer
//  playerwar3_dtor.cpp's own SMemFree call already names), a CAgentTimer at
//  +0x2FC (agenttimer.h, already reconstructed), an RCString at +0x310,
//  three more raw dwords at +0x31C/+0x320/+0x324 set to 1 (untouched by
//  anything else in this session's scope - no name invented, per CLAUDE.md's
//  "do not invent fields the call tree does not read"), four more zeroed at
//  +0x328 through +0x334 (three of which are DumpState's own last three
//  SIntMiniValue fields), and finally the two SetCount(7) calls.
//
//  Same unreproducible __except_handler4-shaped SEH frame every constructor
//  in this family has (docs/msvc-vc8-idioms.md); not chased further.
//============================================================================
#include "playerwar3.h"
#include "agenttypedslots.h"
#include "agenttimer.h"
#include "rcstring.h"
#include "floatmini.h"
#include "storm.h"
#include "tsarray.inl"

void PlayerWar3ConstructSelectionWar3(void* mem);   // playerwar3_ctorsubobjects.cpp, 0x6F4221B0
void PlayerWar3ConstructTechTree(void* mem);        // playerwar3_ctorsubobjects.cpp, 0x6F401390
void PlayerWar3IntMiniArrayReserve(void* arr);      // playerwar3_ctorsubobjects.cpp, 0x6F416EE0(7)

//  0x6F334410 - the +0x2AC array's own SetCount(7).  Already reconstructed
//  as TSGrowableArray<SCheckedUnitSlot>::SetCount
//  (Containers/unitagentptrarraycount.cpp) - see playerwar3_ctorsubobjects
//  .cpp's own header comment for why this constructor reuses it rather than
//  re-declaring it under a private name.
template <> void TSGrowableArray<SCheckedUnitSlot>::SetCount(unsigned int count);

CPlayerWar3::CPlayerWar3()
{
    ((RCString*)((char*)this + 0x24))->Construct(0);

    void* selection = SMemAlloc(0x210, ".\\CPlayerWar3.cpp", 0x3C, 0);
    if (selection)
        PlayerWar3ConstructSelectionWar3(selection);
    *(void**)((char*)this + 0x34) = selection;

    new ((char*)this + 0x38) SCheckedAllySlot(0);

    m_abilityEntryCount = 0x1A;
    for (int i = 0; i < 0x1A; i++)
        m_abilityEntries[i].Construct();

    m_dumpArrayACount = 0xC;
    for (int i = 0; i < 0xC; i++)
        m_dumpArrayA[i].m_value = 0;
    m_dumpArrayBCount = 0xC;
    for (int i = 0; i < 0xC; i++)
        m_dumpArrayB[i].m_value = 0;

    for (unsigned int off = 0x248; off <= 0x294; off += 4)
        *(unsigned int*)((char*)this + off) = 0;

    new ((char*)this + 0x298) FloatMini();
    new ((char*)this + 0x2A0) FloatMini();

    *(unsigned int*)((char*)this + 0x2A8) = 0;

    unsigned int* hero = (unsigned int*)((char*)this + 0x2AC);
    hero[0] = 0; hero[1] = 0; hero[2] = 0; hero[3] = 0;

    unsigned int* intMini = (unsigned int*)((char*)this + 0x2BC);
    intMini[0] = 0; intMini[1] = 0; intMini[2] = 0; intMini[3] = 0;

    void* techTree = SMemAlloc(0x64, ".\\CPlayerWar3.cpp", 0x3D, 0);
    if (techTree)
        PlayerWar3ConstructTechTree(techTree);
    *(void**)((char*)this + 0x2D0) = techTree;

    *(unsigned int*)((char*)this + 0x2D4) = 0xFFFFFFFF;
    *(unsigned int*)((char*)this + 0x2D8) = 0xFFFFFFFF;
    *(unsigned int*)((char*)this + 0x2DC) = 0;
    *(unsigned int*)((char*)this + 0x2E0) = 0;
    *(unsigned int*)((char*)this + 0x2E4) = 0;
    *(unsigned int*)((char*)this + 0x2E8) = 0;
    *(unsigned int*)((char*)this + 0x2EC) = 0;

    *(unsigned int*)((char*)this + 0x2F0) = 0xFFFFFFFF;
    *(unsigned int*)((char*)this + 0x2F4) = 0xFFFFFFFF;
    *(unsigned int*)((char*)this + 0x2F8) = 0;
    new ((char*)this + 0x2FC) CAgentTimer();

    ((RCString*)((char*)this + 0x310))->Construct(0);

    *(unsigned int*)((char*)this + 0x31C) = 1;
    *(unsigned int*)((char*)this + 0x320) = 1;
    *(unsigned int*)((char*)this + 0x324) = 1;
    *(unsigned int*)((char*)this + 0x328) = 0;
    *(unsigned int*)((char*)this + 0x32C) = 0;
    *(unsigned int*)((char*)this + 0x330) = 0;
    *(unsigned int*)((char*)this + 0x334) = 0;

    ((TSGrowableArray<SCheckedUnitSlot>*)((char*)this + 0x2AC))->SetCount(7);
    PlayerWar3IntMiniArrayReserve((char*)this + 0x2BC);
}
