//============================================================================
//  0x6F3046D0 - CCameraWar3::SetProperty: set one camera property's target
//  value and blend span, by the same `camerafield` id
//  GameUI/cameragetproperty.cpp's own GetProperty already established (0
//  target distance, 1 far Z, 2 angle of attack, 3 field of view, 4 roll, 5
//  rotation, 6 Z offset, 7 near Z, 8/9 the two listener fields).  Every
//  case's own `lea ecx, [esi+OFFSET]` lines up exactly with gameui.h's
//  already-committed SCameraProp offsets (case 8 at +0xCC through case 6 at
//  +0x1B4), which is independent confirmation of both tables at once.
//
//  Stops whatever is currently driving the slot first (StopSlot, already
//  reconstructed - camerastopslot.cpp), then makes a new FloatModifier that
//  walks the property to `value` over `span` and adopts it into the
//  property's own +0x10 counted reference - the exact
//  `SUnitTrackedRef::MakeFloatModifier` / `SCountedRef::Assign` pair
//  GameUI/camerapan.cpp's own PanTo already uses for `m_panZ`.
//
//  **The shared tail past the switch (`test eax,eax` / conditional
//  `add [eax+4],-1` / vtbl-slot-0 call) is not hand-written cleanup - it is
//  the compiler's one inlined copy of `SFloatModifierRef::~SFloatModifierRef`
//  (Unit/floatpropmodifier.h)**, reached once no matter which case ran,
//  because all ten cases' own `SFloatModifierRef ref = ...;` temporaries
//  share one stack slot (their lifetimes never overlap - the same reason
//  GetProperty's own ten `CFloat` scratch locals are declared at function
//  scope rather than per-case).  `ref` is declared once, at function scope,
//  for exactly that reason; an out-of-range field skips every case and
//  still runs `ref`'s destructor on its default `{0}` state, which is a
//  no-op.
//
//  A genuine `__except_handler4`-shaped SEH frame (cookie-XORed,
//  EBP-omitted, scope-table trampoline appended after `retn`) wraps the
//  whole body - the same unreproducible shape
//  docs/msvc-vc8-idioms.md documents ("An `__except_handler4`-shaped frame
//  this toolchain cannot reproduce"), needed here because the scope table
//  has to know which of the ten (disjoint but still nominally separate)
//  `ref` temporaries to unwind through if `MakeFloatModifier` or `Assign`
//  raises - so this scores well under 1.000 by construction; the call
//  sequence and field offsets past the frame match the dump case for case.
//
//  Own translation unit: real calls into StopSlot, MakeFloatModifier and
//  SCountedRef::Assign, all already reconstructed elsewhere.
//============================================================================
#include "gameui.h"
#include "floatpropmodifier.h"

//  The `camerafield` ids - GetProperty's own enum (cameragetproperty.cpp),
//  not redeclared there to avoid a duplicate-definition link error; this
//  file's cases are written out with the numeric id in a comment instead.

void CCameraWar3::SetProperty(int field, float span, float value)
{
    StopSlot(field);

    //  Each case's `ref` is declared with an initializer, inside its own
    //  block, exactly like camerapan.cpp's own PanTo call to the same
    //  MakeFloatModifier/Assign pair - copy-initialization here is what
    //  lets the compiler construct the return value straight into `ref`
    //  (no extra temporary, no early destructor call), where a `ref =
    //  MakeFloatModifier(...)` *assignment* to an already-declared local
    //  cannot: that spelling was tried first and measured far worse (a
    //  0.106 gen-268-vs-243 blowout) because it runs `ref`'s destructor on
    //  a same-valued temporary immediately after the call, before
    //  Assign ever sees it. `ref` destructs (drops this function's own
    //  reference, once Assign has already adopted a second one) at the
    //  end of its own case block either way, matching the shipped code's
    //  own per-case "make it, adopt it, drop it" shape.
    switch (field)
    {
    case 8:  //  kCameraFieldListenerDistance, +0xCC
        {
            SFloatModifierRef ref =
                m_listenerDistance.m_value.MakeFloatModifier(AsCFloat(value), AsCFloat(span));
            ((SCountedRef*)&m_listenerDistance.m_pModifier)->Assign((SCountedRef*)&ref);
        }
        break;
    case 9:  //  kCameraFieldListenerAngle, +0xE4
        {
            SFloatModifierRef ref =
                m_listenerAngle.m_value.MakeFloatModifier(AsCFloat(value), AsCFloat(span));
            ((SCountedRef*)&m_listenerAngle.m_pModifier)->Assign((SCountedRef*)&ref);
        }
        break;
    case 0:  //  kCameraFieldTargetDistance, +0xFC
        {
            SFloatModifierRef ref =
                m_targetDistance.m_value.MakeFloatModifier(AsCFloat(value), AsCFloat(span));
            ((SCountedRef*)&m_targetDistance.m_pModifier)->Assign((SCountedRef*)&ref);
        }
        break;
    case 1:  //  kCameraFieldFarZ, +0x114
        {
            SFloatModifierRef ref =
                m_farZ.m_value.MakeFloatModifier(AsCFloat(value), AsCFloat(span));
            ((SCountedRef*)&m_farZ.m_pModifier)->Assign((SCountedRef*)&ref);
        }
        break;
    case 2:  //  kCameraFieldAngleOfAttack, +0x174
        {
            SFloatModifierRef ref =
                m_angleOfAttack.m_value.MakeFloatModifier(AsCFloat(value), AsCFloat(span));
            ((SCountedRef*)&m_angleOfAttack.m_pModifier)->Assign((SCountedRef*)&ref);
        }
        break;
    case 3:  //  kCameraFieldFieldOfView, +0x144
        {
            SFloatModifierRef ref =
                m_fieldOfView.m_value.MakeFloatModifier(AsCFloat(value), AsCFloat(span));
            ((SCountedRef*)&m_fieldOfView.m_pModifier)->Assign((SCountedRef*)&ref);
        }
        break;
    case 4:  //  kCameraFieldRoll, +0x18C
        {
            SFloatModifierRef ref =
                m_roll.m_value.MakeFloatModifier(AsCFloat(value), AsCFloat(span));
            ((SCountedRef*)&m_roll.m_pModifier)->Assign((SCountedRef*)&ref);
        }
        break;
    case 5:  //  kCameraFieldRotation, +0x15C
        {
            SFloatModifierRef ref =
                m_rotation.m_value.MakeFloatModifier(AsCFloat(value), AsCFloat(span));
            ((SCountedRef*)&m_rotation.m_pModifier)->Assign((SCountedRef*)&ref);
        }
        break;
    case 7:  //  kCameraFieldNearZ, +0x12C
        {
            SFloatModifierRef ref =
                m_nearZ.m_value.MakeFloatModifier(AsCFloat(value), AsCFloat(span));
            ((SCountedRef*)&m_nearZ.m_pModifier)->Assign((SCountedRef*)&ref);
        }
        break;
    case 6:  //  kCameraFieldZOffset, +0x1B4 (m_targetZ)
        {
            SFloatModifierRef ref =
                m_targetZ.m_value.MakeFloatModifier(AsCFloat(value), AsCFloat(span));
            ((SCountedRef*)&m_targetZ.m_pModifier)->Assign((SCountedRef*)&ref);
        }
        break;
    }
}
