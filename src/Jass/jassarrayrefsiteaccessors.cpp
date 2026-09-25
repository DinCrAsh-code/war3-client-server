//============================================================================
//  JassArrayRefSite's small accessors, alongside AcquireArray
//  (jassarrayref.cpp) - see jassarray.h for what this closure resolves
//  about each field.
//============================================================================
#include "jassarray.h"

//  0x6F4594D0
void JassArrayRefSite::SetKindAndField1C(int kind, int field1C)
{
    m_kind = kind;
    m_field1C = field1C;
}

//  0x6F459500
int JassArrayRefSite::IsArrayOfKind(unsigned int baseKind)
{
    if (m_kind < 9)
        return 0;
    return (unsigned int)(m_kind - 5) == baseKind;
}

//  0x6F459520
JassArray* JassArrayRefSite::GetArray()
{
    return m_array;
}

//  0x6F459530
void JassArrayRefSite::GetArrayInto(JassArray** out)
{
    *out = m_array;
}

//  0x6F459540
int JassArrayRefSite::GetField24()
{
    return m_field24;
}

//  0x6F4596F0 - written as two guard clauses sharing one `return 0` (the
//  shape the disassembly's own `jl`-to-a-shared-tail-block shows); this
//  build's own compiler still chose the opposite branch polarity and
//  inlined the tail first instead (`jge` past a return-0 placed ahead of
//  the real logic) - see this file's own commit note for why that is
//  DIFFERS rather than a source bug.
unsigned int JassArrayRefSite::GetArrayCount()
{
    if (m_kind < 9)
        return 0;
    JassArray* arr = m_array;
    if (!arr)
        return 0;
    return arr->m_elementCount;
}
