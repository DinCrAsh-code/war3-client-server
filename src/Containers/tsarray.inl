//============================================================================
//  TSGrowableArray's out-of-line members.  Included only by the translation
//  unit that owns an instantiation; a caller sees the declarations in
//  storm.h and emits a call, as the shipped code does.
//============================================================================
#include "storm.h"
//  What the shipped code hands Storm in the __FILE__ slot: the element type's
//  RTTI descriptor name.  Reproduced as a literal because this build has RTTI
//  off, so typeid(BUCKET).raw_name() is not available to generate it.
//  One instantiation per translation unit, so the tag is a macro the
//  including file may set before the include; the relationship table's own
//  element type stays the default because tsarray.cpp was here first.
#ifndef TSARRAY_TYPENAME
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UAGILE_TYPE_RELATIONSHIP@AGILE_TYPE_DATA@@$0?CCCCCCCD@@@"
#endif
static const char s_typeName[] = TSARRAY_TYPENAME;

//----------------------------------------------------------------------------
//  0x6F00AAA0 and siblings - release the array's own storage.  Every element
//  still live gets destroyed first, exactly as SetCount(0) would do it; for
//  every T this dump instantiates it over, that loop has nothing to do (a
//  pointer, a plain int or a POD struct all have trivial destructors) and
//  the whole thing optimises down to the one Storm call the disassembly
//  shows.
//----------------------------------------------------------------------------
template <class T>
TSGrowableArray<T>::~TSGrowableArray()
{
    for (unsigned int i = 0; i < m_count; i++)
        m_data[i].~T();
    if (m_data)
        SMemFree(m_data, s_typeName, -2, 0);
}

//----------------------------------------------------------------------------
//  0x6F467B00 - pick the granularity to grow in: the largest power of two not
//  above the requested count, capped at the number of elements that fit in
//  256 bytes, which for a 12-byte list is 21.  Reaching the cap latches it
//  into m_chunk so the search is done once.
//----------------------------------------------------------------------------
template <class T>
unsigned int TSGrowableArray<T>::ComputeChunk(unsigned int count)
{
    const unsigned int maxchunk = 256 / sizeof(T);

    if (count < maxchunk)
    {
        unsigned int chunk = count;
        while ((chunk & (chunk - 1)) != 0)
            chunk = chunk & (chunk - 1);

        if (chunk < 1)
            return 1;
        return chunk;
    }

    m_chunk = maxchunk;
    return maxchunk;
}

//----------------------------------------------------------------------------
//  0x6F46DFB0 - change the allocated size.  Storm is asked to resize in place
//  first (flags 0x10); when it will not, a fresh block is allocated and the
//  surviving elements are moved into it one at a time, because a list element
//  cannot be copied bytewise - its terminator points at itself.
//----------------------------------------------------------------------------
template <class T>
void TSGrowableArray<T>::SetAlloc(unsigned int alloc)
{
    T* olddata = m_data;

    if (alloc < m_count)
    {
        for (unsigned int i = alloc; i < m_count; i++)
            olddata[i].~T();
    }

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
//  0x6F46F0C0 - change the used size, constructing or destroying the elements
//  the change adds or drops.  Both exits store the new count, so it is one
//  statement after the branch and not one per arm.
//----------------------------------------------------------------------------
template <class T>
void TSGrowableArray<T>::SetCount(unsigned int count)
{
    if (count > m_count)
    {
        if (count > m_alloc)
        {
            unsigned int chunk = m_chunk;
            if (chunk == 0)
                chunk = ComputeChunk(count);

            unsigned int alloc = count;
            unsigned int over = count % chunk;
            if (over != 0)
                alloc = count + (chunk - over);

            SetAlloc(alloc);
        }

        for (unsigned int i = m_count; i < count; i++)
            new (&m_data[i]) T;
    }
    else if (count < m_count)
    {
        for (unsigned int i = count; i < m_count; i++)
            m_data[i].~T();
    }

    m_count = count;
}

//----------------------------------------------------------------------------
//  0x6F0A4730 (instantiated for SCheckedUnitSlot, i.e. the binary's own
//  CAgentPtr<CUnit>) - append one default-constructed element.
//
//  It is SetCount(m_count + 1)'s grow with the `count > m_count` test
//  dropped - for count = m_count + 1 that test is always true and the
//  shipped code has no branch for it - followed by the element's own
//  placement-new construction.  The null test in front of that constructor
//  is MSVC's, not the source's, and it is what makes a failed allocation
//  answer null rather than fault.
//----------------------------------------------------------------------------
template <class T>
T* TSGrowableArray<T>::New()
{
    unsigned int count = m_count + 1;
    if (count > m_alloc)
    {
        unsigned int chunk = m_chunk;
        if (chunk == 0)
            chunk = ComputeChunk(count);

        unsigned int alloc = count;
        unsigned int over = count % chunk;
        if (over != 0)
            alloc = count + (chunk - over);

        SetAlloc(alloc);
    }

    T* slot = &m_data[m_count];
    m_count++;

    return new (slot) T(0);
}

//----------------------------------------------------------------------------
//  0x6F2867E0 (UnitQueryPredicateFn) / 0x6F286700 (SUnitQuery*) - the same
//  grow, handing the caller a raw slot to fill in itself.
//
//  New()'s body with its one placement `new` dropped, and that is the whole
//  difference: the shipped streams have neither the store the construction
//  would emit nor the null test MSVC wraps every placement `new` in, so
//  these are not New() over a POD - they are a separate member.
//----------------------------------------------------------------------------
template <class T>
T* TSGrowableArray<T>::Grow()
{
    unsigned int count = m_count + 1;
    if (count > m_alloc)
    {
        unsigned int chunk = m_chunk;
        if (chunk == 0)
            chunk = ComputeChunk(count);

        unsigned int alloc = count;
        unsigned int over = count % chunk;
        if (over != 0)
            alloc = count + (chunk - over);

        SetAlloc(alloc);
    }

    T* slot = &m_data[m_count];
    m_count++;

    return slot;
}

//----------------------------------------------------------------------------
//  0x6F379640 (CAgentPtr<CUnit>) - destroy every live element and resize the
//  block outright.  Storm is asked with plain flags (no 0x10 "resize in
//  place or fail"), and there is no alloc/copy/free fallback behind it - it
//  has nothing to move, because the loop above has just destroyed
//  everything.  For every T this file's own instantiations use it over (a
//  POD element, no user-defined destructor), that loop optimises down to
//  nothing and the whole member is the two field stores plus the one Storm
//  call the disassembly shows.
//----------------------------------------------------------------------------
template <class T>
void TSGrowableArray<T>::ReleaseAndReserve(unsigned int alloc)
{
    for (unsigned int i = 0; i < m_count; i++)
        m_data[i].~T();

    m_alloc = alloc;
    if (m_data != 0 || alloc != 0)
        m_data = (T*)SMemReAlloc(m_data, alloc * sizeof(T), s_typeName, -2, 0);
}

//----------------------------------------------------------------------------
//  0x6F37B6D0 (CAgentPtr<CUnit>) - ReleaseAndReserve(count) followed by a
//  placement-copy of `count` elements out of another array and a store of
//  the new count.  Written as a placement `new`, not a plain assignment:
//  the per-element null test every instantiation of this member carries
//  (`add eax,ecx; je ...` ahead of the copy) is MSVC's own placement-new
//  null guard, the same one Render/texlayerassign.cpp's own
//  CTexLayerArray::Assign already documents - for a POD T the constructor
//  call it guards is a raw element-sized `mov` chain, which is why the
//  shipped streams never show an actual call per element.
//----------------------------------------------------------------------------
template <class T>
void TSGrowableArray<T>::Assign(unsigned int count, const T* src)
{
    ReleaseAndReserve(count);

    for (unsigned int i = 0; i < count; i++)
        new (&m_data[i]) T(src[i]);

    m_count = count;
}
