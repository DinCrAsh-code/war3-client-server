//============================================================================
//  CCinematicFilter - the 0xA4-byte filter object CWorldFrameWar3 embeds at
//  +0x254 (0x6F39AB01).  The class, its vtable literal and its seven array
//  types all came out of GameUI/cinematicfilter.cpp, which is where the
//  constructor is reconstructed; they live in a header only because the
//  world frame's own constructor has to name the type to construct it as a
//  member rather than through a placement new, and a placement new is not
//  what the shipped stream has.
//============================================================================
#ifndef CINEMATICFILTER_H
#define CINEMATICFILTER_H

#include "renderdevice.h"       // TEXTUREIMAGE
#include "storm.h"
#include "unitcreateparams.h"   // SUnitCreateParamsSub, 0x6F011300

//: 0x6F93D834 - CCinematicFilter's own vtable, written as a literal for the
//: reason the header note gives.
const unsigned int kCinematicFilterVftable = 0x6F93D834u;

struct SFilterPair
{
    float m_x;      // +0x00
    float m_y;      // +0x04
};

//  +0x40 - the only one of the seven with an out-of-line SetCount.
struct SFilterSlotArray
{
    //  0x6F3079D0 - GameUI/camerafilterslotarray.cpp.  `retn 4`.
    void SetCount(unsigned int count);
    //  0x6F303580 - GameUI/camerafilterslotarray.cpp.  `retn 4`.
    void SetAlloc(unsigned int alloc);

    unsigned int m_alloc;   // +0x00
    unsigned int m_count;   // +0x04
    void*        m_data;    // +0x08
};

struct SFilterWordArray
{
    //  0x6F34C1E0 - `retn 4`.  Still a redirect.
    void SetAlloc(unsigned int alloc);

    //  Inlined at its one call site.  No element construction: a word has
    //  nothing to build, and the shipped body has no loop.
    __forceinline void SetCount(unsigned int count)
    {
        if (m_count != count)
        {
            SetAlloc(count);
            m_count = count;
        }
    }

    unsigned int    m_alloc;    // +0x00
    unsigned int    m_count;    // +0x04
    unsigned short* m_data;     // +0x08
};

struct SFilterPairArray
{
    //  0x6F34C150 - `retn 4`.  Still a redirect.
    void SetAlloc(unsigned int alloc);

    __forceinline void SetCount(unsigned int count)
    {
        if (m_count != count)
        {
            SetAlloc(count);

            for (unsigned int i = m_count; i < count; i++)
            {
                SFilterPair* element = &m_data[i];
                if (element != 0)
                {
                    element->m_x = 0.0f;
                    element->m_y = 0.0f;
                }
            }

            m_count = count;
        }
    }

    unsigned int m_alloc;   // +0x00
    unsigned int m_count;   // +0x04
    SFilterPair* m_data;    // +0x08
};

struct SFilterIntArray
{
    //  0x6F311880 - `retn 4`.  Still a redirect.
    void SetAlloc(unsigned int alloc);

    __forceinline void SetCount(unsigned int count)
    {
        if (m_count != count)
        {
            SetAlloc(count);

            for (unsigned int i = m_count; i < count; i++)
            {
                int* element = &m_data[i];
                if (element != 0)
                    *element = 0;
            }

            m_count = count;
        }
    }

    unsigned int m_alloc;   // +0x00
    unsigned int m_count;   // +0x04
    int*         m_data;    // +0x08
};

struct CCinematicFilter
{
    CCinematicFilter();     // 0x6F368400, `retn 0`
    //  0x6F34F070 - GameUI/cinematicfilterdtor.cpp.  `retn 0`.
    ~CCinematicFilter();

    void*                m_vtable;      // +0x00
    float                m_float04;     // +0x04
    float                m_float08;     // +0x08
    int                  m_field0C;     // +0x0C
    //  +0x10 .. +0x1C - SUnitCreateParamsSub is twelve bytes wide, which is
    //  exactly the gap: the next field the constructor writes is +0x1C.
    SUnitCreateParamsSub m_params;      // +0x10
    int                  m_field1C;     // +0x1C
    TEXTUREIMAGE         m_image;       // +0x20
    int                  m_field38;     // +0x38
    unsigned int         m_flags3C;     // +0x3C
    SFilterSlotArray     m_slots;       // +0x40
    SFilterWordArray     m_order;       // +0x4C
    SFilterPairArray     m_pairsA;      // +0x58
    SFilterPairArray     m_pairsB;      // +0x64
    SFilterPairArray     m_pairsC;      // +0x70
    int                  m_field7C;     // +0x7C
    SFilterIntArray      m_intsA;       // +0x80
    SFilterIntArray      m_intsB;       // +0x8C
    //  +0x98 .. +0xA4 - the last array, and the last member: the store at
    //  0x6F3684D9 to +0xA0 is its own m_data, not a field of its own.  The
    //  class is 0xA4 bytes, which is exactly the gap CWorldFrameWar3 leaves
    //  for it between +0x254 and +0x2F8.
    SFilterIntArray      m_intsC;       // +0x98
};

#endif
