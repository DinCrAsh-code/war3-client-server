//============================================================================
//  Two different kinds of "default" body for CSimpleFrame's own contribution
//  to the CLayoutFrame vtable chain (frame.h's own vtable block comment on
//  CSimpleFrame explains the split):
//
//   * a genuine shared default - ten slots (8, 9, 11-13, 16-19, 22) that this
//     session's `bytes` read of every one confirmed are the same tiny
//     "return a constant, touch nothing" body regardless of which
//     CLayoutFrame-derived class asks (0x6F30C2xx, one shared module used
//     by CSimpleButton and, almost certainly, most of its siblings too -
//     nothing in this batch's closure reaches any of these addresses from
//     anywhere else, so "shared" is this session's inference from the
//     address range, not a second confirmed call site);
//   * a base-case placeholder - seven slots (2, 20, 21, 23, 24, 27, 28) this
//     session only ever saw CSimpleButton's own address for.  CSimpleButton
//     overrides every one of them (Frame/simplebuttonvtable*.cpp); a plain
//     CSimpleFrame (or any other sibling not yet reconstructed) still needs
//     *some* body at each slot, and the only body count of it than "the
//     shipped, unhooked vtable already carries the right one - dispatch
//     through it" is vtableboundary.cpp's own convention, extended here
//     past slot 0 to each of these.
//============================================================================
#include "frame.h"
#include "framethunks.h"

namespace {

template <typename Fn>
inline Fn SlotFn(void* self, unsigned int byteOffset)
{
    return (Fn)(*(void***)self)[byteOffset / 4];
}

}  // namespace

//----------------------------------------------------------------------------
//  The ten shared defaults, 0x6F30C210..0x6F30C2C0 - `bytes` read, not
//  guessed:  8 (no args, `return 0`), 9 (two stack args, `return 1`),
//  11/12 (true no-ops), 13 (no-op that still eats one stack arg), 16-19/22
//  (one stack arg, `return 0`).
//----------------------------------------------------------------------------
int  CSimpleFrame::Method_0x20()             { return 0; }
int  CSimpleFrame::Method_0x24(int, int)     { return 1; }
void CSimpleFrame::Method_0x2C()             {}
void CSimpleFrame::Method_0x30()             {}
void CSimpleFrame::Method_0x34(int)          {}
int  CSimpleFrame::Method_0x40(int)          { return 0; }
int  CSimpleFrame::Method_0x44(int)          { return 0; }
int  CSimpleFrame::Method_0x48(int)          { return 0; }
int  CSimpleFrame::Method_0x4C(int)          { return 0; }
int  CSimpleFrame::Method_0x58(int)          { return 0; }

//----------------------------------------------------------------------------
//  The seven CSimpleButton-only base-case placeholders.
//----------------------------------------------------------------------------
void* CSimpleFrame::Destroy2(int deleteFlags)
{
    typedef void* (__thiscall *Fn)(void*, int);
    return SlotFn<Fn>(this, 0x08)(this, deleteFlags);
}

int CSimpleFrame::Method_0x50(const void* msg)
{
    typedef int (__thiscall *Fn)(void*, const void*);
    return SlotFn<Fn>(this, 0x50)(this, msg);
}

int CSimpleFrame::Method_0x54(const void* msg)
{
    typedef int (__thiscall *Fn)(void*, const void*);
    return SlotFn<Fn>(this, 0x54)(this, msg);
}

void CSimpleFrame::NotifyCaptureGained()
{
    typedef void (__thiscall *Fn)(void*);
    SlotFn<Fn>(this, 0x5C)(this);
}

void CSimpleFrame::NotifyCaptureLost()
{
    typedef void (__thiscall *Fn)(void*);
    SlotFn<Fn>(this, 0x60)(this);
}

void CSimpleFrame::Method_0x6Cb(int id)
{
    typedef void (__thiscall *Fn)(void*, int);
    SlotFn<Fn>(this, 0x6C)(this, id);
}

void CSimpleFrame::NotifyEnabledChanged(int a, int b)
{
    typedef void (__thiscall *Fn)(void*, int, int);
    SlotFn<Fn>(this, 0x70)(this, a, b);
}
