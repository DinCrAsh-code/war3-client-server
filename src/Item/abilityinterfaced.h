//============================================================================
//  CAbilityInterfaced: the class this build's own RTTI (`.data:6FA6DBE0`'s
//  inheritance comment, supplied directly) puts directly on top of CAbility -
//  `CAbilityInterfaced : CAbility : CUnitRefList : CAgentWar3 : CAgent :
//  CObserver : TRefCnt`, with CPower and CAbilityButton further up (not
//  reconstructed here; ability.h's own note on the four "near slot 32"
//  default-answer slots already flagged their existence before this class
//  itself was reconstructed).
//
//  Adds no fields of its own: 0x6F0539D0 (the constructor) writes nothing
//  past CAbility::CAbility's own call and this class's own vtable stamp -
//  the same "pure vtable layer" shape CUnitRefList/CAgentWar3 already
//  establish one level down.
//
//  Vtable: 190 slots (docs/targets/vtables/CAbilityInterfaced.txt), one more
//  than CAbility's own 189 - overrides six of CAbility's slots (1, 7, 22, 41,
//  56, 57) and adds exactly one new one at the end (189).  Confirmed against
//  CAbility's own table: every slot outside those seven is byte-identical
//  between the two dumps, so this class inherits everything else unchanged -
//  C++'s default behaviour for a virtual this class does not override.
//============================================================================
#ifndef ABILITYINTERFACED_H
#define ABILITYINTERFACED_H

#include "ability.h"

class CAbilityInterfaced : public CAbility
{
public:
    //  0x6F0539D0 - see abilityinterfaced.cpp.
    CAbilityInterfaced();

    //  slot 1 / +0x04 (0x6F0539F0) - covariant return, same shape as
    //  CAbility::DeleteSelf.  See abilityinterfaced_deleteself.cpp.
    virtual CAbilityInterfaced* DeleteSelf(int flags);

    //  slot 7 / +0x1C (0x6F025D10) - a different family tag than CAbility's
    //  own 'abil' (ability_slots.cpp).  See abilityinterfaced.cpp.
    virtual unsigned int GetAgileTypeId() const;

    //  slot 22 / +0x58 (0x6F025CC0).  See abilityinterfaced.cpp.
    virtual const char* GetClassName();

    //  slot 41 / +0xA4 (0x6F060A30) - overrides CAbility::Method_6F05F550:
    //  calls the base version, then this class's own new slot 189 below.
    //  See abilityinterfaced_slots.cpp.
    virtual void Method_6F05F550();

    //  slot 56 / +0xE0 (0x6F025D40) - overrides CAbility::Method_0xE0
    //  (a nullsub there): calls slot 189 with argument 0.
    //  slot 57 / +0xE4 (0x6F025D30) - overrides CAbility::Method_0xE4
    //  (a nullsub there): calls slot 189 with argument 1.
    //  See abilityinterfaced_slots.cpp.
    virtual void Method_0xE0();
    virtual void Method_0xE4();

    //  slot 189 / +0x2F4 (0x6F021750) - this class's own new slot, past
    //  CAbility's own 189.  A real, empty body in the shipped image (a bare
    //  `retn 4`, confirmed via tools/ida_query.py bytes) - not a guessed
    //  placeholder.  Named by offset per this file's own Method_0x24/
    //  Method_0x5C convention (ability.h), since nothing in this call tree
    //  identifies what it once did; the three callers above all reach it
    //  only through this class's own vtable, never by name.
    virtual void Method_0x2F4(int flag) { }
};

#endif
