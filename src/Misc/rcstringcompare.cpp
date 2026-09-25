//============================================================================
//  0x6F4C4580 - RCSTRINGDATA::Equals, and 0x6F4C4610 - RCString::EqualsCStr.
//  Both fall into the shared tail at 0x6F4C4540, which has no heading of its
//  own and is therefore inlined into each of them here.
//============================================================================
#include "rcstring.h"

int RCSTRINGDATA::Equals(const RCSTRINGDATA* other) const
{
    if (other == this)
        return 1;

    return RCStringCharsEqual(m_chars, other->m_chars);
}

int RCString::EqualsCStr(const char* other) const
{
    const RCSTRINGDATA* data = m_data;
    if (data == 0)
        return other == 0;

    return RCStringCharsEqual(data->m_chars, other);
}
