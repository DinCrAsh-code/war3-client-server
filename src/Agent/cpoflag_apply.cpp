//============================================================================
//  0x6F4A8A00 - NIpse::CPoFlag::Apply, vtable slot 3.
//
//  Tail-reaches CRlProperty::Apply (Agent/rlproperty.h, `this` reinterpreted
//  the same layout-compatible way rlproperty_apply.cpp's own body already
//  does), then queries a value off `arg` via SPoFlagValueSource::QueryValue
//  (cpoflag_queryvalue.cpp - its own translation unit, so /Ob2 cannot inline
//  it into this call site the way it did on the first attempt here) and
//  stores it into this class's own m_value.
//============================================================================
#include "cpoflag.h"
#include "rlproperty.h"

namespace NIpse {

//  Declared here to match cpoflag_queryvalue.cpp's own definition; not part
//  of CPoFlag's own public interface.
struct SPoFlagValueSource
{
    int QueryValue();
};

void CPoFlag::Apply(SHandleRegistrationRequest* arg)
{
    ((CRlProperty*)this)->Apply(arg);
    m_value = ((SPoFlagValueSource*)arg)->QueryValue();
}

}  // namespace NIpse
