//============================================================================
//  0x6F60A600 - the lazily created message frame.
//
//  The host object (g_unk6FACE754, Frame/framethunks.cpp's own
//  ReadUnk6FACE754 hands it back) keeps one CMessageFrame at +0x168 and
//  builds it on first use: make it, take a reference, drop whatever was
//  there, anchor it against the host's own +0xB4 point, set one flag word,
//  and give it FRIZQT__.TTF at the size flt_6F93D01C.
//
//  The release of the *old* pointer sits between the new one being made and
//  it being stored, which is why the reference is taken before the drop -
//  the two would otherwise be the same object on a re-entry.
//
//  The shipped prologue carries a /GS buffer cookie for the 0x104-byte path
//  buffer; this repo builds at a fixed /GS- and cannot emit it.
//============================================================================
#include "messageframe.h"
#include "frame.h"

//  Two of the four calls the creation path makes already have real bodies
//  in this repo and are called as themselves, not redirected: 0x6F6067F0 is
//  CLayoutFrame::SetAllPoints (Frame/framesetpoint.cpp) and 0x6F622350 is
//  SDirtyFlagOwner::OrIndirectField16C (Misc/sweep_field_setters.cpp).  A
//  second naked thunk to either address would compile, link and score the
//  same, and inside the `.mix` every call here would leave for the
//  unhooked shipped code instead of reaching the reconstruction.
//  SDirtyFlagOwner is re-declared per translation unit, which is how
//  Frame/controlframe.cpp and Frame/screenframe.cpp already reach it.
struct SDirtyFlagOwner
{
    void __thiscall OrIndirectField16C(int bits);      // 0x6F622350
};

//  The other two are naked redirects in Frame/messageframethunks.cpp.
char* __fastcall CopyPathString(const char* path, char* out, int size); // 0x6F6C5CB0
struct SFrameFontHost  { void SetFont(const char* path, float size, int flags); };
                                                                    // 0x6F5FB960

//  flt_6F93D01C - the font size.  A real read-only constant in the image,
//  unlike the CFloat globals: it is an ordinary hardware float.
extern const float g_sysMessageFontSize;

typedef void (__thiscall *MessageFrameReleaseFn)(void* self);

struct SMessageFrameHost
{
    //  0x6F60A600 - `retn 0`, `this` in ecx.
    CMessageFrame* EnsureMessageFrame();

    char           m_reserved000[0xB4];
    char           m_anchor0B4[0x168 - 0xB4];   // +0xB4 - the anchor point
    CMessageFrame* m_pMessageFrame;             // +0x168
};

CMessageFrame* SMessageFrameHost::EnsureMessageFrame()
{
    if (m_pMessageFrame == 0)
    {
        CMessageFrame* frame = MakeMessageFrame(this, 0, 0x1F4);
        if (frame != 0)
            (*(int*)((char*)frame + 4))++;

        CMessageFrame* old = m_pMessageFrame;
        if (old != 0)
        {
            if (--(*(int*)((char*)old + 4)) == 0)
                ((MessageFrameReleaseFn)(*(void***)old)[0])(old);
        }

        m_pMessageFrame = frame;
        ((CLayoutFrame*)((char*)frame + 0xB4))
            ->SetAllPoints((CLayoutFrame*)&m_anchor0B4[0], 1);

        //  Read back through the member rather than reusing `frame`: the
        //  shipped code reloads [esi+168h] before each of the three
        //  follow-up calls.
        int* flags = (int*)((char*)m_pMessageFrame + 0x0C);
        *flags |= *(int*)((char*)m_pMessageFrame + 0x10) | 2;

        ((SDirtyFlagOwner*)m_pMessageFrame)->OrIndirectField16C(8);

        char path[0x104];
        CopyPathString("FRIZQT__.TTF", path, sizeof(path));
        ((SFrameFontHost*)m_pMessageFrame)
            ->SetFont(path, g_sysMessageFontSize, 0);
    }

    return m_pMessageFrame;
}
