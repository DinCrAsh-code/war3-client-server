//============================================================================
//  CFrame's own slot 54 (+0xD8) - a real, shared teardown body (same
//  address on a fresh CSimpleTop and CWorldFrameWar3 pull): detach this
//  frame's own subtree (a real but substantial helper, 0x6F601520, left a
//  naked thunk), release the fog/light lists (0x6F5FA790, `__fastcall`
//  taking m_160 in ecx and m_164 in edx - the "receiver in ecx with a
//  second argument in edx" shape docs/msvc-vc8-idioms.md already names),
//  then free m_160 itself (".\\CFrame.cpp":0x26E) if it was ever allocated.
//============================================================================
#include "frame.h"
#include "storm.h"

#define FRAME_TEARDOWN_THUNK(addr)   \
    {                                \
        __asm { mov eax, addr }     \
        __asm { jmp eax }           \
    }

//  0x6F601520 - detach this frame's whole subtree from the root layer list
//  and free every LAYERNODE it owns; real but substantial (its own closure
//  runs to a dozen further callees), left a naked thunk.
__declspec(naked) void CFrame::DetachSubtree(void*) FRAME_TEARDOWN_THUNK(0x6F601520)

//  0x6F5FA790 - release whatever m_160/m_164 name.  `this`'s real class is
//  not established (the receiver comes from CFrame::m_160, which this
//  batch has not otherwise typed), so a free `__fastcall` rather than a
//  guessed member, matching framethunks.h's own ApplyOwnedFrameRect.
void __fastcall ReleaseFrameBlock160(void* a, void* b);
__declspec(naked) void __fastcall ReleaseFrameBlock160(void*, void*) FRAME_TEARDOWN_THUNK(0x6F5FA790)

void CFrame::Method_0xD8(void* arg)
{
    DetachSubtree(arg);
    ReleaseFrameBlock160((void*)m_160, (void*)m_164);

    if (m_160 != 0)
        SMemFree((void*)m_160, ".\\CFrame.cpp", 0x26E, 0);
    m_160 = 0;
}
