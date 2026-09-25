//============================================================================
//  0x6F2757B0 - SCheckedAqueSlot::Assign (agenttypedslots.h).  No SEH frame
//  around this address at all - unlike every converting constructor in this
//  file's own family, Assign itself never opens one, so this is a plain,
//  fully reproducible function.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

//  0x6F0278B0 - `mov eax, 41717565h / retn`, 'Aque'.
static unsigned int GetFourCC_6F0278B0()
{
    return 0x41717565u;
}

SCheckedAqueSlot* SCheckedAqueSlot::Assign(CAgent* candidate)
{
    //  The required tag is fetched into a local *before* the candidate's
    //  own virtual GetAgileTypeId() - the dump's own `call
    //  GetFourCC_6F0278B0` precedes the virtual dispatch - rather than
    //  passed as an inline argument expression, which left the compiler
    //  free to reschedule the (trivially inlinable, same-TU) constant
    //  after the virtual call instead.
    unsigned int required = GetFourCC_6F0278B0();
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), required))
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
