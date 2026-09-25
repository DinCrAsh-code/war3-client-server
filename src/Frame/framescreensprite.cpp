//============================================================================
//  CFrame::EnsureAndConfigureScreenSprite (0x6F5FADC0) - the "system
//  message host" singleton's own screen-space render object, lazily
//  created and reconfigured with `arg`.  `this` is never read (the
//  receiver comes in ecx purely because every call site happens to have
//  one handy).
//
//  Deliberately its own translation unit, separate from both of its own
//  in-repo callers (CFrame's own slot 33, Frame/framechildbroadcast.cpp;
//  CSimpleTop's own slot 34, Frame/simpletoprenderobjects.cpp) - with the
//  body visible in the same TU as a call site, `/Ob2` inlined the whole
//  three-call chain there instead of emitting the real `call` the shipped
//  slot has, the same defect
//  docs/msvc-vc8-idioms.md's "A local helper called from two branches..."
//  entry documents for Frame/frameownertoggle.cpp's own three setters.
//============================================================================
#include "frame.h"

//  0x6F60A030 - Misc/misc_accessors.cpp's own global read, under the exact
//  signature link_check.py requires (see Misc/sysmessagepost.cpp's own
//  comment on why the calling convention and return type must match
//  verbatim).
int __fastcall ReadUnk6FACE754();

void CFrame::EnsureAndConfigureScreenSprite(void* arg)
{
    CScreenFrame* host = (CScreenFrame*)ReadUnk6FACE754();
    if (host != 0)
    {
        void* renderObj = host->EnsureRenderObject(1);
        ((CSpriteFrame*)renderObj)->ConfigureSprite(arg, -1, 0);
    }
}
