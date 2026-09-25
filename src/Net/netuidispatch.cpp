//============================================================================
//  0x6F53F350 - hand a UI message to one session record's UI observer list.
//
//  The record is indexed the way every other helper of this module indexes
//  it - `records + index * 0x304` - and the receiver is that record's +8
//  view, which is CNetPlayerRecord (netdata.h says why the two are modelled
//  as separate classes).
//
//  Own translation unit: the thread-local chain and the dispatch forwarder
//  are both real calls.
//============================================================================
#include "netdata.h"

void __fastcall DispatchSessionUiMessage(unsigned int msg, unsigned int arg1,
                                         void* arg2, int index)
{
    SNetDataSlotRoot* root = (SNetDataSlotRoot*)GetThreadLocalSlot(0x0D);

    root->m_pCache->m_pRecords[index].Session()
        ->DispatchToUiObservers(msg, arg1, arg2);
}
