//============================================================================
//  0x6F3DA5C0 - CCameraSetup::Method_0x20(int context), vtable slot 8.  The
//  same "register every property with whatever listener object its own
//  SUnitTrackedRef.m_vtable names" mechanism as CCameraWar3::Method_0x20
//  (gameui.h/cameraregisterproperties.cpp), but simpler: every one of the
//  seventeen properties here registers the *same* bound pair
//  (g_CFloatZero/g_unk6FAAE624 - dword_6FAAE470/dword_6FAAE624, the shared
//  globals this codebase already knows by those names), and eleven of the
//  seventeen register a literal zero as their value too.  Only six carry a
//  real default: three reuse constants CCameraWar3's own registration
//  already names (g_cameraDefaultTargetDistance, g_cameraDefaultFarZ,
//  g_cameraDefaultNearZ, g_camCursorModeRotation0, g_camPresetAngleOfAttack0
//  - the shipped `.rdata` addresses are identical), and one is genuinely
//  new to this class (g_camSetupProp3Default, property 3 / +0x54).
//
//  Property order in the shipped stream is not declaration order: +0x84
//  (property 6) is registered ahead of +0x64/+0x74 (properties 4/5).  The
//  calls below are in that literal order, not field order.
//============================================================================
#include "camerasetup.h"
#include "CFloat.h"

extern const float g_cameraDefaultTargetDistance;   // flt_6F93645C
extern const float g_cameraDefaultFarZ;              // flt_6F936460
extern const float g_cameraDefaultNearZ;             // flt_6F8E3A14
extern const float g_camSetupProp3Default;           // flt_6F8B23B4
extern const float g_camCursorModeRotation0;         // flt_6F936444
extern const float g_camPresetAngleOfAttack0;        // flt_6F936438

extern const CFloat g_CFloatZero;                    // dword_6FAAE470
extern const unsigned int g_unk6FAAE624;             // dword_6FAAE624

static const float* AsFloatPtr(const void* p) { return (const float*)p; }

//  Slot 6 (+0x18/4) of whatever object each property's own m_vtable
//  names - see cameraregisterproperties.cpp's own header for why this is
//  a raw vtable-slot dispatch rather than a declared virtual call.
static __forceinline void RegisterProp(SUnitTrackedRef* prop, int id,
                                       int context, const float* value)
{
    typedef void (__thiscall *Fn)(void*, int, int, int, const float*,
                                  const float*, const float*, int);
    ((Fn)(*(void***)prop)[0x18 / 4])(prop, id, context, 0, value,
                                     AsFloatPtr(&g_CFloatZero),
                                     AsFloatPtr(&g_unk6FAAE624), 2);
}

int CCameraSetup::Method_0x20(int context)
{
    //  sub_6F471BE0 - CAgent::Method_0x20(context), always 0.
    int id = CAgent::Method_0x20(context);

    float v;

    v = g_cameraDefaultTargetDistance;
    RegisterProp(&m_prop0, id, context, &v);
    ++id;

    v = g_cameraDefaultFarZ;
    RegisterProp(&m_prop1, id, context, &v);
    ++id;

    v = g_cameraDefaultNearZ;
    RegisterProp(&m_prop2, id, context, &v);
    ++id;

    v = g_camSetupProp3Default;
    RegisterProp(&m_prop3, id, context, &v);
    ++id;

    RegisterProp(&m_prop6, id, context, AsFloatPtr(&g_CFloatZero));
    ++id;

    v = g_camCursorModeRotation0;
    RegisterProp(&m_prop4, id, context, &v);
    ++id;

    v = g_camPresetAngleOfAttack0;
    RegisterProp(&m_prop5, id, context, &v);
    ++id;

    RegisterProp(&m_prop7,  id, context, AsFloatPtr(&g_CFloatZero));  ++id;
    RegisterProp(&m_prop8,  id, context, AsFloatPtr(&g_CFloatZero));  ++id;
    RegisterProp(&m_prop9,  id, context, AsFloatPtr(&g_CFloatZero));  ++id;
    RegisterProp(&m_prop10, id, context, AsFloatPtr(&g_CFloatZero));  ++id;
    RegisterProp(&m_prop11, id, context, AsFloatPtr(&g_CFloatZero));  ++id;
    RegisterProp(&m_prop12, id, context, AsFloatPtr(&g_CFloatZero));  ++id;
    RegisterProp(&m_prop13, id, context, AsFloatPtr(&g_CFloatZero));  ++id;
    RegisterProp(&m_prop14, id, context, AsFloatPtr(&g_CFloatZero));  ++id;
    RegisterProp(&m_prop15, id, context, AsFloatPtr(&g_CFloatZero));  ++id;
    RegisterProp(&m_prop16, id, context, AsFloatPtr(&g_CFloatZero));  ++id;

    return id;
}
