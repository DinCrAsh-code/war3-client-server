//============================================================================
//  0x6F2C7760 - CSelectable's vtable slot 14 (+0x38).  See selectable.h.
//
//  The base class's whole save record (a direct `call sub_6F2AC790`, not a
//  vtable dispatch - this *is* the override) plus one byte: whether
//  m_attachmentRefreshFlag is set.  That is the entire difference between a
//  CSelectable's save record and a CWidget's.
//
//  Own translation unit: two real calls.
//============================================================================
#include "selectable.h"
#include "cdatastorescratch.h"

void CSelectable::Save(CDataStoreScratch* store)
{
    CWidget::Save(store);
    store->WriteByte((unsigned char)(m_attachmentRefreshFlag != 0));
}
