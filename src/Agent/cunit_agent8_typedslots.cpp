//============================================================================
//  Three more instances of the type-checked "smart pointer to a
//  CAgent-derived object" family agenttypedslots.h documents (see its own
//  header comment) - byte-for-byte the same body as
//  SCheckedUnitSlot::Assign (unittypedassign.cpp) with the required FourCC
//  swapped, at addresses that BFS from CUnit's own vtable (rather than the
//  JASS handle-resolution sweep that found the other eighteen) reached.
//  None of the three has an established real class - each checks against
//  a plain-looking (not '+'-prefixed) rawcode nothing else in this repo has
//  named yet, so each keeps its literal FourCC per misc_rawcode_getters.cpp's
//  own rule.
//
//  GetFourCC_6F10FA90 already exists (unit_flagbit8.cpp) but as a `static`
//  local to that translation unit - redeclared, not shared, since a static
//  has no external symbol to bind a second TU's call to and this session
//  does not own that file.  Same for the other two rawcode getters below:
//  each is its own two-instruction `mov eax, <imm32> / retn` leaf
//  (0x6F0D9120 'AEbu', 0x6F028740 'Asel') that is not itself in this
//  agent's worklist slice, so it is declared here rather than defined.
//============================================================================
#include "agent.h"
#include "agiletype.h"

extern unsigned int GetFourCC_6F10FA90();   //  0x6F10FA90 - 'Avul'.
extern unsigned int GetFourCC_6F0D9120();   //  0x6F0D9120 - 'AEbu'.
extern unsigned int GetFourCC_6F028740();   //  0x6F028740 - 'Asel'.

struct SCheckedAvulSlot
{
    SCheckedAvulSlot* Assign(CAgent* candidate);
    CAgent* m_value;
};

struct SCheckedAEbuSlot
{
    SCheckedAEbuSlot* Assign(CAgent* candidate);
    CAgent* m_value;
};

struct SCheckedAselSlot
{
    SCheckedAselSlot* Assign(CAgent* candidate);
    CAgent* m_value;
};

//  0x6F075DF0
SCheckedAvulSlot* SCheckedAvulSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F10FA90()))
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

//  0x6F275530
SCheckedAEbuSlot* SCheckedAEbuSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F0D9120()))
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

//  0x6F280F00 - the constructor half of one more instance of the same
//  family: zero, then Assign(candidate), matching sub_6F220A00's own body
//  (cunit_agent8_abilityfactories.cpp). Its own Assign (0x6F275730,
//  checking against 'ANbu' via 0x6F0D8FA0) is not this agent's own
//  address, so it is declared here, not defined.
struct SCheckedANbuSlot
{
    SCheckedANbuSlot(CAgent* candidate);
    SCheckedANbuSlot* Assign(CAgent* candidate);   // 0x6F275730 - not this agent's address
    CAgent* m_value;
};

SCheckedANbuSlot::SCheckedANbuSlot(CAgent* candidate)
{
    m_value = 0;
    Assign(candidate);
}

//  0x6F275930
SCheckedAselSlot* SCheckedAselSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F028740()))
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
//  Link-completion thunks - each of these addresses was declared (not
//  defined) elsewhere in this file because comments record it as another
//  parallel agent's own worklist target, but the two agents independently
//  invented different class/struct names for the same shared address, so
//  the declaring extern reference never actually resolves against the
//  sibling's real (differently-named) reconstruction. Defining a plain
//  redirect thunk here - the same `mov eax,<addr>/jmp eax` idiom this
//  repo already uses for any out-of-scope callee - makes this TU link
//  without re-claiming the address (no funcmap.py row points at these).
//----------------------------------------------------------------------------
__declspec(naked) SCheckedANbuSlot* SCheckedANbuSlot::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F275730h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0D9120()
{
    __asm { mov eax, 06F0D9120h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F10FA90()
{
    __asm { mov eax, 06F10FA90h }
    __asm { jmp eax }
}
