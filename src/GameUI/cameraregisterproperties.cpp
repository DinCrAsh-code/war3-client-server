//============================================================================
//  0x6F301EE0 - CCameraWar3::Method_0x20(int context), vtable slot 8.
//
//  Registers all nine SCameraProp fields (and, after them, the pan-target
//  height, m_target's own Position sub-object twice, and m_localAngle) with
//  whatever TRefCnt-vtabled object each property's own SUnitTrackedRef
//  m_value currently has installed at its vtable slot 6 (+0x18/4) - the
//  "aHlistener"-tagged AddRef seen in refcnt.h's own file header, i.e. an
//  audio/UI listener object, not SUnitTrackedRef's own static type.  The
//  dispatch below reads that vtable pointer and calls through it directly
//  (the STrackTarget pattern gameui.h's own header already uses for a
//  receiver that is not `this`), because the concrete class behind it is
//  never reconstructed in either dump - only its slot-6 shape (id, context,
//  flag, three float pointers, count=2) is established, from the call
//  site's own pushes.
//
//  Each of the nine properties passes a *pair* of bound constants and one
//  *value* (either a config-file float, a literal camera default, or - for
//  m_roll/m_panZ/m_localAngle - the shared zero/max-range globals
//  g_CFloatZero/g_unk6FAAE624 the rest of this codebase already knows by
//  those names).  Three properties (m_fieldOfView, m_roll, m_rotation's
//  bound pair reused by m_localAngle) carry their bounds in *degrees* and
//  convert with the same `*pi/180` shape ResetToDefaults' own listener-angle
//  conversion uses (cameraresetdefaults.cpp).
//
//  m_targetDistance/m_rotation/m_angleOfAttack take their *value* argument
//  from one of three small per-preset/per-cursor-mode constant tables
//  (cameraconstantlookup.cpp) rather than from config or a literal - see
//  that file's own header.  m_rotation's own index folds `m_cursorMode`
//  into the read (`m_presetIndex + 6*m_cursorMode`), which is what makes its
//  table 18 entries long instead of 6.
//
//  The two `Position::RecordSlot8` calls near the end (context, id, 0, 2 -
//  positionrecordbuild.cpp) are a different mechanism entirely: `this` is
//  never read by anything in that call chain (RecordSlot8's own body only
//  forwards it), so the two receivers below (the reserved block at +0x468
//  and m_target itself) are placeholder addresses rather than a claim that
//  either is really a Position.
//
//  Returns the number of properties registered (the running id, plus one -
//  the shipped `lea eax,[esi+1]`).
//============================================================================
#include "gameui.h"
#include "positiontrack.h"

//  0x6F009E30 - configgates.cpp.  Re-declared, not redefined.
float __fastcall GetConfigFloatHW(const char* section, const char* key,
                                  int index);

//  cameraconstantlookup.cpp.
float CameraLookupTargetDistancePreset(unsigned int index);
float CameraLookupRotationByCursorMode(unsigned int index);
float CameraLookupAngleOfAttackPreset(unsigned int index);

extern const float g_camRegListenerDistanceBoundA;    // flt_6F97FF4C
extern const float g_camRegListenerDistanceBoundB;    // flt_6F97FF48
extern const float g_camRegListenerAngleBoundA;       // flt_6FAA13B8
extern const float g_camRegListenerAngleBoundB;       // flt_6F97FF50
extern const float g_camRegTargetDistanceBoundA;      // flt_6F97FF24
extern const float g_camRegTargetDistanceBoundB;      // flt_6F97FF20
extern const float g_camRegFarZBoundA;                // flt_6F97FF2C
extern const float g_camRegFarZBoundB;                // flt_6F97FF28
extern const float g_camRegNearZBoundA;               // flt_6F97FF3C
extern const float g_camRegNearZBoundB;               // flt_6F97FF38
extern const float g_camRegFieldOfViewBoundADeg;      // flt_6F97FF34
extern const float g_camRegFieldOfViewBoundBDeg;      // flt_6F97FF30
extern const float g_camRegRollBoundADeg;             // flt_6FADB300
extern const float g_camRegRollBoundBDeg;             // flt_6FADB24C
extern const float g_camRegRotationBoundA;            // flt_6FADB308
extern const float g_camRegRotationBoundB;            // flt_6FADB304
extern const float g_camRegAngleOfAttackBoundA;       // flt_6FADB310
extern const float g_camRegAngleOfAttackBoundB;       // flt_6FADB30C
extern const float g_camRegTargetZBound;              // flt_6F97FF44

extern const double g_pi;                  // dbl_6F876608
extern const double g_degreesPerHalfTurn;  // dbl_6F92EF60

extern const float g_cameraDefaultFarZ;         // flt_6F936460
extern const float g_cameraDefaultNearZ;        // flt_6F8E3A14
extern const float g_cameraDefaultFieldOfView;  // flt_6F9364DC

//  dword_6FAAE470 / dword_6FAAE624 - the shared zero and "unbounded" globals
//  this codebase already names elsewhere (CFloat.h, floatmini.h).  Read
//  through AsFloatPtr() below since this call goes to a listener-
//  registration slot, not the Position/FloatProp layer AsCFloat() targets.
extern const CFloat g_CFloatZero;      // dword_6FAAE470, CFloat.h
extern const unsigned int g_unk6FAAE624;

static const float* AsFloatPtr(const void* p) { return (const float*)p; }

//  The vtable slot every property registration goes through - slot 6
//  (+0x18/4) of whatever object SUnitTrackedRef::m_value's own vtable
//  pointer names.  See the file header.
static __forceinline void RegisterProp(SUnitTrackedRef* prop, int id,
                                       int context, int flag,
                                       const float* value, const float* p2,
                                       const float* p1)
{
    typedef void (__thiscall *Fn)(void*, int, int, int, const float*,
                                  const float*, const float*, int);
    ((Fn)(*(void***)prop)[0x18 / 4])(prop, id, context, flag, value, p2, p1,
                                     2);
}

static __forceinline float DegToRad(float degrees)
{
    return (float)((double)degrees * g_pi / g_degreesPerHalfTurn);
}

int CCameraWar3::Method_0x20(int context)
{
    //  sub_6F471BE0 - CAgent::Method_0x20(context), always 0 (agentdefaults.cpp).
    int id = CAgent::Method_0x20(context);

    float v;

    //  The two bound constants are copied into their own stack scratch
    //  floats before the value is computed, the same "lo/hi first, then v"
    //  shape m_fieldOfView/m_roll/m_targetZ/m_localAngle below already use -
    //  matching the dump's own `fld <global>/fstp [esp+N]` pair for each
    //  bound, which runs *before* the value's own registration call rather
    //  than taking `&g_camRegXBoundA/B`'s address directly.
    {
        float hi = g_camRegListenerDistanceBoundA;
        float lo = g_camRegListenerDistanceBoundB;
        v = GetConfigFloatHW("Listener", "ListenerDistance", m_presetIndex);
        RegisterProp(&m_listenerDistance.m_value, id, context, 0, &v, &lo, &hi);
    }
    ++id;

    {
        float hi = g_camRegListenerAngleBoundA;
        float lo = g_camRegListenerAngleBoundB;
        v = GetConfigFloatHW("Listener", "ListenerAOA", m_presetIndex);
        RegisterProp(&m_listenerAngle.m_value, id, context, 0, &v, &lo, &hi);
    }
    ++id;

    {
        float hi = g_camRegTargetDistanceBoundA;
        float lo = g_camRegTargetDistanceBoundB;
        v = CameraLookupTargetDistancePreset((unsigned int)m_presetIndex);
        RegisterProp(&m_targetDistance.m_value, id, context, 0, &v, &lo, &hi);
    }
    ++id;

    {
        float hi = g_camRegFarZBoundA;
        float lo = g_camRegFarZBoundB;
        v = g_cameraDefaultFarZ;
        RegisterProp(&m_farZ.m_value, id, context, 0, &v, &lo, &hi);
    }
    ++id;

    {
        float hi = g_camRegNearZBoundA;
        float lo = g_camRegNearZBoundB;
        v = g_cameraDefaultNearZ;
        RegisterProp(&m_nearZ.m_value, id, context, 0, &v, &lo, &hi);
    }
    ++id;

    {
        float lo = DegToRad(g_camRegFieldOfViewBoundADeg);
        float hi = DegToRad(g_camRegFieldOfViewBoundBDeg);
        v = g_cameraDefaultFieldOfView;
        RegisterProp(&m_fieldOfView.m_value, id, context, 0, &v, &hi, &lo);
    }
    ++id;

    {
        float lo = DegToRad(g_camRegRollBoundADeg);
        float hi = DegToRad(g_camRegRollBoundBDeg);
        RegisterProp(&m_roll.m_value, id, context, 0, AsFloatPtr(&g_CFloatZero), &hi,
                    &lo);
    }
    ++id;

    {
        float hi = g_camRegRotationBoundA;
        float lo = g_camRegRotationBoundB;
        v = (float)(m_presetIndex + 6 * m_cursorMode);
        v = DegToRad(CameraLookupRotationByCursorMode((unsigned int)v));
        RegisterProp(&m_rotation.m_value, id, context, 0, &v, &lo, &hi);
    }
    ++id;

    {
        float hi = g_camRegAngleOfAttackBoundA;
        float lo = g_camRegAngleOfAttackBoundB;
        v = DegToRad(CameraLookupAngleOfAttackPreset((unsigned int)m_presetIndex));
        RegisterProp(&m_angleOfAttack.m_value, id, context, 0, &v, &lo, &hi);
    }
    ++id;

    {
        float lo = g_camRegTargetZBound;
        float hi = -lo;
        RegisterProp(&m_targetZ.m_value, id, context, 0, AsFloatPtr(&g_CFloatZero),
                    &hi, &lo);
    }
    ++id;

    RegisterProp(&m_panZ.m_value, id, context, 0, AsFloatPtr(&g_CFloatZero),
                AsFloatPtr(&g_CFloatZero), AsFloatPtr(&g_unk6FAAE624));
    ++id;

    ((PositionTrack*)((char*)this + 0x468))->RecordSlot8(id, context, 0, 2);
    ++id;
    ((PositionTrack*)&m_target)->RecordSlot8(id, context, 0, 2);
    ++id;

    {
        float lo = g_camRegRotationBoundA;
        float hi = g_camRegRotationBoundB;
        //  flag 0x1775, not 0 - the one property whose registration carries
        //  a nonzero flag word.  Not established what it selects.
        typedef void (__thiscall *Fn)(void*, int, int, int, const float*,
                                      const float*, const float*, int);
        ((Fn)(*(void***)&m_localAngle.m_value)[0x18 / 4])(
            &m_localAngle.m_value, id, context, 0x1775, AsFloatPtr(&g_CFloatZero), &hi,
            &lo, 2);
    }
    ++id;

    return id;
}
