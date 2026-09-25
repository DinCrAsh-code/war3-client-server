//============================================================================
//  0x6F75CE60 - every collision shape hanging off one node list, nearest hit
//  wins.
//
//  The list is walked through the same masked-tail encoding every TSList in
//  this binary uses: the raw link is tested with a sign test and the walk
//  stops on a non-positive one.  A node with no shape array (+0x70 zero) or
//  no geometry (+0x40 zero) is skipped whole.
//============================================================================
#include "modelpick.h"
#include "renderthunks.h"

struct PICKNODE
{
    int          m_head;            // +0x00
    int          m_next;            // +0x04
    char         m_reserved08[0x14];// +0x08
    int          m_listHead;        // +0x1C
    char         m_reserved20[0x20];// +0x20
    void*        m_geometry;        // +0x40
    void*        m_transformA;      // +0x44
    char         m_reserved48[0x28];// +0x48
    unsigned int m_shapeCount;      // +0x70
    char*        m_shapes;          // +0x74
};

struct PICKSHAPEENTRY
{
    int   m_kind;                   // +0x00
    char  m_reserved04[8];          // +0x04
    void* m_b;                      // +0x0C
    void* m_c;                      // +0x10
    char  m_reserved14[4];          // +0x14
};

int __stdcall NodeListPick(void* node, const float* origin,
                           const float* direction, float* outDistance)
{
    float best = g_pickInfinity2;
    int hit = 0;

    int link = ((PICKNODE*)node)->m_listHead;
    while (link > 0)
    {
        PICKNODE* entry = (PICKNODE*)link;

        if (entry->m_geometry != 0 && entry->m_shapeCount != 0)
        {
            unsigned int remaining = entry->m_shapeCount;
            PICKSHAPEENTRY* shape = (PICKSHAPEENTRY*)entry->m_shapes;
            do
            {
                float t;
                float scratch;
                remaining--;

                hit = ShapePick(origin, direction, entry->m_geometry,
                                entry->m_transformA, shape->m_kind,
                                shape->m_b, shape->m_c, &t, &scratch);
                if (hit != 0)
                {
                    if (t < best)
                        best = t;
                }
                shape++;
            }
            while (remaining != 0);
        }

        link = entry->m_next;
    }

    if (outDistance != 0)
        *outDistance = best;

    return hit;
}
