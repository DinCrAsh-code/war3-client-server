//============================================================================
//  NIpse::CPoPosBh - vtable slots 3 (Apply) and 4 (Slot4), plus Apply's own
//  two "resolve or default" helper leaves. See poposbh.h for the
//  class-level header comment.
//============================================================================
#include "poposbh.h"
#include "spatialgrid.h"
#include "bhpoprojectile.h"

extern const unsigned int g_unk6FAAE624;

//  Already-declared globals elsewhere in this repo (Item/item_createatposition.cpp,
//  Item/ability_typemigrate.cpp, Pathfinding/pathroutepoints.cpp) - same
//  extern-prototype-in-every-consuming-TU convention this whole codebase
//  already uses for shared data.
extern const CFloat g_unk6FAAE4A0;   // dword_6FAAE4A0
extern const CFloat g_unk6FAAE4E8;   // dword_6FAAE4E8
extern const float         g_slopeThreshold0; // flt_6FAAE4D4

//  Two genuinely new globals this session's own dump reaches for the
//  first time.
extern const CFloat g_unk6FAAE4B4;
extern const CFloat g_unk6FAAE4FC;

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F4AAB70 - Apply's own "resolve or default" helper, `req + 0x10`.
//----------------------------------------------------------------------------
unsigned int* __fastcall PickRegistrationFloatOrDefault_0x10(
    unsigned int* out, const SHandleRegistrationRequest* req)
{
    if (req == 0)
    {
        *out = g_unk6FAAE624;
        return out;
    }
    *out = *(const unsigned int*)((const char*)req + 0x10);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F4AABB0 - same shape, `req + 0x14`.
//----------------------------------------------------------------------------
unsigned int* __fastcall PickRegistrationFloatOrDefault_0x14(
    unsigned int* out, const SHandleRegistrationRequest* req)
{
    if (req == 0)
    {
        *out = g_unk6FAAE624;
        return out;
    }
    *out = *(const unsigned int*)((const char*)req + 0x14);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F4AB390 - vtable slot 3, "Apply". Base `CPoPos::Apply` first, then
//  stamps +0xC0 with a fixed constant, resolves +0xC4/+0xC8 from `arg`
//  (or shared defaults), stamps +0xCC/+0xD0/+0xD4/+0xD8/+0xDC/+0xE0 with
//  more fixed constants (the same `g_CFloatZero` bit pattern for both
//  +0xD4 and +0xD8), then resamples the sync channel against the +0xC0
//  CFloat.
//----------------------------------------------------------------------------
void CPoPosBh::Apply(SHandleRegistrationRequest* arg)
{
    ((CPoPos*)this)->Apply(arg);

    m_pdl = g_unk6FAAE4A0;

    PickRegistrationFloatOrDefault_0x10((unsigned int*)&m_forField, arg);
    PickRegistrationFloatOrDefault_0x14((unsigned int*)&m_rot, arg);

    m_acc = g_unk6FAAE4E8;
    *(float*)&m_ang = g_slopeThreshold0;
    m_lvl = g_CFloatZero;
    m_ldl = g_CFloatZero;
    m_brk = g_unk6FAAE4B4;
    m_prw = g_unk6FAAE4FC;

    ((CBhPoProjectile*)this)->ResampleSyncChannel((const CFloat*)&m_pdl);
}

//----------------------------------------------------------------------------
//  0x6F4AB430 - vtable slot 4, "Slot4". LEFT TODO this session (not even a
//  THUNK - unclaimed): it tears down the +0xA8/+0xB8 related-agent
//  pointer array via 0x6F4AB2E0 (itself calling a `CDynTable`-shaped
//  shrink at 0x6F4AB0D0 and a rebalance at 0x6F4AAC40 - both genuinely
//  new, several-instruction leaves this session did not trace to a
//  confident real signature) before clearing its own +0xBC registration
//  (0x6F4AB130) and tail-calling the base `CPoPos::Slot4`. A wrong guess
//  at the array's own element shape here is exactly the kind of
//  "compiles and looks plausible" trap pipeline/CLAUDE.md's "Correctness,
//  not just shape" section warns about, so this is left for a follow-up
//  session with more room to trace 0x6F4AB0D0/0x6F4AAC40 properly rather
//  than guessed at.
//----------------------------------------------------------------------------

}  // namespace NIpse
