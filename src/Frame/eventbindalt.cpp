//============================================================================
//  0x6F614590 - CLayer::SetAltEventListener: the same six instructions
//  as SetEventListener (eventbind.cpp) against event table 0x400B0064.
//
//  Its own translation unit because it is its own shipped module: 0x6F601840
//  and this sit either side of the whole button, message-frame, layout and
//  frame libraries.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CLayer::SetAltEventListener(int event, void* listener, int enable)
{
    if (enable && listener)
        Dispatch(0x400B0064, (unsigned int)event, listener);
    else
        PostEvent(0x400B0064, listener);
}
