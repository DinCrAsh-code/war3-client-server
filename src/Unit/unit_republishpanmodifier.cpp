//============================================================================
//  0x6F27A4E0 - reached from CUnit's own vtable slot 3 (0x6F2A7E60).
//  `retn 4`: one `float` parameter, by value.
//
//  Computes a fixed span (`g_unk6FAAE50C / g_unk6FAAE50C`, already a real
//  `const CFloat`, ability_typemigrate.cpp - two source constants that
//  happen to fold to the same address, giving CFloat(1.0f) for any
//  non-zero value of that constant) and makes a FloatModifier that walks
//  `m_refFC` (unit.h's own SUnitTrackedRef at +0xFC) to the incoming value
//  over that span, exactly the way `SUnitTrackedRef::MakeFloatModifier`
//  (0x6F4774E0, floatpropmodifiermake.cpp) is already reached from
//  `CCameraWar3::PanTo` (camerapan.cpp) for its own height slot - same
//  call, same `AsCFloat`-style by-value-then-take-address argument
//  passing, `m_pRef110` playing `m_panZ.m_pModifier`'s own role.
//
//  The counted reference this hands back (`SFloatModifierRef`,
//  floatpropmodifier.h) is manually addref'd *before* it overwrites
//  `m_pRef110` - the same `if (move.m_ptr) move.m_ptr->m_refcount++`
//  camerapan.cpp's own `m_pPanMovement` assignment does - so the
//  temporary's own destructor releasing its claim when it goes out of
//  scope at the end of this function does not undo the transfer.  The old
//  `m_pRef110` is released inline (unit_dtorbody.cpp's `ReleaseRef` is
//  `static` to that file, so this repeats its two lines rather than
//  reaching for it).
//
//  Own translation unit: no callee here is otherwise reconstructed inside
//  this same module.
//============================================================================
#include "unit.h"
#include "floatpropmodifier.h"
#include "refcnt.h"

extern const CFloat g_unk6FAAE50C;   // ability_typemigrate.cpp

void CUnit::RepublishPanFloatModifier(float value)
{
    CFloat span = g_unk6FAAE50C / g_unk6FAAE50C;

    SFloatModifierRef ref =
        m_refFC.MakeFloatModifier((const CFloat*)&value, &span);

    if (ref.m_ptr)
        ref.m_ptr->m_refcount++;

    TRefCnt* held = (TRefCnt*)m_pRef110;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
    m_pRef110 = ref.m_ptr;
}
