//============================================================================
//  0x6F0F2E60 - CMissileFire's own vtable slot 47, Method_0xBC.  Overrides
//  CMissile's own real Method_0xBC (missile.h/missile_core.cpp) rather
//  than the empty nullsub every level below CMissile left untouched.
//
//  Best-effort, out-of-depth-budget reconstruction: no asm/*.md dump
//  covers this address, and its own deepest callee (0x6F2C31D0) is itself
//  a large, unexplored leaf several levels past this session's depth
//  budget - the same situation CMissile.md's own Method_0xBC documents
//  for its own deep reconfirmation closure one level down.
//
//  What the frame actually does, read address-by-address off raw_asm (an
//  ordinary SEH frame the compiler generates automatically for the
//  RAII SCheckedUnitSlot local below - the same shape missile_arrival.cpp's
//  own OnArrival gets from the same local type, /EHs-c-):
//
//    - resolve the +0xA0 ref (SHandleRefAt0xA0Owner::GetField0x54IfSet,
//      0x6F2BBA80 - same resolver missile_core.cpp/missile_arrival.cpp
//      already use) into `target`;
//    - ask `this` for its own tracked vector (GetTrackedVector, this
//      class's own inherited vtable slot 40/+0xA0 - CBullet's real
//      resolve through m_projectilePosition, bullet_position.cpp) into a
//      3-float local, keeping only the first two components;
//    - resolve the +0x2C target-handle ref the same way
//      CBulletBase::Deactivate/Method_0xA8/OnDispose already do
//      ((m_targetTypeTag & (int)m_targetHandle) != -1, then
//      QueryHandleField0x54) into `resolvedTarget`;
//    - hold a counted reference to it for the duration (SCheckedUnitSlot,
//      Agent/agenttypedslots.h - same idiom missile_arrival.cpp's own
//      OnArrival uses, inlined release at the scope exit either way);
//    - forward `target`, the tracked position, this class's own
//      m_fieldAC/m_fieldB0, a literal callback address (0x6F0DC580 - not
//      itself reconstructed; only its address is ever taken here, never
//      called from this function) and several literal flags/zeros into
//      the deep leaf below;
//    - tail-call CMissile::Method_0xBC() directly (non-virtually,
//      0x6F2BECB0, confirmed against the call target) as the very last
//      step - this level layers its own work *before* the inherited body
//      rather than replacing it, the same "layer one extra step on top of
//      the inherited body" shape this whole family uses.
//============================================================================
#include "missilefire.h"
#include "itemhandleresolve.h"
#include "agenttypedslots.h"

//  0x6F2BBA80 - {handle,typeTag} pair at this+0xA0/+0xA4 (misc_handle_
//  resolvers.cpp).  Re-declared here rather than pulled in through a
//  header because that file doesn't have one - same convention every
//  other user of this exact struct already uses.
struct SHandleRefAt0xA0Owner { void* __thiscall GetField0x54IfSet(); };

//----------------------------------------------------------------------------
//  0x6F2C31D0 - the deep "no-hit report/spawn" leaf this override forwards
//  to: two pointer arguments in ecx/edx (the tracked position's own x/y,
//  by address) plus twelve stack arguments (a 2-float buffer built from
//  this object's own m_floatF/m_floatG, the resolved +0x2C target, this
//  class's own m_fieldAC/m_fieldB0, the +0xA0 ref, a callback address and
//  a handful of literal flags/zeros) - several levels past this session's
//  own depth budget.  Naked redirect to the real, unhooked shipped body,
//  same convention CMissile.md's own deep impact-reconfirmation closures
//  (SFieldA0ImpactRecheck::Recheck, SFieldA0ReconfirmOwner::Reconfirm)
//  already use.
//
//  Signature read directly off this call site's own register/stack setup
//  (no dump exists for 0x6F2C31D0 itself to confirm it against) - a
//  best-effort reading, not a dump-confirmed one.
//----------------------------------------------------------------------------
void __fastcall MissileFireNoHitReport(
    const float* trackedX, const float* trackedY,
    const float (*posGF)[2],
    unsigned int one1, unsigned int flags,
    unsigned int zero1, unsigned int zero2,
    int fieldB0, void* resolvedTarget, int fieldAC,
    void* callback, void* handleA0Target,
    unsigned int one2, unsigned int zero3);

__declspec(naked) void __fastcall MissileFireNoHitReport(
    const float*, const float*, const float (*)[2],
    unsigned int, unsigned int, unsigned int, unsigned int,
    int, void*, int, void*, void*, unsigned int, unsigned int)
{
    __asm { mov eax, 0x6F2C31D0 }
    __asm { jmp eax }
}

void CMissileFire::Method_0xBC()
{
    void* target = ((SHandleRefAt0xA0Owner*)this)->GetField0x54IfSet();

    float trackedVec[3];
    this->GetTrackedVector(trackedVec);

    void* resolvedTarget = 0;
    if ((m_targetTypeTag & (int)m_targetHandle) != -1)
        resolvedTarget = QueryHandleField0x54((SOptionalHandleRef*)&m_targetHandle);

    SCheckedUnitSlot ref((CAgent*)resolvedTarget);

    //  {m_floatG.m_value, m_floatF.m_value}, in that order - confirmed by
    //  the raw disassembly's own local layout (var_20 = m_floatG, var_1C =
    //  m_floatF, four bytes apart, address of var_20 passed as the buffer).
    float posGF[2];
    posGF[0] = *(const float*)&m_floatG.m_value;
    posGF[1] = *(const float*)&m_floatF.m_value;

    MissileFireNoHitReport(
        &trackedVec[0], &trackedVec[1], &posGF,
        1, 0x4000000u, 0, 0,
        m_fieldB0, ref.m_value, m_fieldAC,
        (void*)0x6F0DC580, target,
        1, 0);

    this->CMissile::Method_0xBC();
}
