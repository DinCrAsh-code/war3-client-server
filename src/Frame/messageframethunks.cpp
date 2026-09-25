//============================================================================
//  Naked redirects for the message frame's callees this batch declares and
//  calls but does not reconstruct.  None is a reconstruction and none is
//  ever hookable.
//
//  0x6F6067F0 and 0x6F622350 used to be here too and are not: both already
//  have real bodies in this repo (CLayoutFrame::SetAllPoints and
//  SDirtyFlagOwner::OrIndirectField16C), so a redirect would have sent
//  every call inside the `.mix` back out to the unhooked image.
//
//  Every argument count is the shipped call site's own push count:
//
//      0x6F623ED0  ecx = owner, edx = 0, one push (500)     -> retn 4
//      0x6F6C5CB0  ecx = path, edx = buffer, one push     -> retn 4
//      0x6F5FB960  ecx = frame, three pushes              -> retn 0Ch
//      0x6F622670  ecx = list, three pushes               -> retn 0Ch
//============================================================================
#include "messageframe.h"

struct SFrameFontHost  { void SetFont(const char* path, float size, int flags); };

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

ADDR_THUNK(CMessageFrame* __fastcall MakeMessageFrame(void*, void*, int), 0x6F623ED0)
ADDR_THUNK(char* __fastcall CopyPathString(const char*, char*, int), 0x6F6C5CB0)
ADDR_THUNK(void SFrameFontHost::SetFont(const char*, float, int), 0x6F5FB960)
ADDR_THUNK(void CMessageFrame::InsertAfter(TEXTMSG*, int, TEXTMSG*), 0x6F622670)

#undef ADDR_THUNK
