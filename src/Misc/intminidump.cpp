//============================================================================
//  0x6F472EB0 - SIntMiniValue::DumpTo.  See textsink.h.
//
//  Own translation unit: every caller (CWidget::DumpState, CItem::DumpState)
//  reaches it with a real `call`, several times over.
//============================================================================
#include "textsink.h"

void SIntMiniValue::DumpTo(void* sink)
{
    LogFormatted(sink, "[IntMini]");
    LogFormatted(sink, "    val[0x%X]", m_value);
}
