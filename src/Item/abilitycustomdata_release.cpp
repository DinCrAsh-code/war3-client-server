//============================================================================
//  0x6F251950 - CCustomData::ReleaseOwnedRefAndFinalize(): release
//  m_ownedRef through its own vtable slot 0 if it is non-null, clear it,
//  then tail-call this object's own Method3() (vtable slot 3 - nullsub for
//  CCustomData itself, a real override for CAbilityCustomData).
//
//  Own translation unit for the same reason as abilitycustomdata_dtor.cpp:
//  ~CCustomData() must reach this through a real call, not an inlined body.
//============================================================================
#include "abilitycustomdata.h"

typedef void (__thiscall *ReleaseRefFn)(void*, int);

void CCustomData::ReleaseOwnedRefAndFinalize()
{
    if (m_ownedRef != 0)
    {
        ((ReleaseRefFn)(*(void***)m_ownedRef)[0])(m_ownedRef, 1);
        m_ownedRef = 0;
    }
    Method3();
}
