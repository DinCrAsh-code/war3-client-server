//============================================================================
//  0x6F4C48F0 - RCString::~RCString.  Re-stamp this class's own vtable,
//  Reset, release whatever Reset left, then re-stamp the base's.
//
//  Written with explicit stores rather than as a C++ destructor because
//  this build emits neither vtable: with /GR- off and no virtual member
//  declared here, `??_7RCString@@6B@` has nothing to point at, so the two
//  stamps are bare symbol stores and the compare canonicalises both sides
//  to `mov [reg], SYM`.
//============================================================================
#include "rcstring.h"

RCString::~RCString()
{
    m_vtable = (void**)g_vftableRCString;

    Reset();

    RCSTRINGDATA* mine = m_data;
    if (mine != 0)
    {
        typedef void (__thiscall *ReleaseFn)(void*);
        ((ReleaseFn)mine->m_vtable[2])(mine);
    }

    m_vtable = (void**)g_vftableTRefCnt;
}
