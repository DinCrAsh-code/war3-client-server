//============================================================================
//  0x6F743E60 - one animated collision shape against the ray.
//
//  The shape is skipped outright when its +0x34 is null.  The engine-side
//  test is handed the shape's two descriptors, the constant 3 that says
//  which kind it is, its geometry and transform, and two out slots - one of
//  which is this function's own first parameter slot, reused, which is why
//  the answer is read back out of it with `fld [esp+4+arg_0]`.
//============================================================================
#include "modelpick.h"
#include "renderthunks.h"

struct PICKSHAPE
{
    char  m_reserved00[4];      // +0x00
    void* m_a;                  // +0x04
    void* m_b;                  // +0x08
    char  m_reserved0C[0x28];   // +0x0C
    void* m_geometry;           // +0x34
    void* m_transform;          // +0x38
};

int __stdcall ShapeListPick(void* shape, const float* origin,
                            const float* direction, float* outDistance)
{
    const PICKSHAPE* self = (const PICKSHAPE*)shape;
    if (self->m_geometry == 0)
        return 0;

    float t;
    int hit = ShapePick(origin, direction, self->m_a, self->m_b, 3,
                        self->m_geometry, self->m_transform, &t,
                        (void*)&direction);
    if (hit != 0 && outDistance != 0)
        *outDistance = t;

    return hit;
}
