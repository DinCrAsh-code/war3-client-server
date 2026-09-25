//============================================================================
//  0x6F2AC790 - CWidget's vtable slot 14 (+0x38).  See widget.h.
//
//  The widget's own save record, and a compact statement of what a CWidget
//  actually *is*: the header (widget_saveheader.cpp), then the footprint
//  type, both player bitmasks, a "has a pathing registration" flag and that
//  registration's own contents if it has one, two more flags derived from
//  +0x38 and +0x3C, and finally +0x40 as a CFloat.
//
//  Three details are load-bearing:
//   * +0x2C/+0x2E go out as *words*, which is independent confirmation that
//     they are the two 16-bit player bitmasks widget.h calls them and not
//     one dword;
//   * the +0x38 flag is `>= 0` (`setnl`), signed - the same
//     negative-means-unset reading CWidget::ReleaseHandle38 already has -
//     while the +0x34 and +0x3C flags are plain `!= 0`;
//   * +0x40 goes through the CFloat writer, not the dword one, which is the
//     first thing in any dump to give that field a type.
//
//  Own translation unit: eight real calls.
//============================================================================
#include "widget.h"
#include "widgetpathrefadjust.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store,
                                           const CFloat* value);

void CWidget::Save(CDataStoreScratch* store)
{
    CWar3Image::Save(store);

    store->WriteDword(m_footprintType);
    store->WriteWord(m_playerMaskA);
    store->WriteWord(m_playerMaskB);

    store->WriteByte((unsigned char)(m_pathGridRegistration != 0));
    if (m_pathGridRegistration != 0)
        ((SPathRefArray*)m_pathGridRegistration)->Save(store);

    store->WriteByte((unsigned char)((int)m_reserved38 >= 0));
    store->WriteByte((unsigned char)(m_reserved3C != 0));

    WriteCFloat(store, (const CFloat*)&m_reserved40);
}
