//============================================================================
//  NIpse::CPoPosCl - vtable slot 3 (Apply), shared with
//  NTempest::CLinkedList<NIpse::CPoPosCl>. See poposcl.h for the
//  class-level header comment.
//============================================================================
#include "poposcl.h"

extern const unsigned int g_unk6FAAE4EC;   // flt_6FAAE4EC - no other user yet
extern const float        g_itemValueFloor; // flt_6FAAE4C4 (Unit/destructable_life.cpp)

//  Declared CFloat, not unsigned int, to agree with the other TUs already
//  extern-declaring these two (Item/ability_typemigrate.cpp,
//  poposbh_apply_slot4.cpp) - MSVC mangles a global's type into its own
//  name, so a mismatched declaration here would silently split into two
//  unresolved symbols for one address (link_check.py's own "SPLIT DATA
//  GLOBAL" finding). Read as raw bits below, same as g_itemValueFloor.
extern const CFloat g_unk6FAAE4E8;   // dword_6FAAE4E8
extern const CFloat g_unk6FAAE4FC;

namespace NIpse {

//  0x6F493790 - CPoPosCl_ResetTailPair, own translation unit
//  (poposcl_resettailpair.cpp) so /Ob2 cannot fold it into Apply's own
//  body the way it does within one TU.
void __fastcall CPoPosCl_ResetTailPair(CPoPosCl* self);

//----------------------------------------------------------------------------
//  0x6F494650 - vtable slot 3, "Apply". Base `CPoPos::Apply` first, then
//  +0xA0/+0x9C both to -1 (a "no slot" sentinel), three fixed-constant
//  stamps at +0xB0/+0xB4/+0xB8, four copies of the same `g_CFloatZero`
//  bit pattern at +0xCC/+0xC8/+0xC4/+0xC0, one more fixed constant at
//  +0xBC, a zero at +0xD8, and a final `-1`/`-1` pair at +0xD4/+0xD0
//  (0x6F493790, inlined here - too small to stay out of line at /Ob2).
//----------------------------------------------------------------------------
void CPoPosCl::Apply(SHandleRegistrationRequest* arg)
{
    ((CPoPos*)this)->Apply(arg);

    char* base = (char*)this;
    *(unsigned int*)(base + 0xA0) = 0xFFFFFFFF;
    *(unsigned int*)(base + 0x9C) = 0xFFFFFFFF;

    *(unsigned int*)(base + 0xB0) = g_unk6FAAE4EC;
    *(unsigned int*)(base + 0xB4) = *(const unsigned int*)&g_unk6FAAE4E8;
    *(unsigned int*)(base + 0xB8) = *(const unsigned int*)&g_itemValueFloor;

    unsigned int zero = g_CFloatZero.m_bits;
    *(unsigned int*)(base + 0xCC) = zero;
    *(unsigned int*)(base + 0xC8) = zero;
    *(unsigned int*)(base + 0xC4) = zero;
    *(unsigned int*)(base + 0xC0) = zero;

    *(unsigned int*)(base + 0xBC) = *(const unsigned int*)&g_unk6FAAE4FC;

    *(unsigned int*)(base + 0xD8) = 0;

    CPoPosCl_ResetTailPair(this);
}

}  // namespace NIpse
