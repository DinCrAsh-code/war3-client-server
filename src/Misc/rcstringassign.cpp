//============================================================================
//  0x6F4C45A0 - RCString::Assign.
//============================================================================
#include "rcstring.h"

RCString* RCString::Assign(const RCString* other)
{
    RCSTRINGDATA* taken = other->m_data;
    if (taken != 0)
        taken->m_refcount++;

    RCSTRINGDATA* mine = m_data;
    if (mine != 0)
    {
        typedef void (__thiscall *ReleaseFn)(void*);
        ((ReleaseFn)mine->m_vtable[2])(mine);
        m_data = other->m_data;
        return this;
    }

    m_data = other->m_data;
    return this;
}
