//============================================================================
//  The `location` JASS natives - 0x6F3C0DD0..0x6F3C0E50, one contiguous run
//  in the registration table, all four resolving their handle through
//  ResolveLocationHandle and then touching the coordinate pair at +0x24/+0x28
//  directly.  See jassnatives.h.
//============================================================================
#include "jassnatives.h"

//----------------------------------------------------------------------------
//  0x6F3C0DD0 - `RemoveLocation` takes "(Hlocation;)V".  The destroy call is
//  a tail jump in the shipped code (`jmp eax` off the vtable, not `call`),
//  which is what MSVC emits for a void function whose last statement is a
//  call with the same argument list - so the dispatch is written as the last
//  statement and nothing follows it.
//
//  Slot 23 (+0x5C) is the same slot agent.h already names Method_0x5C on
//  CAgent.  It is dispatched by hand here rather than through a declared
//  class because nothing in this repo establishes a vtable for whatever a
//  location object really is, and a guessed one would misplace every slot
//  after it.
//----------------------------------------------------------------------------
void __cdecl JASS_RemoveLocation(int hLocation)
{
    SJassLocation* location = ResolveLocationHandle(hLocation);

    if (location)
    {
        typedef void (__thiscall *DestroySelfFn)(void*);
        ((DestroySelfFn)(*(void***)location)[0x5C / 4])(location);
    }
}

//----------------------------------------------------------------------------
//  0x6F3C0DF0 - `MoveLocation` takes "(Hlocation;RR)V": both reals arrive by
//  pointer and are dereferenced into the record.
//----------------------------------------------------------------------------
void __cdecl JASS_MoveLocation(int hLocation, const int* x, const int* y)
{
    SJassLocation* location = ResolveLocationHandle(hLocation);

    if (location)
    {
        //  Both reals are read out before either is stored: the shipped code
        //  loads the two pointers, dereferences both, and only then writes
        //  the pair, which is what naming the values does and what storing
        //  straight through the arguments does not.
        int newY = *y;
        int newX = *x;

        location->m_x = newX;
        location->m_y = newY;
    }
}

//----------------------------------------------------------------------------
//  0x6F3C0E10 / 0x6F3C0E30 - `GetLocationX` / `GetLocationY`, both
//  "(Hlocation;)R".  The result is the real's raw encoding in eax, so these
//  return an int and never touch the FPU; the null path returns 0 without a
//  `xor eax,eax` because the resolver has already left eax at zero.
//----------------------------------------------------------------------------
int __cdecl JASS_GetLocationX(int hLocation)
{
    SJassLocation* location = ResolveLocationHandle(hLocation);

    if (!location)
        return 0;

    return location->m_x;
}

int __cdecl JASS_GetLocationY(int hLocation)
{
    SJassLocation* location = ResolveLocationHandle(hLocation);

    if (!location)
        return 0;

    return location->m_y;
}

//----------------------------------------------------------------------------
//  0x6F3C0E50 - `GetLocationZ`, "(Hlocation;)R".  Unlike its two siblings
//  this one is not a field read: the record stores only x and y, and the z a
//  location reports is the world height at that point, asked for fresh on
//  every call (jasslocationheight.cpp).
//
//  The pair crosses into that query as *hardware* floats, which is the only
//  place in this family a JASS real is ever anything but its raw encoding:
//  each is copied into a stack slot and `fld`ed straight back out, and the
//  answer is copied back the same way.  That is what the two casts below
//  are, and it is why this function reserves eight bytes of frame it never
//  otherwise touches.
//
//  The two reads are named, and `y` first: the shipped code loads +0x28
//  before +0x24 and only then starts building the argument block.
//----------------------------------------------------------------------------
//  0x6F24F3E0 - jasslocationheight.cpp.  Called with -1 and 0 in the two
//  register words and 1 for the trailing mode.
float __fastcall QueryWorldHeightAt(int flag, void* outPtr,
                                    float x, float y, int mode);

int __cdecl JASS_GetLocationZ(int hLocation)
{
    SJassLocation* location = ResolveLocationHandle(hLocation);

    if (!location)
        return 0;

    int rawY = location->m_y;
    int rawX = location->m_x;

    float z = QueryWorldHeightAt(-1, 0,
                                 *(const float*)&rawX, *(const float*)&rawY, 1);

    return *(const int*)&z;
}
