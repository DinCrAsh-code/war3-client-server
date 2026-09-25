//============================================================================
//  0x6F2BCC50 - CMissileThunderBoltRefs::Apply, the gated "deal damage"
//  step DealDamage (missile_dealdamage.cpp) hands its computed impact
//  point and gate flag to.  CMissileThunderBolt impact pipeline, see
//  docs/targets/cmissilethunderbolt_impact_pipeline.md.
//
//  Best-effort, read in full but not yet verified against a build: builds
//  two locals from missile fields (+0x8C, +0x54) and copies the 3-float
//  impact point out of its own `point` argument, then hands nine
//  arguments to sub_6F47A650(missile+0x78) - not itself reconstructed,
//  its own body unexamined this session - and logs
//  "Can't hit that target!" through sub_6F4CBF60 if it answers zero.
//  sub_6F47A650's real signature is inferred purely from this call site's
//  argument setup, not from its own body.
//============================================================================
#include "missile_thunderbolt.h"

struct SFieldPair { void* field8C; void* field54; };

//  sub_6F47A650 - the actual hit-validation query, called on missile+0x78.
//  Not reconstructed; signature inferred from this call site only.  Naked
//  jmp thunk to the real shipped address (same pattern as
//  src/unreconstructed_thunks.cpp's MEMBER_THUNK / misc_adjuster_thunks.cpp)
//  so build_mix.py's full-DLL link resolves it - verify.py never links, so
//  a declared-only extern compiles clean here and only fails there.
struct SHitValidator
{
    int __thiscall Validate(void* arg0, CFloat px, CFloat py, CFloat pz,
                             void* field54Ptr, unsigned int magic, void* missile,
                             SFieldPair* pair, int flag);
};
__declspec(naked) int __thiscall SHitValidator::Validate(void*, CFloat, CFloat, CFloat,
                                                          void*, unsigned int, void*,
                                                          SFieldPair*, int)
{
    __asm { mov eax, 0x6F47A650 }
    __asm { jmp eax }
}

//  0x6F4CBF60 - posts a line into the system-message list (here,
//  "Can't hit that target!"), __fastcall(ecx=message, edx=type).  It is a
//  real reconstruction now (Misc/sysmessagepost.cpp); the naked redirect
//  that used to sit here is gone.  The value is `hit + 3` - hit is always
//  0 on this path (the `!hit` guard), so it is always 3 in practice, but
//  the shipped code computes it from `hit` rather than a literal, so the
//  source likely does too.
//
//  **Not `extern "C"` any more.**  A C-linkage symbol carries no readable
//  name beside its EXTRN in a /FA listing, so once the address gained a
//  funcmap.py entry the target's `call sub_6F4CBF60` would have resolved to
//  `PostSysMessage` while this side stayed an anonymous `SYM` - one
//  instruction lost at every call site in the repo
//  (docs/msvc-vc8-idioms.md, "A C-linkage symbol has no readable name in
//  the listing").
int __fastcall PostSysMessage(const char* message, int value);

void __thiscall CMissileThunderBoltRefs::Apply(void* arg0, SImpactPoint* point, int flag)
{
    SFieldPair pair;
    pair.field8C = *(void**)((char*)this + 0x8C);
    pair.field54 = *(void**)((char*)this + 0x54);

    SImpactPoint localPoint = *point;

    int hit = ((SHitValidator*)((char*)this + 0x78))
                  ->Validate(arg0, localPoint.x, localPoint.y, localPoint.z,
                             &pair.field54, 0xD019C, this, &pair, flag);

    if (!hit)
        PostSysMessage("Can't hit that target!", hit + 3);
}
