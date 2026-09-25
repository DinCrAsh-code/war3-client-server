//============================================================================
//  0x6F2AC770 - CWidget's vtable slot 17 (+0x44).  See widget.h.
//
//  Dumps exactly two of the widget's words, m_flags at +0x20 and
//  m_footprintType at +0x30, each as an "[IntMini]" value
//  (widget_intmini.cpp).  Nothing else about the object is printed, which
//  is a small piece of evidence on its own: those two are the fields the
//  shipped build considered worth a debug dump.
//============================================================================
#include "widget.h"
#include "textsink.h"

void CWidget::DumpState(void* sink)
{
    ((SIntMiniValue*)&m_flags)->DumpTo(sink);
    ((SIntMiniValue*)&m_footprintType)->DumpTo(sink);
}
