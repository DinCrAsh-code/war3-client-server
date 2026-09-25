//============================================================================
//  0x6F502750 - CTexLayerArray::SetAlloc.
//
//  Every layer still live is destroyed first - which for a CTexLayer is the
//  two references it holds, released in the same two-turn loop the copy
//  constructor takes them in - and then the storage moves.  Above one layer
//  it is a Storm block under the element type's own RTTI descriptor name;
//  at one or zero it is the array's own inline slot, which is why the small
//  arm frees the heap block and points m_data back at m_inline.
//
//  The `m_data = 0` in the large arm before the reallocation is the shipped
//  code's: growing out of the inline slot must not hand Storm a pointer into
//  the array itself.
//============================================================================
#include "material.h"
#include "refcnt.h"

static const char s_typeName[] = ".?AUCTexLayer@@";

void CTexLayerArray::SetAlloc(unsigned int alloc)
{
    for (unsigned int i = 0; i < m_count; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            CTexture* texture = m_data[i].m_textures[j];
            if (texture != 0)
                ((TRefCnt*)texture)->Release();
        }
    }

    if (alloc > 1)
    {
        if (m_alloc <= 1)
            m_data = 0;

        m_data = (CTexLayer*)SMemReAlloc(m_data, alloc * sizeof(CTexLayer),
                                         s_typeName, -2, 0);
        m_alloc = alloc;
    }
    else
    {
        if (m_alloc > 1)
            SMemFree(m_data, s_typeName, -2, 0);

        m_alloc = alloc;
        m_data = (CTexLayer*)m_inline;
    }
}
