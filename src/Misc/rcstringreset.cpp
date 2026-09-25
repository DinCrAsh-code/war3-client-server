//============================================================================
//  0x6F4C45E0 - RCString::Reset.  The shared empty buffer is never released;
//  anything else is, through its own vtable slot 2.
//============================================================================
#include "rcstring.h"

void RCString::Reset()
{
    if (!m_data->Equals(&g_emptyRCStringData))
        return;

    RCSTRINGDATA* mine = m_data;
    if (mine != 0)
    {
        typedef void (__thiscall *ReleaseFn)(void*);
        ((ReleaseFn)mine->m_vtable[2])(mine);
    }

    m_data = 0;
}
