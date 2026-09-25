//============================================================================
//  0x6F75E870 - one pickable object against the ray: its bounds sphere
//  first, then its own collision geometry, then every node list it carries
//  and - when the caller asked for them - every animated shape.
//
//  The sphere test is a hard reject: a miss there answers 0 without looking
//  at anything else.  Everything after it keeps the nearest hit, and the
//  distance is written out unconditionally on the way through.
//============================================================================
#include "modelpick.h"
#include "renderthunks.h"

//  0x6F744390 - Containers/sweep_array_indexers.cpp owns the body; the
//  struct is re-declared here exactly as that file declares it, so the two
//  spellings are one mangled name.
struct SStridedArrayOwner
{
    void* __thiscall At76_6F744390(int index);
};

struct PICKOBJECT
{
    int          m_recordId;        // +0x00
    char         m_reserved04[0x54];// +0x04
    unsigned int m_nodeCount;       // +0x58
    char*        m_nodes;           // +0x5C
    char         m_reserved60[0x0C];// +0x60
    unsigned int m_shapeCount;      // +0x6C
    char*        m_shapes;          // +0x70
};

struct PICKRECORD
{
    char         m_reserved00[0x10];// +0x00
    unsigned int m_count;           // +0x10
    void*        m_a;               // +0x14
    char         m_reserved18[0x28];// +0x18
    void*        m_b;               // +0x40
    void*        m_c;               // +0x44
};

//  A member and not a free `__fastcall`: 0x6F75E870 takes the owner in ecx
//  and cleans twenty bytes, and `__fastcall` would put the first argument in
//  edx and clean sixteen - an abi_audit FATAL.
struct PICKOWNER
{
    //  0x6F75E870.
    int Pick(void* object, const float* origin, const float* direction,
             float* outDistance, int includeShapes);

    char                 m_reserved000[0x14C];  // +0x000
    SStridedArrayOwner*  m_records;             // +0x14C
};

int PICKOWNER::Pick(void* object, const float* origin,
                    const float* direction, float* outDistance,
                    int includeShapes)
{
    PICKOWNER* owner = this;
    PICKOBJECT* self = (PICKOBJECT*)object;
    float best = g_pickInfinity2;

    float centre[3];
    centre[0] = 0.0f;
    centre[1] = 0.0f;
    centre[2] = 0.0f;

    //  One slot for three jobs, exactly as the shipped code uses it: the
    //  radius GetPickBounds writes, the radius SpherePick is handed, and the
    //  parameter every test after it answers through.
    float t;
    GetPickBounds(self, centre, &t);

    if (SpherePick(origin, direction, centre, t, &t) == 0)
        return 0;

    int hit = 0;

    if (self->m_recordId != -1)
    {
        PICKRECORD* record = (PICKRECORD*)
            owner->m_records->At76_6F744390(self->m_recordId);

        if (record != 0 && record->m_count != 0)
        {
            float scratch;
            if (ShapePick(origin, direction, (void*)record->m_count,
                          record->m_a, 3, record->m_b, record->m_c, &t,
                          &scratch) != 0)
            {
                hit = 1;
                if (t < best)
                    best = t;
            }
        }
    }

    for (unsigned int i = 0; i < self->m_nodeCount; i++)
    {
        if (NodeListPick(self->m_nodes + i * 0x24, origin, direction, &t) != 0)
        {
            hit = 1;
            if (t < best)
                best = t;
        }
    }

    if (includeShapes != 0)
    {
        for (unsigned int j = 0; j < self->m_shapeCount; j++)
        {
            if (ShapeListPick(self->m_shapes + j * 0x50, origin, direction,
                              &t) != 0)
            {
                hit = 1;
                if (t < best)
                    best = t;
            }
        }
    }

    *outDistance = best;
    return hit;
}
