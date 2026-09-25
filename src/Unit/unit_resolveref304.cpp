//============================================================================
//  0x6F275D90 - CUnit's slot-3 closure leaf: resolve m_ref304 (unit.h,
//  `SRefMiniValue`, the same {t,b} pair layout QueryHandleField0x54's own
//  SOptionalHandleRef takes) if it is set, or return null.  `retn 0`.
//============================================================================
#include "unit.h"
#include "itemhandleresolve.h"

void* CUnit::ResolveRef304()
{
    SOptionalHandleRef* ref = (SOptionalHandleRef*)&m_ref304;

    if ((ref->m_typeTag & (int)ref->m_handle) == -1)
        return 0;

    return QueryHandleField0x54(ref);
}
