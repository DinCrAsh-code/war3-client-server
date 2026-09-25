//============================================================================
//  0x6F4770E0 / 0x6F4772E0 - FloatMini::DumpTo and FloatMiniB::DumpTo.
//  See floatmini.h and textsink.h.
//
//  Own translation unit: CItem::DumpState (item_dump.cpp) reaches it with a
//  real call, and it makes four of its own.
//
//  This is the function that named FloatMiniB's two fields: the format
//  string is literally `    val[%s] min[%s] max[%s]`, fed from +0x04, +0x08
//  and +0x0C in that order.  The two locals the shipped code stages +0x08
//  and +0x0C through before formatting them are what FormatCFloat's own
//  `this`-in-ecx convention forces - it needs an address, and the fields'
//  own addresses would keep `this` live across three calls.
//============================================================================
#include "floatmini.h"
#include "textsink.h"

//----------------------------------------------------------------------------
//  0x6F4770E0 - the base class's own dump, found by CUnit::DumpState
//  (unit_dumpstate.cpp), which prints four of them.  It is FloatMini's and
//  not a separate "[FltMini]" type: the tag differs from FloatMiniB's by
//  exactly the "B", it formats one CFloat and formats it from +0x04, which
//  is where FloatMini::m_value is, and unlike FloatMiniB's it never
//  touches +0x08/+0x0C - the two fields FloatMiniB adds.
//----------------------------------------------------------------------------
void FloatMini::DumpTo(void* sink)
{
    LogFormatted(sink, "[FltMini]");
    LogFormatted(sink, "    val[%s]", FormatCFloat(&m_value, 10, 3));
}

void FloatMiniB::DumpTo(void* sink)
{
    LogFormatted(sink, "[FltMiniB]");

    CFloat lo = m_min;
    CFloat hi = m_max;
    LogFormatted(sink, "    val[%s] min[%s] max[%s]",
                 FormatCFloat(&m_value, 10, 3),
                 FormatCFloat(&lo, 10, 3),
                 FormatCFloat(&hi, 10, 3));
}
