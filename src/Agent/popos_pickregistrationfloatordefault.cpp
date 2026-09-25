//============================================================================
//  0x6F4A71F0 - NIpse::PickRegistrationFloatOrDefault_0xC, CPoPos::Apply's
//  own "resolve or default" helper (popos_apply_slot4.cpp), `req + 0xC`.
//  Same hidden-pointer-return shape poposbh.h's own
//  PickRegistrationFloatOrDefault_0x10/0x14 establish (and share the SAME
//  default global, g_unk6FAAE624) - just this class's own field offset
//  into `req`.
//
//  Own translation unit, deliberately NOT popos_apply_slot4.cpp (this
//  function's only caller in this dump, CPoPos::Apply): making this body
//  visible there let /Ob2 inline it into Apply and replace the real
//  out-of-line `call NIpse::PickRegistrationFloatOrDefault_0xC` the
//  shipped Apply body has with an inlined copy - the same "put a callee in
//  its own TU rather than reaching for `__declspec(noinline)`" fix
//  Pathfinding/pmregionpoolalloc.cpp and Pathfinding/pathrefallocator.cpp
//  both needed for the same reason elsewhere in this same follow-up.
//============================================================================
struct SHandleRegistrationRequest;

extern const unsigned int g_unk6FAAE624;

namespace NIpse {

unsigned int* __fastcall PickRegistrationFloatOrDefault_0xC(
    unsigned int* out, const SHandleRegistrationRequest* req)
{
    unsigned int temp;
    const unsigned int* src = &temp;
    if (req == 0)
        src = &g_unk6FAAE624;
    else
    {
        temp = *(const unsigned int*)((const char*)req + 0xC);
        src = &temp;
    }
    *out = *src;
    return out;
}

}  // namespace NIpse
