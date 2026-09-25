//============================================================================
//  CDestructable's own SetLife/AddLife (vtable slots 73/74), CItem's own
//  SetLife/AddLife (item_valueset.cpp) mirrored almost exactly - the same
//  "publish through FloatMini's own vtable slot 0, then ask
//  FloatValueIsFarFrom whether the write just crossed the floor" shape -
//  with three differences that are real behaviour, not transcription
//  noise:
//
//   * neither clamps the incoming value at zero from below the way
//     CItem's own setters do - the shipped code passes the caller's
//     pointer straight through;
//   * both call CWidget::NotifyLifeChanged (0x6F2AB460,
//     unreconstructed_thunks.cpp) unconditionally at the end, which
//     CItem's own two never do - the same notify CUnit::SetLife
//     (unit_setvalue.cpp) already calls for a different reason;
//   * where CItem calls its own OnValueFloorReached, this class calls
//     0x6F26AE00, a death/regrowth trigger this session's own vtable-slot
//     pass did not chase down (see this file's own note below) - reached
//     through a naked redirect rather than reconstructed, the same
//     "out of scope for this pass" call destructable_offset.cpp already
//     documents for two of slot 72's own callees.
//
//  Own translation unit: both make real out-of-line calls to
//  FloatValueIsFarFrom and to the floor-reached trigger.
//============================================================================
#include "destructable.h"

//  0x6F022270 - floatmini_compare.cpp.
bool __fastcall FloatValueIsFarFrom(const FloatMini* self, const CFloat* target);

//  flt_6FAAE4C4 - g_itemValueFloor (item_valueset.cpp's own DATA row);
//  reused here rather than declared a second time under a new name, since
//  it is the same constant at the same address.
extern const float g_itemValueFloor;

//  0x6F26AE00 - a death/regrowth trigger this pass did not chase down (see
//  this file's own header comment).  Takes `this` alone and cleans nothing
//  (`retn`).
__declspec(naked) void __fastcall CDestructableOnLifeFloorReached(void*)
{
    __asm { mov eax, 06F26AE00h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F26B100 - slot 73 (+0x124), SetLife.
//----------------------------------------------------------------------------
void CDestructable::SetLife(const CFloat* value)
{
    CFloat oldValue = m_floatB.m_value;

    ((FloatMiniB::NotifyFn)(*(void***)&m_floatB)[0])(
        &m_floatB, (CFloat*)value, 1);

    if (!FloatValueIsFarFrom(&m_floatB, (const CFloat*)&g_itemValueFloor) &&
        !(*(const float*)&oldValue <= g_itemValueFloor))
        CDestructableOnLifeFloorReached(this);

    CWidget::NotifyLifeChanged();
}

//----------------------------------------------------------------------------
//  0x6F26B160 - slot 74 (+0x128), AddLife.
//----------------------------------------------------------------------------
CFloat CDestructable::AddLife(const CFloat* delta)
{
    CFloat oldValue = m_floatB.m_value;

    FloatMiniB* range = &m_floatB;
    void** floatVtable = *(void***)range;

    typedef void (__thiscall *NotifyRefFn)(void* self, const CFloat& value, int flag);
    ((NotifyRefFn)floatVtable[0])(range, range->m_value + *delta, 1);

    if (!FloatValueIsFarFrom(&m_floatB, (const CFloat*)&g_itemValueFloor) &&
        !(*(const float*)&oldValue <= g_itemValueFloor))
        CDestructableOnLifeFloorReached(this);

    CWidget::NotifyLifeChanged();

    return CFloat(m_floatB.m_value) - CFloat(oldValue);
}
