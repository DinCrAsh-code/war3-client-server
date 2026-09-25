//============================================================================
//  0x6F50B790 - the recursive pick: every drawable range of every geoset this
//  model instance carries, and then every child, keeping the nearest hit.
//
//  The out parameter starts at FLT_MAX and is only ever lowered, which is
//  what the `fcomp` / `test ah, 41h` pair in front of each store is; when
//  nothing was hit at all it is set to +infinity on the way out.  A geoset is
//  skipped when its entry says it is not drawable (+0x10), when the model's
//  own per-geoset byte table says so, or - unless the caller asked for
//  hidden geometry - when its flag bit 2 is set.
//
//  The eleven-argument engine call is the triangle-list intersection below
//  the closure boundary (0x6F7B86C0, `retn 2Ch`); renderthunks.h declares it.
//============================================================================
#include "modelpick.h"
#include "renderthunks.h"
#include "fvec3.h"

//  What the model's +0x0C/+0x10 array holds.
struct PICKENTRY
{
    char             m_reserved00[8];   // +0x00
    unsigned int     m_recordIndex;     // +0x08
    struct PICKGEOSET* m_geoset;        // +0x0C
    int              m_skip;            // +0x10
};

struct PICKRANGE
{
    int          m_kind;                // +0x00
    unsigned int m_count;               // +0x04
};

struct PICKGEOSET
{
    char         m_reserved000[0x0C];   // +0x000
    void*        m_positions;           // +0x00C
    void*        m_indices;             // +0x010
    char         m_reserved014[0x38];   // +0x014
    void*        m_matrices;            // +0x04C
    char         m_reserved050[0x78];   // +0x050
    unsigned int m_boneCount;           // +0x0C8
    PICKRANGE*   m_ranges;              // +0x0CC
    char         m_reserved0D0[0x10];   // +0x0D0
    char*        m_vertices;            // +0x0E0
    char         m_reserved0E4[0x38];   // +0x0E4
    unsigned int m_visibilityIndex;     // +0x11C
    char         m_reserved120[4];      // +0x120
    unsigned int m_flags;               // +0x124
    char         m_reserved128[0x18];   // +0x128
    unsigned int m_matrixMode;          // +0x140
};

struct PICKMODEL
{
    char         m_reserved00[0x0C];    // +0x00
    unsigned int m_entryCount;          // +0x0C
    PICKENTRY**  m_entries;             // +0x10
    char         m_reserved14[0x0C];    // +0x14
    char*        m_visibility;          // +0x20
};

int __fastcall ModelPickSegment(CModelInstance* model, const float* from,
                                const float* to, float* outDistance,
                                int includeChildren, int includeHidden)
{
    PICKMODEL* self = (PICKMODEL*)model;
    FVec3 direction;

    *outDistance = g_pickFarthest;

    direction.m_x = to[0] - from[0];
    direction.m_y = to[1] - from[1];
    direction.m_z = to[2] - from[2];
    direction.Normalize();

    int hit = 0;

    unsigned int remaining = self->m_entryCount;
    PICKENTRY** slot = self->m_entries;
    while (remaining != 0)
    {
        PICKENTRY* entry = *slot;
        remaining--;

        if (entry->m_skip == 0)
        {
            PICKGEOSET* geoset = entry->m_geoset;

            if (self->m_visibility[geoset->m_visibilityIndex * 16 + 3] != 0 &&
                (includeHidden != 0 || (geoset->m_flags & 4) == 0))
            {
                char* vertices = geoset->m_vertices;
                PICKRANGE* range = geoset->m_ranges;
                unsigned int ranges = geoset->m_boneCount;

                while (ranges != 0)
                {
                    ranges--;

                    if (range->m_kind >= 3)
                    {
                        float distance;
                        float parameter;

                        if (TriangleListPick(
                                from, &direction.m_x,
                                PickRecordAt_6F509420(entry->m_recordIndex),
                                geoset->m_positions, geoset->m_indices, 0x0C,
                                geoset->m_matrices,
                                geoset->m_matrixMode == 1,
                                range->m_kind, range->m_count, vertices,
                                &parameter, &distance) != 0)
                        {
                            hit = 1;
                            if (parameter < *outDistance)
                                *outDistance = parameter;
                        }
                    }

                    vertices += range->m_count * 2;
                    range++;
                }
            }
        }

        slot++;
    }

    if (includeChildren != 0 && (model->m_flags & 0x10) != 0 &&
        model->m_childListCount != 0)
    {
        unsigned int lists = model->m_childListCount;
        char* list = (char*)model->m_childLists + 8;
        do
        {
            int link = *(int*)list;
            lists--;

            while (link > 0)
            {
                float childDistance;
                if (ModelPickSegment(*(CModelInstance**)(link + 8), from, to,
                                     &childDistance, 1, includeHidden) != 0)
                {
                    hit = 1;
                    if (childDistance < *outDistance)
                        *outDistance = childDistance;
                }
                link = *(int*)(link + 4);
            }

            list += 0x0C;
        }
        while (lists != 0);
    }

    if (hit != 0)
        return 1;

    *outDistance = g_pickInfinity;
    return 0;
}
