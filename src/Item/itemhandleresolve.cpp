//============================================================================
//  See itemhandleresolve.h.
//============================================================================
#include "itemhandleresolve.h"

//----------------------------------------------------------------------------
//  0x6F4786B0.
//----------------------------------------------------------------------------
void* __fastcall QueryHandleField0x54(SOptionalHandleRef* ref)
{
    SItemHandleObject* object = (SItemHandleObject*)LookupHandle(ref->m_handle, ref->m_typeTag);
    if (!object || object->m_pendingFlag != 0)
        return 0;
    return object->m_field54;
}
