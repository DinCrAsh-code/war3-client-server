//============================================================================
//  TSFixedArray's out-of-line members.  Included only by the translation
//  unit that owns an instantiation, exactly as tsarray.inl is.
//============================================================================
#include "storm.h"

//  The element type's RTTI descriptor name, in the __FILE__ slot Storm's
//  allocator wants.  Reproduced as a literal because this build has RTTI
//  off; the shipped tag for the one instantiation here is
//  ".PAVAgileAgentQuery@@", read straight out of the image at 0x6FA833A4.
#ifndef TSFIXEDARRAY_TYPENAME
#define TSFIXEDARRAY_TYPENAME ".PAVAgileAgentQuery@@"
#endif
static const char s_typeName[] = TSFIXEDARRAY_TYPENAME;

//----------------------------------------------------------------------------
//  0x6F469880 - change the allocated size.  Byte for byte the same body
//  TSGrowableArray::SetAlloc (tsarray.inl, 0x6F46DFB0) has: Storm is asked
//  to resize in place first (flags 0x10), and only when it refuses is a
//  fresh block allocated and the surviving elements moved into it one at a
//  time.
//----------------------------------------------------------------------------
template <class T>
void TSFixedArray<T>::SetAlloc(unsigned int alloc)
{
    T* olddata = m_data;

    m_alloc = alloc;
    m_data = (T*)SMemReAlloc(olddata, alloc * sizeof(T), s_typeName, -2, 0x10);
    if (m_data == 0)
    {
        m_data = (T*)SMemAlloc(alloc * sizeof(T), s_typeName, -2, 0);
        if (olddata != 0)
        {
            unsigned int moved = alloc < m_count ? alloc : m_count;
            for (unsigned int i = 0; i < moved; i++)
            {
                new (&m_data[i]) T(olddata[i]);
                olddata[i].~T();
            }
            SMemFree(olddata, s_typeName, -2, 0);
        }
    }
}

//----------------------------------------------------------------------------
//  0x6F46A950 - change the used size.  Unlike TSGrowableArray::SetCount
//  there is no chunk to round up to, so the allocation is sized to exactly
//  the count asked for - and a count of zero releases the block outright
//  rather than leaving an empty allocation behind.
//
//  The grow arm default-constructs every newly added slot with a
//  *default*-initialising placement `new` (no parens): for a scalar T
//  (AgileAgentQuery*, this template's first instantiation) that leaves the
//  slot's bytes untouched and the compiler drops the loop entirely, which
//  is why 0x6F46A950 has no construction code at all past the SetAlloc
//  call. AuFixedStringArray::SetCount (0x6F553C50,
//  Render/fontfileversionarray.cpp) is the same template over a type with a
//  real default constructor, and its dump shows exactly this loop's one
//  instruction (`mov byte ptr [ecx],0`) inlined between the SetAlloc call
//  and the m_count store - see that file for the full story.
//----------------------------------------------------------------------------
template <class T>
void TSFixedArray<T>::SetCount(unsigned int count)
{
    if (count != m_count)
    {
        if (count == 0)
        {
            if (m_data)
                SMemFree(m_data, s_typeName, -2, 0);

            m_alloc = 0;
            m_count = 0;
            m_data = 0;
        }
        else
        {
            SetAlloc(count);
            for (unsigned int i = m_count; i < count; i++)
                new (&m_data[i]) T;
            m_count = count;
        }
    }
}
