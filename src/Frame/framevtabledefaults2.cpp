//============================================================================
//  Two different kinds of "default" body for CFrame's own contribution to
//  its 57-slot vtable (docs/targets/vtables/CSimpleTop.txt, cross-checked
//  address-for-address against a fresh CWorldFrameWar3 pull) - the same
//  split simpleframevtabledefaults.cpp already documents for the sibling
//  CLayoutFrame-rooted hierarchy:
//
//   * a genuine shared default - eighteen slots (28-30, 36-44, 48-50, 55,
//     and 43) that a fresh `ida_query` pull confirmed are the same tiny
//     "true no-op" or "return a constant, touch nothing" body on *both*
//     leaf classes this session cross-checked;
//   * a base-case placeholder - nine slots (10, 11, 12, 14, 16, 27, 34, 35,
//     45) where CSimpleTop and CWorldFrameWar3 disagree, so CFrame's own
//     un-overridden body is not evidenced anywhere in this closure (no
//     plain, non-further-derived CFrame is ever constructed).  Each
//     dispatches through whatever the real, shipped vtable holds, the same
//     placeholder convention vtableboundary.cpp and
//     simpleframevtabledefaults.cpp already use.  CSimpleTop overrides all
//     nine with its own real bodies (Frame/simpletopvtable.cpp and
//     Frame/simpletoprenderobjects.cpp).
//============================================================================
#include "frame.h"

namespace {

template <typename Fn>
inline Fn SlotFn(void* self, unsigned int byteOffset)
{
    return (Fn)(*(void***)self)[byteOffset / 4];
}

}  // namespace

//----------------------------------------------------------------------------
//  The eighteen shared defaults - `bytes`-confirmed, not guessed.
//----------------------------------------------------------------------------
void CFrame::Method_0x70() {}
void CFrame::Method_0x74() {}
void CFrame::Method_0x78() {}
void CFrame::Method_0x90() {}
void CFrame::Method_0x94() {}
void CFrame::Method_0x98() {}
void CFrame::Method_0x9C() {}
void CFrame::Method_0xA0() {}
void CFrame::Method_0xA4() {}
void CFrame::Method_0xA8() {}
void CFrame::Method_0xB0() {}

int CFrame::Method_0xAC(int, int) { return 1; }
int CFrame::Method_0xC0(int)      { return 0; }
int CFrame::Method_0xC4()         { return 0; }
int CFrame::Method_0xC8()         { return 0; }
int CFrame::Method_0xDC(int, int) { return 1; }

//----------------------------------------------------------------------------
//  The nine disagreement slots - base-case placeholders only.
//----------------------------------------------------------------------------
int CFrame::Method_0x28(int unused) const
{
    typedef int (__thiscall *Fn)(const void*, int);
    return SlotFn<Fn>((void*)this, 0x28)(this, unused);
}

int CFrame::Method_0x2C(void* a, float b, float c)
{
    typedef int (__thiscall *Fn)(void*, void*, float, float);
    return SlotFn<Fn>(this, 0x2C)(this, a, b, c);
}

void CFrame::Method_0x30()
{
    typedef void (__thiscall *Fn)(void*);
    SlotFn<Fn>(this, 0x30)(this);
}

int CFrame::Method_0x38(SAgentMessage* msg)
{
    typedef int (__thiscall *Fn)(void*, SAgentMessage*);
    return SlotFn<Fn>(this, 0x38)(this, msg);
}

int CFrame::Method_0x40(SAgentMessage* msg)
{
    typedef int (__thiscall *Fn)(void*, SAgentMessage*);
    return SlotFn<Fn>(this, 0x40)(this, msg);
}

void CFrame::Method_0x6C()
{
    typedef void (__thiscall *Fn)(void*);
    SlotFn<Fn>(this, 0x6C)(this);
}

void CFrame::Method_0x88()
{
    typedef void (__thiscall *Fn)(void*);
    SlotFn<Fn>(this, 0x88)(this);
}

void CFrame::Method_0x8C()
{
    typedef void (__thiscall *Fn)(void*);
    SlotFn<Fn>(this, 0x8C)(this);
}

void CFrame::MarkAllDirty()
{
    typedef void (__thiscall *Fn)(void*);
    SlotFn<Fn>(this, 0xB4)(this);
}
