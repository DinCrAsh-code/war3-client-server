//============================================================================
//  0x6F4AB430 - NIpse::CPoPosBh::Slot4, vtable slot 4. See poposbh.h for the
//  class-level header comment and poposbh_relatedagents.cpp for
//  TeardownRelatedAgents/ClearBcRegistration, both real calls this makes.
//
//  Own translation unit, deliberately: putting this alongside
//  TeardownRelatedAgents/ClearBcRegistration in one TU let /Ob2 inline
//  ClearBcRegistration's own six-instruction body straight into Slot4 (it
//  scores 1.000 EXACT on its own, called from here it silently vanished as
//  a real `call`) - the exact "split a small callee to its own TU" trap
//  pipeline/CLAUDE.md's own house rules warn about, and the same fix
//  `CPoPos::Apply`'s own follow-up hit three times.
//============================================================================
#include "poposbh.h"

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F4AB430 - vtable slot 4, "Slot4". Sets the 0x1000000 flags bit (+0x4C,
//  the same field every sibling in this chain's own Slot4 touches), tears
//  down the related-agent array WITHOUT shrinking it
//  (`TeardownRelatedAgents(notify=1, shrink=0)`), clears this object's own
//  +0xBC sync registration, then tail-calls the base `CPoPos::Slot4`.
//----------------------------------------------------------------------------
void CPoPosBh::Slot4(int arg)
{
    *(unsigned int*)((char*)this + 0x4C) |= 0x1000000;

    TeardownRelatedAgents(1, 0);
    ClearBcRegistration();

    ((CPoPos*)this)->Slot4(arg);
}

}  // namespace NIpse
