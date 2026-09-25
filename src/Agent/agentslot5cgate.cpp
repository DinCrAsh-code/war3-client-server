//============================================================================
//  0x6F4A6970 - the rendering-side gate CAgent's vtable slot 23 hands its
//  resolved handle object to (agent_slot5c.cpp).
//
//  Was a `mov eax,<address> / jmp eax` redirect described as "out of scope
//  (it builds a CTimeSync channel and a path-trace record)".  Nineteen
//  instructions, and it does not build a channel - it *asks* for one, with
//  CTimeSync::GetChannel, which has been reconstructed here since the first
//  target.
//
//  The whole body is a lazy cache: if the object's +0x20 is already filled
//  in, do nothing; otherwise get its sync channel and hand the channel, the
//  object, a zeroed local and the shared CFloat zero to the builder at
//  0x6F4A66E0, and remember what comes back.
//
//  Reading the shipped push sequence is the only subtle part.  Four pushes
//  stand between the two calls, but GetChannel cleans exactly one of them
//  (`retn 4`), so the *last* push is its argument and the first three are
//  CSyncChannel::Register's - which is why they are written in the order
//  they are below and why the local is zeroed before either call runs.
//
//  The `const` comes off GetChannel's answer because Register is not a
//  const member; the shipped code keeps one pointer in eax across both
//  calls and knows nothing about either qualifier.
//
//  Own translation unit: slot 23 reaches this with a real tail jump.
//============================================================================
#include "game.h"
#include "CFloat.h"

//  0x6F4A66E0 is CSyncChannel::Register (game.h), already declared and
//  thunked in agentrelation_thunks.cpp - declared through game.h, not
//  re-thunked here.  A second naked body at the same address is the
//  two-symbols-for-one-function defect
//  docs/notes/verifier-gate-link-failures.md warns about, and this file had
//  one until the merge that brought that declaration in caught it.

//  The one field of the handle object this gate touches.  Declared locally
//  rather than added to game.h's CHandleObject: +0x20 is past the 0x1C-byte
//  shared base (see game.h's SItemHandleObject note), so extending the base
//  would shift every CPathTrace field instead.
struct SGatedHandleObject
{
    char  m_reserved00[0x20];
    void* m_pTraceRecord;   // +0x20
};

void __fastcall AgentSlot5CGate(CHandleObject* obj)
{
    SGatedHandleObject* self = (SGatedHandleObject*)obj;
    if (self->m_pTraceRecord != 0)
        return;

    //  The scratch dword is zeroed before GetChannel runs, not after: the
    //  shipped `mov [esp+18h+var_4],0` sits between the four pushes and the
    //  first call.
    int scratch = 0;

    //  One expression, not a `channel` local: MSVC evaluates a call's
    //  arguments before its object expression, which is what puts the
    //  builder's three pushes *ahead* of the GetChannel call in the shipped
    //  code.  Binding the channel to a local first reverses that and moves
    //  all three - the same ordering AttachmentSetVisible
    //  (attachmentsetvisible.cpp) turns on.
    self->m_pTraceRecord =
        ((CSyncChannel*)g_pTimeSync->GetChannel(obj))
            ->Register(obj, &scratch, &g_CFloatZero);
}
