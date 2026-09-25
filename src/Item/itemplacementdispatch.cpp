//============================================================================
//  0x6F4D3170 / 0x6F4D3180 - two tiny, non-virtual member functions whose
//  entire body is "dispatch through my own object's vtable at a fixed
//  slot" - the classic non-virtual-interface shape (a stable, addressable
//  public entry point that forwards to a protected/private virtual the
//  concrete subclass overrides), which is also why neither inlines despite
//  being a couple of instructions: the vtable slot they call through is not
//  resolvable at compile time.
//
//  `this` here is whatever CSelectable::m_reserved28 (selectable.h) points
//  at - JASS_SetItemPosition_callee1 (0x6F2B6AE0) reads that field directly
//  and calls both of these on it.  The class itself is not otherwise
//  identified by this call tree (no other member is reached), so it is
//  dispatched here the same way widget.cpp/observereventreg.cpp already
//  reach an unidentified vtable slot on a known-shape object: a raw
//  `void**` cast rather than inventing six unknown virtuals in front of the
//  two slots this dump actually reaches.
//============================================================================
#include "itemplacementdispatch.h"

typedef void (__thiscall *SetOrientationMatrixFn)(void*, const SOrientationMatrix*);
typedef void (__thiscall *SetPositionFn)(void*, const FVec3*);

//----------------------------------------------------------------------------
//  0x6F4D3170 - vtable slot 0x18 (index 6).  `target` in ecx, the 36-byte
//  matrix passed by value on the stack (too large for edx) - __fastcall,
//  not thiscall, since this is a free function forwarding an interface
//  pointer rather than a genuine member of a known class (see the file
//  header).
//----------------------------------------------------------------------------
void __fastcall SetItemPlacementOrientation(void* target, SOrientationMatrix m)
{
    ((SetOrientationMatrixFn)(*(void***)target)[0x18 / 4])(target, &m);
}

//----------------------------------------------------------------------------
//  0x6F4D3180 - vtable slot 0x1C (index 7).  Same shape, a 12-byte FVec3.
//----------------------------------------------------------------------------
void __fastcall SetItemPlacementPosition(void* target, FVec3 pos)
{
    ((SetPositionFn)(*(void***)target)[0x1C / 4])(target, &pos);
}

//----------------------------------------------------------------------------
//  0x6F4D3360.  A plain field write, not a vtable dispatch: only bits in
//  the top byte are ever touched (the shipped code masks the caller's
//  `mask` down to 0xFF000000 before using it, even though the one known
//  caller's own 0x4000000 is already inside that range).
//----------------------------------------------------------------------------
void __fastcall SetItemPlacementFlagBits(void* target, unsigned int mask, int set)
{
    mask &= 0xFF000000;
    if (set)
        *(unsigned int*)((char*)target + 0x28) |= mask;
    else
        *(unsigned int*)((char*)target + 0x28) &= ~mask;
}
