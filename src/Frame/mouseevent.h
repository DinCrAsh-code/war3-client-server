#ifndef FRAME_MOUSEEVENT_H
#define FRAME_MOUSEEVENT_H

#include "../Agent/refcnt.h"

//----------------------------------------------------------------------------
//  CMouseEvent - the stack-built message CSimpleTop's mouse handlers hand to
//  a frame's HitTestMessage (slot 14, +0x38).
//
//  Only the shape the capture handler needs is named here; the class itself is
//  not reconstructed and its constructor stays a thunk, per the standing
//  "recover the call, stub the callee" rule.
//
//  Provenance, all from 0x6F2F5920 (the constructor, 38 instructions, in
//  asm/sub_6F609B50_0x6F609B50_calltree_asm.md:775):
//
//      mov  [esi], offset ??_7CMouseEvent@@6B@   ; +0x00, its own vftable
//      mov  [esi+4], eax        (eax = 0)        ; +0x04, TRefCnt::m_refcount
//      mov  dword ptr [esi+8], 0FFFFFFFFh        ; +0x08
//      mov  [esi+0Ch], eax                       ; +0x0C
//      call sub_6F2F0EB0                         ; ecx = this, one stack arg
//
//  The base is TRefCnt: the constructor's unwind funclet tail-jumps to
//  0x6F0202E0, one of TRefCnt::~TRefCnt's two addresses.  TRefCnt is
//  {vftable, int m_refcount} = 8 bytes, so this class's own two dwords land at
//  +0x08 and +0x0C and sizeof is 0x10 - which is what makes the 0x3C frame in
//  FrameShortcutHandler_6F60B410 add up.
//
//  m_08 is a message-type word: the constructor writes -1 and the capture
//  handler immediately overwrites it with 0x400500CA before hit-testing.
//----------------------------------------------------------------------------
class CMouseEvent : public TRefCnt
{
public:
    //  0x6F2F5920 - thunked, not reconstructed.  __thiscall, `retn 4`.
    CMouseEvent(const void* raw);

    int m_08;   // +0x08  message type; ctor writes -1
    int m_0C;   // +0x0C
};

#endif
