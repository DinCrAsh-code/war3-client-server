//============================================================================
//  CFrame's own slots 15, 17-24 (+0x3C, +0x44..+0x60) - a real, shared
//  family of eight forwarders that all tail-dispatch this object's own
//  slot 4 (Method_0x10).  Slot 17 is the only member with a real body of
//  its own: it redirects through a global first (dword_6FACE684 -
//  g_frameFocusRedirect) and only forwards *that* object's own slot 4 when
//  nothing is redirected.
//
//  Eight bodily-identical addresses rather than one shared function because
//  that is what the shipped binary has - every one of 15, 18-24 is its own
//  `mov eax,[ecx] / mov eax,[eax+10h] / jmp eax` at its own address, and
//  CSimpleTop separately carries two more copies of the same three
//  instructions at its own slots 14/16 (Frame/simpletopvtable.cpp) - eleven
//  addresses in this one closure, all the same three instructions.
//============================================================================
#include "frame.h"

extern CFrame* g_frameFocusRedirect;   // dword_6FACE684

int CFrame::Method_0x3C(SAgentMessage* msg) { return Method_0x10(msg); }
int CFrame::Method_0x48(SAgentMessage* msg) { return Method_0x10(msg); }
int CFrame::Method_0x4C(SAgentMessage* msg) { return Method_0x10(msg); }
int CFrame::Method_0x50(SAgentMessage* msg) { return Method_0x10(msg); }
int CFrame::Method_0x54(SAgentMessage* msg) { return Method_0x10(msg); }
int CFrame::Method_0x58(SAgentMessage* msg) { return Method_0x10(msg); }
int CFrame::Method_0x5C(SAgentMessage* msg) { return Method_0x10(msg); }
int CFrame::Method_0x60(SAgentMessage* msg) { return Method_0x10(msg); }

int CFrame::Method_0x44(SAgentMessage* msg)
{
    CFrame* target = g_frameFocusRedirect ? g_frameFocusRedirect : this;
    return target->Method_0x10(msg);
}
