//============================================================================
//  Two of CFrame's own real, shared vtable slots (docs/targets/vtables/
//  CSimpleTop.txt, cross-checked address-for-address against a fresh
//  CWorldFrameWar3 pull - both share these two verbatim):
//
//   * slot 4 (+0x10) - an override of CObserver::Method_0x10 that stamps
//     `this` into the message's own +0xC field (one dword past what
//     observer.h's SAgentMessage currently names) before tail-forwarding to
//     the base body;
//   * slot 51 (+0xCC) - copies three globals into a 12-byte out-parameter,
//     never touching `this` at all.
//============================================================================
#include "frame.h"

//  dword_6FACE690/694/698 - a contiguous three-word record Method_0xCC
//  copies out whole.  Nothing in this batch's closure writes any of the
//  three, so their real meaning is not established; named by address like
//  the other uncertain globals this session found nearby (dword_6FACE66C
//  family, Frame/frameownertoggle.cpp).
extern int g_frameState690;   // dword_6FACE690
extern int g_frameState694;   // dword_6FACE694
extern int g_frameState698;   // dword_6FACE698

int CFrame::Method_0x10(SAgentMessage* msg)
{
    //  One dword past SAgentMessage's own currently-named extent
    //  (observer.h) - every caller in this closure hands a message struct
    //  big enough to carry it, but the shared struct itself is not grown
    //  here to avoid disturbing its other, already-scored users.
    *(void**)((char*)msg + 0xC) = this;
    return CObserver::Method_0x10(msg);
}

void CFrame::Method_0xCC(void* out)
{
    int* p = (int*)out;
    p[0] = g_frameState690;
    p[1] = g_frameState694;
    p[2] = g_frameState698;
}
