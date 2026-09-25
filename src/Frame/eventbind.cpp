//============================================================================
//  0x6F601840 - CLayer::SetEventListener.
//
//  Bind or unbind `listener` for one of this frame's event classes.  The bind
//  arm is a *tail jump* into CObserver's own slot 2 (Agent/observer.h), which
//  is why the shipped stream shuffles the three incoming argument slots in
//  place - it is reusing its own frame for the call it never returns from -
//  and the unbind arm is the same tail into CObserver::PostEvent.
//
//  0x40090064 is the event table this frame's listeners live in.  Its twin at
//  0x6F614590 is the identical six instructions against table 0x400B0064, on
//  a different receiver; what either table *means* is not established by this
//  call tree, so the two names differ by that and nothing more.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CLayer::SetEventListener(int event, void* listener, int enable)
{
    if (enable && listener)
        Dispatch(0x40090064, (unsigned int)event, listener);
    else
        PostEvent(0x40090064, listener);
}
