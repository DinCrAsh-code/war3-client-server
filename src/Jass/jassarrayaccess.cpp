//============================================================================
//  JassArray's own bounds-checked element accessors - the `array[i]`/
//  `array[i] = v` JASS array indexing compiles to, alongside
//  JassArray::SetCount (jassarraysetcount.cpp) which grows the backing
//  store these two just index into.
//============================================================================
#include "jassarray.h"

//  0x6F459710
void JassArray::SetElement(unsigned int index, void* value)
{
    if (index < m_elementCount)
        m_elementData[index] = value;
}

//  0x6F459730
void* JassArray::GetElement(unsigned int index)
{
    if (index >= m_elementCount)
        return 0;
    return m_elementData[index];
}
