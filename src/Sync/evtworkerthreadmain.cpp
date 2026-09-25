//============================================================================
//  EvtWorkerThreadMain (0x6F62CF50) - the body of every thread EvtSched
//  launches, and the root of this dump.
//
//  Its own translation unit so OsTcpPump stays a real call.  Declared in
//  evtsched.h, because EvtSchedShutdown (the other dump's root) is what hands
//  its address to _beginthreadex.
//============================================================================
//  ostcp.h first: it is the header that pulls in Winsock 2, and winsock2.h
//  has to get in ahead of the Winsock 1 declarations <windows.h> would
//  otherwise bring with it.
#include "ostcp.h"

#include "event.h"
#include "evtsched.h"

//----------------------------------------------------------------------------
//  0x6F62CF50 - retn 4, `unsigned int __stdcall(void*)`, the shape
//  _beginthreadex requires.
//
//  The argument is EvtSchedShutdown's shared manual-reset Event.  Wait(0) is a
//  poll, so the loop reads "while the shutdown event has *not* been signalled,
//  spend 100ms pumping the network" - a signalled event returns
//  WAIT_OBJECT_0, which is zero, and every other result (WAIT_TIMEOUT above
//  all) is non-zero and keeps the loop going.
//
//  So the whole of EvtSched's worker pool is Storm's Winsock pump: this is the
//  only thing in the game that ever calls OsTcpPump.
//----------------------------------------------------------------------------
unsigned int __stdcall EvtWorkerThreadMain(void* arg)
{
    Event* shutdownEvent = (Event*)arg;

    while (shutdownEvent->Wait(0))
        OsTcpPump(100);

    return 0;
}
