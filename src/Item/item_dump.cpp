//============================================================================
//  0x6F2B6070 - CItem's vtable slot 17 (+0x44).  See item.h.
//
//  CWidget::DumpState's two words first (a direct base-class call, since
//  this *is* the override), then everything of CItem's own the shipped
//  build considered printable - which is a small map of the object in its
//  own right:
//
//      +0x54  m_floatB            "[FltMiniB]"  val/min/max
//      +0x84  m_position's +0x84  "[IntMini]"
//      +0x88  m_position's +0x88  "[IntMini]"
//      +0x74  m_position's +0x74  "[IntMini]"
//      +0x8C  m_position's m_placementRef  "[RefMini]"
//      +0xCC  m_position's +0xCC  "[IntMini]"
//      +0x9C  m_position's own array, m_reserved98 entries of 12 bytes
//             each, "[RefMini]" apiece
//
//  That last loop is what settles position.h's own long-standing guess that
//  +0x9C onward is "an array of three identically-shaped sub-records":
//  it is an array all right, of *four* (the count the constructor stores at
//  +0x98) twelve-byte records, and the shipped code walks it with a
//  pointer, not an index.
//
//  Own translation unit: eight real calls, and the loop body is a ninth.
//============================================================================
#include "item.h"
#include "textsink.h"

void CItem::DumpState(void* sink)
{
    CWidget::DumpState(sink);

    m_floatB.DumpTo(sink);
    ((SIntMiniValue*)&m_position.m_reserved84)->DumpTo(sink);
    ((SIntMiniValue*)&m_position.m_reserved88)->DumpTo(sink);
    ((SIntMiniValue*)&m_position.m_reserved74)->DumpTo(sink);
    ((SRefMiniValue*)&m_position.m_placementRef)->DumpTo(sink);
    ((SIntMiniValue*)&m_position.m_reservedCC)->DumpTo(sink);

    //  A pointer walk, not `for (i = 0; i < n; ++i) a[i].DumpTo(sink)`:
    //  the shipped loop advances the object pointer by 12 and counts the
    //  index down to zero, which is what a pointer-and-count loop gives and
    //  an indexed one does not (that one keeps the base live and emits a
    //  scaled address at every iteration).  The count is read once, before
    //  the loop, and the loop is skipped outright when it is zero.
    //  `> 0` and not `!= 0`: the count is unsigned and the shipped guard is
    //  `jbe`, the unsigned form, which `!= 0` lowers to a plain `je`.
    unsigned int remaining = m_position.m_reserved98;
    if (remaining > 0)
    {
        SRefMiniValue* entry = (SRefMiniValue*)&m_position.m_reserved9C;
        do
        {
            entry->DumpTo(sink);
            entry = (SRefMiniValue*)((char*)entry + 12);
            remaining--;
        }
        while (remaining != 0);
    }
}
