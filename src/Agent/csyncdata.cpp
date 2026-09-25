//============================================================================
//  NTempest::CSyncData - vtable slots 0 (ScalarDeletingDestructor), 1
//  (GetChecksumField) and 2 (ClearFields). See csyncdata.h for the
//  class-level header comment. Slot 4 (DumpDebugState) is its own
//  translation unit (csyncdata_dumpdebugstate.cpp).
//============================================================================
#include "csyncdata.h"
#include "storm.h"   // SMemFree (Storm_403)

namespace NTempest {

//  0x6F53D690 - vtable slot 0, scalar deleting destructor. Restamps
//  straight to the base NTempest::CEntity's own vftable
//  (centity_dtor.cpp's own 0x6F951784) with no real ~CSyncData() call.
void* CSyncData::ScalarDeletingDestructor(unsigned int flags)
{
    *(void**)this = (void*)0x6F951784;   // ??_7CEntity@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

//  0x6F536BE0 - vtable slot 1.
unsigned int CSyncData::GetChecksumField()
{
    return m_field4;
}

//  0x6F536C10 - vtable slot 2.
void CSyncData::ClearFields()
{
    m_field4 = 0;
    m_count = 0;
}

//  0x6F536C20 - vtable slot 3 (nullsub_1539). Genuinely its own, distinct
//  one-instruction no-op - see csyncdata.h's own declaration comment.
void CSyncData::Slot3()
{
}

}  // namespace NTempest
