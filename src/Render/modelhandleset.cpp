//============================================================================
//  0x6F504230 - SModelHandleSet::~SModelHandleSet.
//
//  The record SetSpriteModel (0x6F4D8D60) fills once per sequence through
//  0x6F505890 and throws away again at the bottom of the same loop: five
//  words, of which this destructor reads three - the count, the block, and
//  one more counted reference at +0x10.
//
//  Every element is a counted reference and is released one at a time; the
//  block itself is then handed back to Storm under the type descriptor name
//  ".PAUHMODEL__@@", i.e. "pointer to struct HMODEL__", which is what says
//  the elements are model handles and is what names the record.
//
//  The walk counts *down* from the element count and steps the pointer up,
//  which is the shipped body: it never forms an index, and writing it as an
//  indexed loop costs the whole loop its shape.
//
//  Score capped by the __except_handler4 frame the shipped build puts round
//  a destructor whose body owns a block - the `push -1`, the scope table,
//  the cookie prologue, the unwind-state store and the funclet that repeats
//  the free - which this repo's fixed /GS- /EHs-c- does not emit.
//
//  Own translation unit: two real calls out of it.
//============================================================================
#include "refcnt.h"
#include "storm.h"

//  The Storm allocator's own type-descriptor tag for the block.  A file-scope
//  constant and not a DATA name, for the reason
//  docs/msvc-vc8-idioms.md gives: the same descriptor is shared with every
//  other container over the same element type.
static const char kModelHandlePtrTypeName[] = ".PAUHMODEL__@@";

struct SModelHandleSet
{
    ~SModelHandleSet();

    unsigned int m_alloc;       // +0x00
    unsigned int m_count;       // +0x04
    TRefCnt**    m_data;        // +0x08
    int          m_field0C;     // +0x0C
    TRefCnt*     m_pRef10;      // +0x10
};

SModelHandleSet::~SModelHandleSet()
{
    unsigned int remaining = m_count;
    TRefCnt** element = m_data;

    while (remaining != 0)
    {
        TRefCnt* held = *element;
        remaining--;
        held->Release();
        element++;
    }

    if (m_pRef10 != 0)
        m_pRef10->Release();

    if (m_data != 0)
        SMemFree(m_data, kModelHandlePtrTypeName, -2, 0);
}
