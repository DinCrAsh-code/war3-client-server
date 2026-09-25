//============================================================================
//  CSimpleTop's own slots 34/35 (+0x88/+0x8C) - a real "ensure some host
//  resource, then broadcast through CLayer::m_field38's own vtable" pair.
//  Neither is CSimpleTop::m_pCapturedFrame despite both, several calls
//  deep, touching a +0x16C field on some *other* object - that object is
//  the "system message host" singleton (g_unk6FACE754), whose own +0x16C
//  is CScreenFrame::m_pRenderObject (already named,
//  Frame/screenframerender.cpp), not this class's +0x16C.  See
//  docs/notes/pause-mouse-capture.md's own dated section for the full
//  chase.
//
//  0x6F5FADC0 itself is real now - CFrame::EnsureAndConfigureScreenSprite,
//  Frame/framechildbroadcast.cpp (shared verbatim with CFrame's own slot
//  33).  Two further internal helpers stay naked thunks: 0x6F5FB940 (a
//  CLayer-level "find my nearest owner with m_field2C set" walk) and
//  0x6F5FADE0/0x6F5FF7C0 (a second, per-mode-indexed host resource this
//  session did not pin down).
//============================================================================
#include "frame.h"

#define SIMPLETOP_RENDER_THUNK(addr)   \
    {                                  \
        __asm { mov eax, addr }       \
        __asm { jmp eax }             \
    }

__declspec(naked) int  CSimpleTop::FindOwnerWithPriority() SIMPLETOP_RENDER_THUNK(0x6F5FB940)
__declspec(naked) void CSimpleTop::EnsureHostModeResource(void*) SIMPLETOP_RENDER_THUNK(0x6F5FADE0)
__declspec(naked) void CSimpleTop::EnsureHostModeResourceAlt(void*) SIMPLETOP_RENDER_THUNK(0x6F5FF7C0)

void CSimpleTop::Method_0x88()
{
    EnsureAndConfigureScreenSprite((void*)FindOwnerWithPriority());

    if (m_field38 != 0)
    {
        EnsureHostModeResource(m_field38);

        typedef void (__thiscall *Fn)(void*);
        Fn f = (Fn)(*(void***)m_field38)[0xD4 / 4];
        f(m_field38);
    }
}

void CSimpleTop::Method_0x8C()
{
    EnsureAndConfigureScreenSprite(0);

    if (m_field38 != 0)
    {
        EnsureHostModeResourceAlt(m_field38);

        typedef void (__thiscall *Fn)(void*);
        Fn f = (Fn)(*(void***)m_field38)[0xD0 / 4];
        f(m_field38);
    }
}
