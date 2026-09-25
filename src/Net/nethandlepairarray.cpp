//============================================================================
//  NetHandlePairArray - see nethandlepairarray.h for the class-level notes.
//============================================================================
#include "nethandlepairarray.h"

//  0x6F2C9700
void __thiscall NetHandlePairArray::SetAlloc(unsigned int alloc)
{
    NetHandlePair* olddata = m_data;

    m_alloc = alloc;
    m_data = (NetHandlePair*)SMemReAlloc(olddata, alloc * sizeof(NetHandlePair),
                                         NETHANDLEPAIRARRAY_TAG, -2, 0x10);
    if (m_data == 0)
    {
        m_data = (NetHandlePair*)SMemAlloc(alloc * sizeof(NetHandlePair),
                                           NETHANDLEPAIRARRAY_TAG, -2, 0);
        if (olddata != 0)
        {
            unsigned int moved = (alloc < m_count) ? alloc : m_count;
            for (unsigned int i = 0; i < moved; i++)
                m_data[i] = olddata[i];

            SMemFree(olddata, NETHANDLEPAIRARRAY_TAG, -2, 0);
        }
    }
}

//  0x6F2CEC00
void __thiscall NetHandlePairArray::SetCount(unsigned int count)
{
    if (count == m_count)
        return;

    if (count != 0)
    {
        SetAlloc(count);

        for (unsigned int i = m_count; i < count; i++)
            new (&m_data[i]) NetHandlePair;

        m_count = count;
        return;
    }

    if (m_data != 0)
        SMemFree(m_data, NETHANDLEPAIRARRAY_TAG, -2, 0);

    m_alloc = 0;
    m_count = 0;
    m_data = 0;
}
