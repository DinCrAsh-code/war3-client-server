//============================================================================
//  0x6F29DE70 - `retn 4`, __fastcall(ecx=self, edx=other, stack=idRef):
//  resolve a config-registry CFloat by a raw 4CC/id into `*self` (through
//  sub_6F29BEA0 = ResolveConfigFloatById, defined in Unit/unittypequeries.cpp
//  - see the declaration below), then, if `other` is
//  non-null, scale `*self` by a raw CFloat bit pattern read out of
//  `other`'s own +0x29C and round the product with RoundCFloatHalfUp
//  (0x6F6EF330, Math/cfloatround.cpp) - the same CFloat::operator*
//  (0x6F6EEE20) plus round-half-up shape playerwar3_method0x0c.cpp's own
//  gold/lumber tax computation already uses.
//
//  Neither `self`'s nor `other`'s real class is evidenced in this call
//  tree (no caller is in it), so both are named by what this function
//  alone touches - a leading CFloat, and a raw float bit pattern at
//  +0x29C - rather than guessed further.
//============================================================================
#include "CFloat.h"

//  __fastcall, matching the real definition's own convention exactly - a
//  free function taking `const CFloat&`, not a member, so its mangled name
//  has to agree with cfloatround.cpp's or this becomes a second symbol
//  rather than the same one.
CFloat __fastcall RoundCFloatHalfUp(const CFloat& value);   // 0x6F6EF330, Math/cfloatround.cpp

struct SConfigFloatFieldHolder
{
    CFloat m_value;   // +0x00
};

struct SFieldAt0x29CHost
{
    char         m_reserved000[0x29C];
    unsigned int m_field29C;   // +0x29C - raw bits of a CFloat
};

//  0x6F29BEA0 - createunit-G1's own worklist target (2026-09-08), defined in
//  Unit/unittypequeries.cpp: it is that module's own probe over
//  g_unitTypeTable (dword_6FAB445C), not this file's - see that definition's
//  own comment for why the body lives there.  Declared here (matching that
//  file's SConfigFloatFieldHolder-by-name signature exactly) so this call
//  binds onto it rather than inventing a second symbol.
void* __fastcall ResolveConfigFloatById(SConfigFloatFieldHolder* out, unsigned int id);

SConfigFloatFieldHolder* __fastcall ComputeScaledRoundedFloat(
    SConfigFloatFieldHolder* self, SFieldAt0x29CHost* other, unsigned int id)
{
    ResolveConfigFloatById(self, id);

    if (other)
    {
        CFloat scale = *(const CFloat*)&other->m_field29C;
        self->m_value = self->m_value * scale;
        self->m_value = RoundCFloatHalfUp(self->m_value);
    }

    return self;
}
