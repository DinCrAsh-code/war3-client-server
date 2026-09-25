//============================================================================
//  0x6F73B180 - the centre and radius of one pickable object's bounds.
//
//  X and Y are the object's own +0x38/+0x3C plus a runtime offset each; Z is
//  the midpoint of +0x40 and +0x44; the radius is +0x48 unchanged.  The two
//  offsets are globals rather than constants - both words are 0xFFFFFFFF in
//  the image, so they are written at run time - which is why they are DATA
//  rows and not literals.
//============================================================================
#include "modelpick.h"

struct PICKBOUNDS
{
    char  m_reserved00[0x38];   // +0x00
    float m_x;                  // +0x38
    float m_y;                  // +0x3C
    float m_zMin;               // +0x40
    float m_zMax;               // +0x44
    float m_radius;             // +0x48
};

void __stdcall GetPickBounds(const void* object, float* outCentre,
                             float* outRadius)
{
    const PICKBOUNDS* bounds = (const PICKBOUNDS*)object;

    outCentre[0] = bounds->m_x + g_pickOriginX;
    outCentre[1] = bounds->m_y + g_pickOriginY;
    outCentre[2] = (float)((bounds->m_zMax + bounds->m_zMin) * 0.5);

    *outRadius = bounds->m_radius;
}
