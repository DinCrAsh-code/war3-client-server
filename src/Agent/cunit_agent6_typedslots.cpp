//============================================================================
//  Two more instantiations of the "type-checked smart pointer to a
//  CAgent-derived object" family agenttypedslots.h documents (see that
//  header's own comment) - reached from CUnit's own vtable closure at BFS
//  depth 1-2.  Byte-for-byte the same body as SCheckedUnitSlot::Assign
//  (unittypedassign.cpp) with one operand changed: which fixed FourCC the
//  candidate's own virtual GetAgileTypeId() is checked against.
//
//  0x6F084EF0 checks 'Aatk' and 0x6F219380 checks 'AInv' - the same two
//  cached-ability tags CUnit::FindAbility already names
//  (unit_findability.cpp / unit_abilityshortcuts.cpp: `m_atk1E8` at 'Aatk',
//  `m_inv1F8` at 'AInv', the inventory ability), so these are named for the
//  ability they check rather than left address-derived, unlike a required
//  type this call tree gives no other evidence for.
//
//  Each gets its own translation unit, matching every other member of the
//  family (agenttypedslots.h's own rule) - and its own required-type getter
//  in a *separate* naked leaf, kept out of line so the optimiser cannot see
//  through it (there is only one call site here, but the shape matches
//  every sibling in this family for the same reason misc_rawcode_getters.cpp
//  gives).
//============================================================================
#include "agent.h"
#include "agiletype.h"

//----------------------------------------------------------------------------
//  0x6F0C4D10 - 'Aatk' (0x4161746B, MSB-first).  Not itself a worklist entry
//  (below this agent-6 slice's own BFS cut), but a bare
//  `mov eax,<imm32>; retn` leaf like every other required-type getter this
//  codebase already reconstructs for real (misc_rawcode_getters.cpp).
//----------------------------------------------------------------------------
unsigned int GetAttackAbilityFourCC_6F0C4D10()
{
    return 0x4161746B;
}

//  0x6F0DB030 - 'AInv' (0x41496E76, MSB-first).  Same shape.
unsigned int GetInventoryAbilityFourCC_6F0DB030()
{
    return 0x41496E76;
}

//----------------------------------------------------------------------------
//  0x6F084EF0 - the 'Aatk' slot's Assign.
//----------------------------------------------------------------------------
struct SCheckedAttackAbilitySlot_6F084EF0
{
    SCheckedAttackAbilitySlot_6F084EF0* Assign(CAgent* candidate);
    CAgent* m_value;
};

SCheckedAttackAbilitySlot_6F084EF0*
SCheckedAttackAbilitySlot_6F084EF0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate &&
        AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                GetAttackAbilityFourCC_6F0C4D10()))
        checked = candidate;
    else
        checked = 0;

    CAgent* old = m_value;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (candidate)
                candidate->m_refcount++;
        }
        m_value = checked;
    }

    return this;
}

//----------------------------------------------------------------------------
//  0x6F219380 - the 'AInv' slot's Assign.  Same body as 0x6F084EF0's own,
//  just the other cached ability tag.
//----------------------------------------------------------------------------
struct SCheckedInventoryAbilitySlot_6F219380
{
    SCheckedInventoryAbilitySlot_6F219380* Assign(CAgent* candidate);
    CAgent* m_value;
};

SCheckedInventoryAbilitySlot_6F219380*
SCheckedInventoryAbilitySlot_6F219380::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate &&
        AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                GetInventoryAbilityFourCC_6F0DB030()))
        checked = candidate;
    else
        checked = 0;

    CAgent* old = m_value;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (candidate)
                candidate->m_refcount++;
        }
        m_value = checked;
    }

    return this;
}
