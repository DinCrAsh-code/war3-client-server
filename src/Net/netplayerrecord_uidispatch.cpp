//============================================================================
//  0x6F5378C0 - CNetPlayerRecord::DispatchToUiObservers: hand a message to
//  the UI observer list embedded at +0x10.
//
//  Four instructions, and all four are what an ordinary virtual call on a
//  member subobject compiles to when the callee's stack arguments are the
//  caller's own: the vftable out of the subobject (`mov eax,[ecx+10h]`),
//  slot 2 out of that, the receiver adjusted to the subobject
//  (`add ecx,10h`) and a tail `jmp`.  Nothing is pushed here - the three
//  dwords the caller pushed are already in place, and the observer's own
//  `retn 0Ch` cleans them.
//
//  Own translation unit: 0x6F53F350 calls it for real, and its sibling
//  FireToObserverLists (0x6F5378E0) is in netcommand_fire.cpp.
//============================================================================
#include "netdata.h"
#include "observer.h"

void CNetPlayerRecord::DispatchToUiObservers(unsigned int msg,
                                             unsigned int arg1, void* arg2)
{
    m_uiObserverList.Dispatch(msg, arg1, arg2);
}
