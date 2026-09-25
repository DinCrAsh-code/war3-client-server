//============================================================================
//  0x6F28C750 - SUnitQuery::SUnitQuery: the query record's own constructor.
//
//  The module is ground truth rather than a guess.  Its one caller,
//  MakeUnitQuery (unitquerypool.cpp), hands Storm the __FILE__ string
//  "e:\drive1\temp\buildwar3x\war3\source\unit\CUnit_Enum.cpp" and a size
//  of 0x94 - so this record is `CUnit_Enum.cpp`'s own, it really is 0x94
//  bytes, and the RTTI descriptor its two arrays are allocated under
//  (`.PAUTEnumData@@`, `.P6IHPAVCUnit@@PAUTEnumData@@@Z`) says the shipped
//  name for the record is `TEnumData` and for a filter
//  `int (__fastcall*)(CUnit*, TEnumData*)`.  The established SUnitQuery /
//  UnitQueryPredicateFn names are kept - renaming the type would touch
//  every one of BuildAndRegisterEventBinding's own call sites for no
//  instruction - but the evidence is written down here so nobody has to
//  re-derive it.
//
//  Three things it does and nothing else:
//
//   * zero the three words at +0x18/+0x1C/+0x20 that
//     BuildAndRegisterEventBinding's own initialiser block deliberately
//     leaves alone (agenteventbinding.h says why), and all four words of
//     each of the three arrays.  There is no TSGrowableArray constructor
//     in the shipped stream: member constructors would have run *before*
//     the body's own three stores, and these run after, so all twelve
//     stores are the body's;
//   * SetCount(0x1F) on the pre-filter array and SetCount(0x100) on the
//     candidate array, both inlined - which is what a template member
//     defined in the same translation unit and used once gets at /Ob2,
//     while the ComputeChunk/SetAlloc it calls stay real calls (the same
//     split TSGrowableArray<T>::Grow already shows);
//   * stamp 0x80 into the candidate array's growth granularity, *after*
//     the reserve, so the reserve itself goes through ComputeChunk and
//     latches the 0x40 cap first.
//
//  Own translation unit: MakeUnitQuery calls it for real.
//============================================================================
#include "agenteventbinding.h"

//----------------------------------------------------------------------------
//  SetCount(), forced inline for these two instantiations and no others.
//
//  The shipped constructor has both reserves written out with no call in
//  between; MSVC's /Ob2 cost model declines a body this size on its own
//  (measured - two `call ...::SetCount` where the shipped stream has the
//  arithmetic).  It cannot be `__forceinline` in storm.h, because five
//  other instantiations of SetCount are real, separately addressed
//  functions that TSHashTable::Rehash and CGridSize call for real
//  (0x6F46F0C0, 0x6F3A3D70, 0x6F3A3C60, 0x6F42D690, 0x6F3234F0) - so it is
//  forced here, where the shipped code shows it inlined, and nowhere else.
//  Same reasoning storm.h gives for TSGrowableArray<T>::Add and
//  TSExplicitList<T>::LinkToHead, just scoped to one translation unit.
//
//  The body is SetCountBody below so the two specialisations do not carry
//  two copies of it; it is the generic template's own body verbatim.
//
//  tsarray.inl is deliberately *not* included here.  With it, MSVC has
//  ComputeChunk's body in front of it, sees the constant count, and folds
//  the whole power-of-two search into the constructor - where the shipped
//  stream calls it (0x6F274D30 / 0x6F086180).  Without it those two stay
//  declarations and the calls are real, which is the shipped shape.
//----------------------------------------------------------------------------
template <class T>
static void SetCountBody(TSGrowableArray<T>* self, unsigned int count)
{
    if (count > self->m_count)
    {
        if (count > self->m_alloc)
        {
            unsigned int chunk = self->m_chunk;
            if (chunk == 0)
                chunk = self->ComputeChunk(count);

            unsigned int alloc = count;
            unsigned int over = count % chunk;
            if (over != 0)
                alloc = count + (chunk - over);

            self->SetAlloc(alloc);
        }

        for (unsigned int i = self->m_count; i < count; i++)
            new (&self->m_data[i]) T;
    }
    else if (count < self->m_count)
    {
        for (unsigned int i = count; i < self->m_count; i++)
            self->m_data[i].~T();
    }

    self->m_count = count;
}

template <>
__forceinline void TSGrowableArray<UnitQueryPredicateFn>::SetCount(
        unsigned int count)
{
    SetCountBody(this, count);
}

template <>
__forceinline void TSGrowableArray<CUnit*>::SetCount(unsigned int count)
{
    SetCountBody(this, count);
}

SUnitQuery::SUnitQuery()
{
    //  Descending, which is the shipped store order (+0x20, +0x1C, +0x18).
    m_field20  = 0;
    m_posY     = 0;
    m_posX     = 0;

    m_pre.m_alloc = 0;
    m_pre.m_count = 0;
    m_pre.m_data  = 0;
    m_pre.m_chunk = 0;
    TSGrowableArray<UnitQueryPredicateFn>* pre = &m_pre;

    m_post.m_alloc = 0;
    m_post.m_count = 0;
    m_post.m_data  = 0;
    m_post.m_chunk = 0;

    TSGrowableArray<CUnit*>* found = &m_found;
    found->m_alloc = 0;
    found->m_count = 0;
    found->m_data  = 0;
    found->m_chunk = 0;

    pre->SetCount(0x1F);
    found->SetCount(0x100);
    m_found.m_chunk = 0x80;
}
