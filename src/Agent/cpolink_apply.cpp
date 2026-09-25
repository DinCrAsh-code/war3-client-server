//============================================================================
//  0x6F4AA130 - NIpse::CPoLink::Apply, vtable slot 3.
//
//  CRlProperty::Apply(arg) first, then a lookup+fixup: `arg`'s own +0x0C
//  field (SPoLinkTargetSource::QueryTarget, cpolink_querytarget.cpp - its
//  own translation unit, so /Ob2 cannot inline it into this call site the
//  way it did on the first attempt here) is itself a pointer; on a hit,
//  this class's own m_targetHandle/m_targetSub are copied from that
//  pointer's own +0x14/+0x18 fields, on a miss (null) both are set to -1.
//============================================================================
#include "cpolink.h"
#include "rlproperty.h"

namespace NIpse {

//  Declared here to match cpolink_querytarget.cpp's own definition.
struct SPoLinkTargetSource
{
    void* QueryTarget();
};

void CPoLink::Apply(SHandleRegistrationRequest* arg)
{
    ((CRlProperty*)this)->Apply(arg);

    void* target = ((SPoLinkTargetSource*)arg)->QueryTarget();
    if (target != 0)
    {
        m_targetHandle = *(unsigned int*)((char*)target + 0x14);
        m_targetSub = *(unsigned int*)((char*)target + 0x18);
    }
    else
    {
        m_targetSub = 0xFFFFFFFF;
        m_targetHandle = 0xFFFFFFFF;
    }
}

}  // namespace NIpse
