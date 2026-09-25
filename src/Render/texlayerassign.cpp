//============================================================================
//  0x6F503700 - CTexLayerArray::Assign: size the array and copy-construct
//  every layer into it.
//
//  The `if (dst != 0)` in front of the copy constructor is MSVC's own
//  placement-new null test and not a source statement, which is what says
//  the loop body is `new (&m_data[i]) CTexLayer(src[i])` rather than an
//  assignment - CTexLayer has no assignment operator to call.
//============================================================================
#include "material.h"

void CTexLayerArray::Assign(unsigned int count, const CTexLayer* src)
{
    SetAlloc(count);

    for (unsigned int i = 0; i < count; i++)
        new (&m_data[i]) CTexLayer(src[i]);

    m_count = count;
}
