//============================================================================
//  0x6F3027B0 - CCameraWar3::GetProperty: one camera property's current
//  value, selected by the JASS `camerafield` id.
//
//  This function is what *proves* the field numbering the whole family uses,
//  because the JASS native `GetCameraField` (0x6F3B4CD0) hands its own
//  `camerafield` argument straight through: 0 target distance, 1 far Z, 2
//  angle of attack, 3 field of view, 4 roll, 5 rotation, 6 Z offset - which
//  is Warcraft III's published CAMERA_FIELD_* order - plus three the JASS
//  API does not expose, 7 near Z and the two listener fields at 8 and 9.
//  Every one of the ten lands on the property gameui.h already named from
//  the default CCameraWar3::ResetToDefaults writes into it, and the two
//  readings agree field for field.
//
//  **Which getter each case uses is the second half of that evidence.**  The
//  four angles plus the listener angle read through GetWrappedValue, which
//  wraps into one turn (floatmodstep.cpp); the five that are lengths read
//  the value straight.  That is the same split ResetToDefaults makes on the
//  writing side.
//
//  The case blocks are written in the shipped block order - 8, 9, 0, 1, 6,
//  7, 2, 3, 4, 5 - which IDA's own `jumptable ... case <n>` comments give
//  directly and which is not the numeric order.  Reordering them costs the
//  layout of the jump table's targets and nothing else.
//
//  **The ten scratch CFloats are declared at function scope, one per case,
//  and not inside each case block.**  That is worth the whole 0x24-byte
//  frame: locals declared inside the blocks are packed into one slot by
//  this compiler - the blocks' lifetimes are disjoint - and the function
//  then needs no frame at all, which loses the `sub esp, 24h` and one
//  `add esp, 24h` on each of the eleven exits.  Declared here they get ten
//  separate slots, which is what the shipped frame carries.
//
//  Own translation unit: ten real calls into the FloatProp layer.
//============================================================================
#include "gameui.h"

//  The `camerafield` ids, in the numbering this switch establishes.
enum
{
    kCameraFieldTargetDistance   = 0,
    kCameraFieldFarZ             = 1,
    kCameraFieldAngleOfAttack    = 2,
    kCameraFieldFieldOfView      = 3,
    kCameraFieldRoll             = 4,
    kCameraFieldRotation         = 5,
    kCameraFieldZOffset          = 6,
    kCameraFieldNearZ            = 7,
    kCameraFieldListenerDistance = 8,
    kCameraFieldListenerAngle    = 9
};

//  The properties are hardware floats stored in the FloatProp layer's own
//  encoding, which is the same four bytes - see gameui.h's AsCFloat().
static float AsFloat(const CFloat& v) { return *(const float*)&v.m_bits; }

float CCameraWar3::GetProperty(int field)
{
    CFloat listenerDistance(kCFloatNoInit);
    CFloat listenerAngle(kCFloatNoInit);
    CFloat targetDistance(kCFloatNoInit);
    CFloat farZ(kCFloatNoInit);
    CFloat zOffset(kCFloatNoInit);
    CFloat nearZ(kCFloatNoInit);
    CFloat angleOfAttack(kCFloatNoInit);
    CFloat fieldOfView(kCFloatNoInit);
    CFloat roll(kCFloatNoInit);
    CFloat rotation(kCFloatNoInit);

    switch (field)
    {
    case kCameraFieldListenerDistance:
        m_listenerDistance.m_value.GetValue(&listenerDistance);
        return AsFloat(listenerDistance);
    case kCameraFieldListenerAngle:
        listenerAngle = m_listenerAngle.m_value.GetWrappedValue();
        return AsFloat(listenerAngle);
    case kCameraFieldTargetDistance:
        m_targetDistance.m_value.GetValue(&targetDistance);
        return AsFloat(targetDistance);
    case kCameraFieldFarZ:
        m_farZ.m_value.GetValue(&farZ);
        return AsFloat(farZ);
    case kCameraFieldZOffset:
        m_targetZ.m_value.GetValue(&zOffset);
        return AsFloat(zOffset);
    case kCameraFieldNearZ:
        m_nearZ.m_value.GetValue(&nearZ);
        return AsFloat(nearZ);
    case kCameraFieldAngleOfAttack:
        angleOfAttack = m_angleOfAttack.m_value.GetWrappedValue();
        return AsFloat(angleOfAttack);
    case kCameraFieldFieldOfView:
        fieldOfView = m_fieldOfView.m_value.GetWrappedValue();
        return AsFloat(fieldOfView);
    case kCameraFieldRoll:
        roll = m_roll.m_value.GetWrappedValue();
        return AsFloat(roll);
    case kCameraFieldRotation:
        rotation = m_rotation.m_value.GetWrappedValue();
        return AsFloat(rotation);
    }

    return 0.0f;
}
