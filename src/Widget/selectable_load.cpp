//============================================================================
//  0x6F2C7790 - CSelectable's vtable slot 15 (+0x3C).  See selectable.h.
//
//  CSelectable::Save's counterpart (selectable_save.cpp): the base class's
//  whole load record through a direct call, then the one byte that is
//  CSelectable's own.
//============================================================================
#include "selectable.h"
#include "cdatastore.h"

void CSelectable::Load(CDataStore* store)
{
    CWidget::Load(store);

    unsigned char flag = 0;
    store->ReadByte(&flag);
    m_attachmentRefreshFlag = flag;
}
