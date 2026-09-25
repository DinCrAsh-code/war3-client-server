//============================================================================
//  0x6F3085F0 - CCameraWar3::ResetToDefaults: every camera field back to
//  its configured or built-in default, the two controllers made if they do
//  not exist yet, the terrain sample kernel filled in, and a tail jump into
//  the binder.
//
//  This function is what *names* nine of the ten properties in gameui.h.
//  Two come from the configuration file by key ("Listener" /
//  "ListenerDistance" and "ListenerAOA"); the rest are literal constants,
//  and they are Warcraft III's own published camera defaults - 1650 target
//  distance, 5000 far Z, 100 near Z, 70 degrees of field of view, 90
//  degrees of rotation, 304 degrees of angle of attack, zero roll.  Which
//  setter each uses says as much: the three that go through
//  AssignWrappedValue are wrapped into one turn, i.e. they are angles.
//
//  Every value travels as the address of one *hardware float* local, reused
//  for all eleven publications - the shipped frame is a single `push ecx`
//  and every `fstp` lands in it - because the FloatProp setters take a
//  `const CFloat*` and CFloat is the same four bytes (gameui.h, AsCFloat).
//
//  The listener angle is the one computed value: the configuration file
//  holds it in degrees and the property wants radians, so it is multiplied
//  by pi and divided by 180 - two *double* constants, which is why those
//  two operands are `fmul ds:dbl_...` and not float loads.
//
//  The sample kernel at +0x570 is written twice: an identity 3x3 first
//  (diagonal ones, then the six zeroes) and then the real weights over the
//  top of it.  That is in the shipped instruction stream, not a reading of
//  it - the identity is a matrix's own default value and the assignment
//  that follows does not remove it.
//
//  Own translation unit: thirteen real calls and a tail jump.
//============================================================================
#include "gameui.h"

//  0x6F009E30 - configgates.cpp.  Re-declared, not redefined.
float __fastcall GetConfigFloatHW(const char* section, const char* key,
                                  int index);

//  0x6F4E7C40 / 0x6F4E9F80 - the two controller factories, both `retn 0`.
//  Thunked in gameui_thunks.cpp.
void* __fastcall MakeCameraController();
void* __fastcall MakeCameraEventSink();

//  The camera's built-in defaults, all of them unnamed constants in the
//  image; declared by address, no invented values.  flt_6F97FF5C and
//  flt_6F97FF64 hold the same number and are two distinct constants in the
//  shipped .rdata, so they stay two here.
extern const float g_cameraDefaultAngleOfAttack;  // flt_6F936610
extern const float g_cameraDefaultTargetDistance; // flt_6F93645C
extern const float g_cameraDefaultFieldOfView;    // flt_6F9364DC
extern const float g_cameraDefaultFarZ;           // flt_6F936460
extern const float g_cameraDefaultNearZ;          // flt_6F8E3A14
extern const float g_cameraDefaultRotation;       // flt_6F93660C
extern const float g_groundSampleCornerWeight;    // flt_6F97FF5C
extern const float g_groundSampleEdgeWeight;      // flt_6F97FF60
extern const float g_groundSampleCentreWeight;    // flt_6F97FF64
//  dbl_6F876608 / dbl_6F92EF60 - pi and one hundred and eighty, as doubles.
extern const double g_pi;
extern const double g_degreesPerHalfTurn;

void CCameraWar3::ResetToDefaults()
{
    float value;

    value = GetConfigFloatHW("Listener", "ListenerDistance", 0);
    m_listenerDistance.m_value.SetValue(AsCFloat(value));

    value = (float)(GetConfigFloatHW("Listener", "ListenerAOA", 0) *
                    g_pi / g_degreesPerHalfTurn);
    m_listenerAngle.m_value.AssignWrappedValue(AsCFloat(value));

    value = g_cameraDefaultAngleOfAttack;
    m_angleOfAttack.m_value.AssignWrappedValue(AsCFloat(value));

    value = g_cameraDefaultTargetDistance;
    m_targetDistance.m_value.SetValue(AsCFloat(value));

    value = g_cameraDefaultFieldOfView;
    m_fieldOfView.m_value.AssignWrappedValue(AsCFloat(value));

    value = g_cameraDefaultFarZ;
    m_farZ.m_value.SetValue(AsCFloat(value));

    m_targetZ.m_value.SetValue(&g_CFloatZero);
    m_panZ.m_value.SetValue(&g_CFloatZero);

    value = g_cameraDefaultNearZ;
    m_nearZ.m_value.SetValue(AsCFloat(value));

    value = g_cameraDefaultRotation;
    m_rotation.m_value.AssignWrappedValue(AsCFloat(value));

    value = 0.0f;
    m_roll.m_value.AssignWrappedValue(AsCFloat(value));

    m_localAngle.m_value.AssignWrappedValue(&g_CFloatZero);

    if (m_pDefaultController == 0)
    {
        m_pDefaultController = MakeCameraController();
        m_pController = m_pDefaultController;
    }
    if (m_pAltController == 0)
        m_pAltController = MakeCameraEventSink();

    //  The identity first, then the kernel over it.  See the header note.
    m_sampleWeights[8] = 1.0f;
    m_sampleWeights[4] = 1.0f;
    m_sampleWeights[0] = 1.0f;
    m_sampleWeights[7] = 0.0f;
    m_sampleWeights[6] = 0.0f;
    m_sampleWeights[5] = 0.0f;
    m_sampleWeights[3] = 0.0f;
    m_sampleWeights[2] = 0.0f;
    m_sampleWeights[1] = 0.0f;

    m_sampleWeights[7] = g_groundSampleEdgeWeight;
    m_sampleWeights[5] = g_groundSampleEdgeWeight;
    m_sampleWeights[1] = g_groundSampleEdgeWeight;
    m_sampleWeights[3] = g_groundSampleEdgeWeight;
    m_sampleWeights[8] = g_groundSampleCornerWeight;
    m_sampleWeights[2] = g_groundSampleCornerWeight;
    m_sampleWeights[6] = g_groundSampleCornerWeight;
    m_sampleWeights[0] = g_groundSampleCornerWeight;
    m_sampleWeights[4] = g_groundSampleCentreWeight;

    m_controllerMode = 0;

    BindControllerProps();
}
