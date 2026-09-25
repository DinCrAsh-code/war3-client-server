//============================================================================
//  NIpse::CPrClusterGen - vtable slots 3/4 (Apply/Slot4), the cluster-D
//  follow-up session (2026-09-12).  See prcluster.h's own per-method
//  comments for the full per-slot writeup; this file carries only the
//  bodies.
//
//  Own translation unit: ResetRefSlots/AssignField0F8 (prclustergen_helpers.cpp)
//  and SHandleTableEntry::RegisterOrReuse/UnregisterAndNotify
//  (Pathfinding/handletable.h) all stay real calls, not inlines.
//============================================================================
#include "prcluster.h"
#include "handletable.h"
#include "floatmini.h"

namespace NIpse {

//  0x6F48F0D0
void CPrClusterGen::Apply(SHandleRegistrationRequest* arg)
{
    ((SHandleTableEntry*)this)->RegisterOrReuse(arg);

    m_field0E0 = (unsigned int)-1;
    m_field0DC = (unsigned int)-1;
    CFloat initVal = *(const CFloat*)&g_unk6FAB73F4;
    m_field0E8 = initVal;
    m_field0EC = initVal;
    m_field0F0 = 0;
    m_field0F4 = 0;
    m_field0F8 = 0;
    m_field0FC = *(const CFloat*)&g_unk6FAAE624;
    m_field100 = 0;

    ResetRefSlots();
}

//  0x6F48F130 - two tail calls, no real body of its own.
void CPrClusterGen::Slot4(int arg)
{
    AssignField0F8(0);
    ((SHandleTableEntry*)this)->UnregisterAndNotify(arg);
}

}  // namespace NIpse
