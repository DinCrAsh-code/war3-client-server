//============================================================================
//  0x6F60A7B0 - CScreenFrame::CScreenFrame: a CFrame built with (0, 0, 0),
//  four counted-reference slots and mode 2.
//
//  OrField128And124 is Misc/sweep_field_setters.cpp's name for 0x6F606EA0,
//  already reconstructed there; it is re-declared against a local copy of its
//  receiver, which mangles identically because MSVC puts only the class
//  *name* in a member's mangled name.
//
//  Score capped by the unified EH frame; see framecore.cpp.
//============================================================================
#include "frame.h"
#include "framethunks.h"


//  0x6F606EA0 - Misc/sweep_field_setters.cpp.
struct SDirtyFlagOwner
{
    void __thiscall OrField128And124(int bits);
};

CScreenFrame::CScreenFrame()
    : CFrame(0, 0, 0)
{
    m_168 = 0;
    m_pRenderObject = 0;

    m_flagMask |= 1;
    m_flags |= 1;

    ((SDirtyFlagOwner*)this)->OrField128And124(2);
    ReleaseRefs();
}
