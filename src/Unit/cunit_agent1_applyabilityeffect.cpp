//============================================================================
//  0x6F1FC130 - a depth-1/2 CUnit-vtable-BFS leaf (cunit_agent1_worklist.md).
//  `this` is CUnit (confirmed by the direct QueryVisibleImpl(1) call, the
//  same minimal local re-declaration GameUI/gameuiscreenproject.cpp and
//  GameUI/gameuivisualplacement.cpp already use rather than pulling in the
//  whole class).
//
//  Skips everything if the unit is not visible-for-check-1; otherwise looks
//  up the fixed 'ACsp' ability type record (LookupAbilityTypeRecord,
//  already declared in Item/ability_slots.cpp), resolves its own row table
//  at +0x1F8 (RowTableLookup, Game/objectdatatable.h), and - only if a row
//  really exists - makes an 'ACsp' agent (0x6F1FA010, the same
//  CAgentPtr_<tag>-family shape cunit_agent1_makeagent.cpp documents in
//  full, reused here rather than re-derived) and dispatches through its
//  own vtable slot 0x80 with `this` and a pointer into the type record's
//  own +0x18.
//
//  The final call (0x6F079990) is a much larger ability-installation
//  dispatcher outside this batch's own worklist - it constructs, among
//  other things, this file's own CAgentPtr_Buff (0x6F03C050, the direct
//  giveaway that this whole tree is ability/buff plumbing) - so it is kept
//  as a thunk to its real, unhooked body rather than reconstructed.
//============================================================================
class CUnit { public: int QueryVisibleImpl(int which); };
class CAgent;   // Agent/agent.h - only used here as a pointer return type

//  0x6F265ED0 - Item/ability_slots.cpp's own declaration; re-declared here
//  because that file does not expose it through a header (a second
//  declaration with the identical signature is the same symbol).
extern void* __fastcall LookupAbilityTypeRecord(unsigned int typeId, int selector);

//  0x6F0098F0 - Game/objectdatatable.h.
int __fastcall RowTableLookup(int column);

//  0x6F1D6180 - 'ACsp', a bare FourCC constant with no further business
//  name recoverable from this call tree.  Defined in
//  Misc/cunit_agent1_leaf_getters.cpp rather than here: a single-constant-
//  return leaf in the *same* translation unit as its only caller gets
//  inlined by this build's /O2 (measured - see that file's own copy of
//  this leaf), which turns the shipped real `call` into an immediate load
//  and costs the caller two instructions it should keep.
extern unsigned int GetFourCC_6F1D6180();

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  0x6F1FA010 - the 'ACsp' instantiation of the CAgentPtr_<tag>/
//  MakeCheckedAgent_<tag> family cunit_agent1_makeagent.cpp and
//  cunit_agent1_agentptr_family.h document in full. Was kept as a thunk to
//  its real, unhooked body here; a later CUnit-vtable-closure session gave
//  it a real definition in cunit_agent1_makeagent.cpp alongside its 37
//  siblings, so this is now a plain extern declaration against that real
//  body rather than a second thunk to the same address (two definitions
//  of the same symbol would be an LNK2005, and a differently-named thunk
//  would leave one shipped address with two disconnected C++ bodies).
extern CAgent* MakeCheckedAgent_ACsp();

//  0x6F079990 - the ability-installation dispatcher itself (see the file
//  header above for why it stays a thunk).  `retn 4`.
struct SUnitAbilityInstallLeaf { void __thiscall InstallAbilityAgent(void* agent); };
ADDR_THUNK(void SUnitAbilityInstallLeaf::InstallAbilityAgent(void* agent), 0x6F079990)

#undef ADDR_THUNK

//  The made agent's own vtable slot 0x80 (+0x80/4 = 32): (this-of-outer,
//  pointer into the ability type record's +0x18).  Argument count and
//  types come straight off this function's own dump (two explicit pushes
//  before the indirect `call`), not a guess.
typedef void (__thiscall *AgentSlot0x80Fn)(void* self, CUnit* unit, void* recordField18);

struct SApplyAbilityEffectLeaf { void __thiscall ApplyAcspAbility(); };

//  0x6F1FC130
void __thiscall SApplyAbilityEffectLeaf::ApplyAcspAbility()
{
    CUnit* unit = (CUnit*)this;

    if (unit->QueryVisibleImpl(1) != 0)
        return;

    void* record = LookupAbilityTypeRecord(GetFourCC_6F1D6180(), 0);
    if (record == 0)
        return;

    if (RowTableLookup(*(int*)((char*)record + 0x1F8)) == 0)
        return;

    void* agent = MakeCheckedAgent_ACsp();
    ((AgentSlot0x80Fn)(*(void***)agent)[0x80 / 4])(agent, unit, (char*)record + 0x18);

    ((SUnitAbilityInstallLeaf*)unit)->InstallAbilityAgent(agent);
}
