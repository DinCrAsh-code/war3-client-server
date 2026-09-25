//============================================================================
//  0x6F305A60 - the per-property leaf CCameraWar3::ApplyPresetBlend
//  (0x6F306B80, still a thunk - gameui_thunks.cpp) calls ten times, once
//  per camera property, to blend one property toward `value` over
//  `span` seconds.  `retn 10h`.
//
//  Two ten-way switches on the same `camerafield` id GetProperty/SetProperty
//  already established (cameragetproperty.cpp/camerasetproperty.cpp), whose
//  case offsets (+0xCC..+0x1B4) independently confirm that same table a
//  third time:
//
//   - **`|span - 0| <= g_cameraTolerance`** (the up-front hardware-float
//     `fabs`/`fcomp` guard): treat `span` as "instant" - stop whatever is
//     driving the slot (StopSlot, already reconstructed) and write `value`
//     straight into the property, through `SetValue` for the five plain
//     (non-angle) fields and through `AssignWrappedValue` - with `value`
//     first converted `DegreesToRadiansHW` when `flag` (arg_C) is set -
//     for the five angle fields.  Each case is its own early return
//     (its own epilogue and `retn 10h`), matching GetProperty's own
//     "cases don't fall through" shape.
//   - **Otherwise**: read the property's *current* value back
//     (`GetValue`/`GetWrappedValue`, wrapping both the current and the
//     - possibly degrees-converted - target through `WrapToRange` for the
//     five angle fields so the delta takes the short way around), form
//     `delta = target - current` with a plain hardware `float` subtract
//     (not CFloat's own software one - this blend math is local-UI-only,
//     not part of the deterministic simulation), and call `SetProperty`
//     with the *original* `span` and `delta / span` where the shipped
//     stream computes it (`fdivp`/`fxch`) - **not** `(field, value, span)`;
//     see camerasetproperty.cpp's own header for the confirmed
//     `(field, span, value)` stack order this call site is the direct
//     evidence for.
//
//  All ten "current value" reads share one tail (0x6F305EE4/0x6F305EEB in
//  the dump) reached by `jmp`, which this reconstruction folds into one
//  shared static helper (`BlendToProperty`) for the same reason - real
//  code, not hand duplication, is what a "call this ten times" leaf is
//  for.
//
//  No SEH frame here (unlike its own caller's neighbours) - a plain
//  `sub esp` frame throughout - so the shortfall this scores at is
//  whatever this session's own switch/tail-merge shape did not reproduce
//  bit for bit, not an unreproducible compiler feature.
//============================================================================
#include "gameui.h"

//  The compiler's own intrinsic - see camerapan.cpp's own header for why
//  `fabs` needs C linkage and `#pragma intrinsic` to make this build's
//  /O2 listing show the shipped code's own bare x87 `fabs` instruction
//  rather than a call.
extern "C" double __cdecl fabs(double value);
#pragma intrinsic(fabs)

//  0x6F302710 - already a real reconstruction, GameUI/camerasmoothing.cpp
//  (`float __fastcall WrapToRange(float x)`) - declared here rather than
//  included from that header-less TU, the same "declare the signature
//  that already exists elsewhere" pattern this file's own MakeFloatModifier/
//  SetValue/etc. declarations all follow.  A first pass here redeclared it
//  as a naked `__stdcall` stub under a mistaken belief that it had no real
//  body yet; that duplicated the C++ name across two different compiled
//  definitions and cost the *already-finished* WrapToRange 2 of its own
//  21/23 matched instructions in every dump that reaches it, until this
//  fix removed the duplicate.
float __fastcall WrapToRange(float x);

//  0x6F301B70 - GameUI/cameramiscleaves.cpp.
float __fastcall DegreesToRadiansHW(float degrees);

//  flt_6F92E7A4 - GameUI/camerapan.cpp's own g_cameraTolerance (0.001f).
extern const float g_cameraTolerance;

//  Apply one property's blend, given its already-read `value`/`span`/
//  `flag` and the two field accessors GetProperty/SetProperty already
//  established for this same offset.  Not every property is an angle -
//  `wrapped` selects which of the two shipped shapes this call site uses.
static void BlendToProperty(CCameraWar3* self, int field, float value,
                            float span, int flag, SCameraProp* prop,
                            int wrapped)
{
    float current;
    if (wrapped)
    {
        CFloat wrappedCurrent = prop->m_value.GetWrappedValue();
        current = WrapToRange(*(const float*)&wrappedCurrent.m_bits);

        float target = flag ? DegreesToRadiansHW(value) : value;
        target = WrapToRange(target);
        value = target;
    }
    else
    {
        CFloat currentValue(kCFloatNoInit);
        prop->m_value.GetValue(&currentValue);
        current = *(const float*)&currentValue.m_bits;
    }

    float delta = value - current;
    self->SetProperty(field, span, delta / span);
}

void CCameraWar3::ApplyPresetBlendProperty(int field, float value, float span,
                                           int flag)
{
    float absSpan = (float)fabs(span);
    if (absSpan <= g_cameraTolerance)
    {
        //  "instant" path: |span| <= tolerance.
        StopSlot(field);

        switch (field)
        {
        case 8:   m_listenerDistance.m_value.SetValue(AsCFloat(value)); return;
        case 9:
            {
                float v = flag ? DegreesToRadiansHW(value) : value;
                m_listenerAngle.m_value.AssignWrappedValue(AsCFloat(v));
            }
            return;
        case 0:   m_targetDistance.m_value.SetValue(AsCFloat(value)); return;
        case 1:   m_farZ.m_value.SetValue(AsCFloat(value)); return;
        case 7:   m_nearZ.m_value.SetValue(AsCFloat(value)); return;
        case 6:   m_targetZ.m_value.SetValue(AsCFloat(value)); return;
        case 2:
            {
                float v = flag ? DegreesToRadiansHW(value) : value;
                m_angleOfAttack.m_value.AssignWrappedValue(AsCFloat(v));
            }
            return;
        case 3:
            {
                float v = flag ? DegreesToRadiansHW(value) : value;
                m_fieldOfView.m_value.AssignWrappedValue(AsCFloat(v));
            }
            return;
        case 4:
            {
                float v = flag ? DegreesToRadiansHW(value) : value;
                m_roll.m_value.AssignWrappedValue(AsCFloat(v));
            }
            return;
        case 5:
            {
                float v = flag ? DegreesToRadiansHW(value) : value;
                m_rotation.m_value.AssignWrappedValue(AsCFloat(v));
            }
            return;
        }
        return;
    }

    //  Blend path: read the current value back, form the delta, and hand
    //  it to SetProperty as a per-second rate over the original span.
    switch (field)
    {
    case 8:  BlendToProperty(this, field, value, span, flag, &m_listenerDistance, 0); break;
    case 9:  BlendToProperty(this, field, value, span, flag, &m_listenerAngle, 1); break;
    case 0:  BlendToProperty(this, field, value, span, flag, &m_targetDistance, 0); break;
    case 1:  BlendToProperty(this, field, value, span, flag, &m_farZ, 0); break;
    case 7:  BlendToProperty(this, field, value, span, flag, &m_nearZ, 0); break;
    case 6:  BlendToProperty(this, field, value, span, flag, &m_targetZ, 0); break;
    case 2:  BlendToProperty(this, field, value, span, flag, &m_angleOfAttack, 1); break;
    case 3:  BlendToProperty(this, field, value, span, flag, &m_fieldOfView, 1); break;
    case 4:  BlendToProperty(this, field, value, span, flag, &m_roll, 1); break;
    case 5:  BlendToProperty(this, field, value, span, flag, &m_rotation, 1); break;
    default:
        //  Out of range: the shipped stream still runs the shared tail
        //  with `delta` read off an `fldz`'d scratch (i.e. 0), which -
        //  divided by `span` - calls SetProperty with a zero rate.  Not
        //  reachable from any of this repo's own callers (every one
        //  clamps its own field/index first), but reproduced for parity.
        SetProperty(field, span, 0.0f / span);
        break;
    }
}
