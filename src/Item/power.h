//============================================================================
//  CPower: directly below CAbilityInterfaced in the real RTTI chain
//  (`CPower: CAbilityInterfaced, CAbility, CUnitRefList, CAgentWar3,
//  CAgent, CObserver, TRefCnt`, class_informer_dump.tsv).  Discovered while
//  fixing CAbilityButton's own hierarchy (was `CAbilityButton : CAbility`,
//  missing both CPower and CAbilityInterfaced entirely -
//  CAbilityButton: CPower, CAbilityInterfaced, CAbility, ...).
//
//  Adds no fields of its own.  There is no dedicated CPower constructor
//  address anywhere in agent_worktrees/funcs/ or asm/ - `grep -rl
//  "6F87EA44" agent_worktrees/funcs/` is empty - which, per this class's own
//  CAbilityInterfaced precedent, means it is a pure vtable-stamp-only
//  trivial constructor, fully inlined into whichever derived constructor
//  actually runs.  Confirmed directly: CAbilitySpell::CAbilitySpell
//  (0x6F068DF0, abilitybutton.h's own root) calls sub_6F052190
//  (CAbility::CAbility) and then goes STRAIGHT to
//  `mov dword ptr [esi], offset ??_7CAbilityButton@@6B@` - no intermediate
//  CPower or CAbilityInterfaced vtable stamp appears at all, and the first
//  field write after that call is `[esi+6Ch]`, exactly where
//  CAbilityButton's own four fields begin (abilitybutton.h) and exactly
//  where CAbility's own layout ends (ability.h: "ends the class at +0x6C").
//  No write anywhere in that constructor's own range between CAbility's end
//  and CAbilityButton's own +0x6C, so CPower (like CAbilityInterfaced)
//  contributes zero fields and MSVC elides its own redundant vtable stamp
//  as a dead store - the same elision abilitybutton.h's own header comment
//  already documents for CAbilityButton's own stamp.
//
//  Vtable: 194 slots (docs/targets/vtables/CPower.txt), four more than
//  CAbilityInterfaced's own 190.  Diffed index-by-index against
//  CAbilityInterfaced's own committed dump
//  (agent_worktrees/classes/0x6F87EA44.json vs 0x6F878F2C.json): CPower
//  overrides exactly four of CAbilityInterfaced's own slots -
//    1  (+0x04) DeleteSelf            - covariant return, CPower*
//    7  (+0x1C) GetAgileTypeId        - 'powr' (a different family tag)
//    22 (+0x58) GetClassName          - "CPower"
//    148/151/152/153 (+0x250/+0x25C/+0x260/+0x264) - four near-identical
//      "ask this instance's own new leaf slot first, fall back to the
//      inherited CAbility::EvaluateDepCheckAlias(0) otherwise" dispatchers -
//      see power_slots.cpp's own header for the full derivation
//  and adds exactly four new slots at the end (190-193, +0x2F8/+0x2FC/
//  +0x300/+0x304) - CPower's own default answer for each of the four
//  dispatchers above, always `mov eax, 0xDD; retn <n>` in this class
//  (a derived class, e.g. CAbilityButton, is expected to override one or
//  more of these with a real per-instance answer - none of the four
//  overrides above ever compute 0xDD themselves, so CPower's own default
//  is exactly this "not overridden" leaf).  Every other slot outside those
//  eight is byte-identical between the two dumps, confirmed by the same
//  index-by-index diff, so this class inherits everything else unchanged.
//============================================================================
#ifndef POWER_H
#define POWER_H

#include "abilityinterfaced.h"

class CPower : public CAbilityInterfaced
{
public:
    //  No dedicated constructor address exists - see header comment above.
    //  Declared only so a derived class's own constructor has something to
    //  call; never itself compiled as a distinct reconstructed body.
    CPower() { }

    //  slot 1 / +0x04 (0x6F05F8C0) - covariant return, same shape as
    //  CAbilityInterfaced::DeleteSelf.  The "effective destructor" the dump
    //  calls is CAbility::~CAbility() directly (0x6F038580, confirmed via
    //  its own agent_worktrees/funcs/ symbol) - CPower adds no fields, so
    //  there is nothing of its own (or CAbilityInterfaced's) to unwind.
    //  See power_deleteself.cpp.
    virtual CPower* DeleteSelf(int flags);

    //  slot 7 / +0x1C (0x6F0293D0) - `mov eax, 706F7772h; retn`, the
    //  multichar literal 'powr' (p=0x70,o=0x6F,w=0x77,r=0x72).  See
    //  power.cpp.
    virtual unsigned int GetAgileTypeId() const;

    //  slot 22 / +0x58 (0x6F029380).  See power.cpp.
    virtual const char* GetClassName();

    //  slots 148/151/152/153 - overrides of CAbility's own
    //  Method_6F052850/Method_6F052820/Method_6F052810/Method_6F052800
    //  (ability.h): same name and signature as the base declares them,
    //  which is what lands the override at the base's own slot instead of
    //  appending a new one (the same C++ rule ability.h's own "full
    //  virtual-flip" note already spells out for CAbility/CAgentWar3).
    //  Each tries this instance's own new leaf slot first (190/192/191/193
    //  respectively - see power_slots.cpp's own header for exactly how the
    //  four dispatchers pair up with the four leaves) and, only if that
    //  answers 0, falls back to the inherited
    //  CAbility::EvaluateDepCheckAlias(0), translating its 0/nonzero result
    //  through the same "neg/sbb/and/add" ternary lowering into 0xB8 (not
    //  found) or 0 (found).  See power_slots.cpp.
    virtual int Method_6F052850(int, int, int, int);
    virtual int Method_6F052820(int, int, int, int, int);
    virtual int Method_6F052810(int, int, int, int);
    virtual int Method_6F052800(int, int, int);

    //  slots 190-193 / +0x2F8/+0x2FC/+0x300/+0x304 - CPower's own four new
    //  slots, past CAbilityInterfaced's own 190.  Each is the plain
    //  "not overridden" default the four dispatchers above call through:
    //  a bare `mov eax, 0xDD; retn <n>`, differing only in how many stack
    //  dwords each one's own caller (above) actually forwards.  Named by
    //  offset per this file's own established Method_0xNN convention
    //  (abilityinterfaced.h/ability.h) since nothing in this call tree
    //  identifies what any of the four individually answer - only that a
    //  derived class overriding one supplies a real answer in place of
    //  0xDD.  See power_slots.cpp.
    virtual int Method_0x2F8(int, int);
    virtual int Method_0x2FC(int, int);
    virtual int Method_0x300(int, int, int);
    virtual int Method_0x304(int);
};

#endif
