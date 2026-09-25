//============================================================================
//  0x6F478550 - SRefMiniValue::DumpTo.  See textsink.h.
//
//  Own translation unit for the same reason as intminidump.cpp: CItem's own
//  slot 17 calls it five times.
//============================================================================
#include "textsink.h"

void SRefMiniValue::DumpTo(void* sink)
{
    LogFormatted(sink, "[RefMini]");
    LogFormatted(sink, "    t[%u] b[%u]", m_t, m_b);
}
