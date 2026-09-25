//============================================================================
//  g_jassFramePool (unk_6FAB728C) - the JASS VM's own global pool of
//  reusable call-frame blocks (each "AUFrame"-tagged, carrying an
//  SScriptDataTable at +0x90).  First reached by jassmoduleteardown.cpp
//  (jassvm-core-A, teardown-only: unlink-and-free every live frame) as a
//  CDataAllocator with one extra trailing field, +0x14 - a TSLink-style
//  list head threading every live frame.  This session's own
//  jassscriptframepool.cpp reaches one field further, +0x18, from the
//  allocate side (AcquireScriptFrame, 0x6F45C0D0) - moved out to this
//  shared header so both files declare the identical type for the one
//  global rather than two TUs guessing at overlapping-but-different
//  struct shapes for the same shipped object.
//============================================================================
#ifndef JASSFRAMEPOOL_H
#define JASSFRAMEPOOL_H

#include "dataallocator.h"

struct JassFramePool : public CDataAllocator
{
    void*        m_frameListHead;   // +0x14 - TSLink-style list head
                                     //   threading every live frame
                                     //   (jassmoduleteardown.cpp's own
                                     //   JassFrameNode::Unlink, 0x6F452EB0)
    unsigned int m_liveFrameCount;  // +0x18 - bumped by AcquireScriptFrame

    //  0x6F45C0D0 - `retn 4` (__thiscall: `this` in ecx, `zeroMemory` the
    //  one stack argument) - jassscriptframepool.cpp.  Declared as a
    //  member rather than a free `__fastcall` function because the real
    //  call site pushes `zeroMemory` on the stack instead of putting it
    //  in edx; a free __fastcall(pool, zeroMemory) would put it in edx
    //  instead and clean 0 stack bytes where the shipped body cleans 4 -
    //  exactly the ABI mismatch tools/abi_audit.py caught on this
    //  session's first draft.
    void* AcquireFrame(int zeroMemory);

    //  0x6F45AD70 - `retn 4` (__thiscall: `this` in ecx, `frame` the one
    //  stack argument).  jassscriptframereturn.cpp.  Tears the frame's own
    //  contents down through ReleaseScriptFrameContents (jassscriptframepool.h,
    //  0x6F45ACE0) and then does what that function's own comment says its
    //  caller must: thread the frame's own link word (`frame - 4`, the same
    //  4-byte offset AcquireFrame hands back past) onto this pool's own
    //  m_frameListHead, LIFO.
    void ReleaseFrame(struct JassScriptFrame* frame);
};

extern JassFramePool g_jassFramePool;   // unk_6FAB728C

#endif
