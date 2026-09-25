//============================================================================
//  0x6F4AB250 - NIpse::CPoPosBh::Slot18Helper. See poposbh.h's own header
//  comment. Own translation unit, separate from CPoPosBh::Slot18
//  (poposbh_slot18_outer.cpp) - the same "a small single-call-site body
//  shares a TU with its own caller" trap this repo's own house rules
//  warn about (popos_apply_slot4.cpp's own history, among others).
//============================================================================
#include "poposbh.h"

namespace NIpse {

void CPoPosBh::Slot18Helper()
{
    ClearBcRegistration();

    if (Slot18Gate() != 0)
        ((CBhPoProjectile*)this)->ResampleSyncChannel((const CFloat*)&m_pdl);
}

}  // namespace NIpse
