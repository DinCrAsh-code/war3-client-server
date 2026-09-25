//============================================================================
//  0x6F39F2C0 - the player-table submit CWidget::SubmitToPlayerTable calls.
//
//  Own translation unit: it calls 0x6F38B5C0 for real.
//
//  Bit 0 of `flags` means "skip": the shipped code tests it with
//  `mov eax,1 / test [flags], al`, which is a byte test against the
//  constant 1 already sitting in eax for the shift below - the same
//  register serving both purposes is why the constant is materialised
//  before the test rather than being an immediate.
//
//  `playerIndex` is turned into a one-bit mask (`1 << index`) on the way
//  through, so the callee takes a player *mask* where this one takes an
//  index.
//============================================================================
#include "game.h"
#include "widgetregistersubmit.h"

//  0x6F38B5C0 - below the depth cut; thunked.  A thiscall member: the table
//  in ecx and FOUR stack arguments (mask, pos, a, b), cleaning 16 bytes
//  (`retn 10h`).  It must NOT be spelled `__fastcall`: that passes the mask
//  in edx and pushes only three stack arguments, where the shipped callee
//  reads the mask off the stack as its first argument and the position as
//  its second - so a __fastcall spelling shifts every argument down one slot
//  (a plain int lands where the CWorldVec3* position belongs) and
//  under-pushes four bytes the callee then cleans off this frame.  Live,
//  that fed a bogus position into the deep CFloat math under 0x6F192620 and
//  faulted it reading through `pos + 4` (a null+4 access violation the first
//  time a widget was submitted to the player table on selection).  Same
//  struct-member redirect idiom as SWorldObjectSingleton::PublishWidgetPosition
//  (widget_slot64.cpp), and for the same `ret`-count reason.
//  Reconstructed in playertablesubmit.cpp; declared, not defined, here so
//  this call site reaches it with a real call.  The fourth argument is an
//  out-pointer, not the `int` this file declared before the body was read -
//  same four dwords, so the `retn 10h` above is unaffected.
struct SPlayerTableSubmit
{
    int SubmitMasked(unsigned int playerMask, CWorldVec3* pos, int resultMask,
                     int* out);
};

//  `int`, and `return 1` on the skip path: the shipped body opens
//  `mov eax, 1` and uses that same 1 both as the operand of the bit-0 test
//  and as whatever is left in eax when the test skips the call, then
//  overwrites it with `shl eax, cl` on the other path.  Declaring it void
//  said the value was never used, which is true of this function but not
//  of the *slot* that reaches it: CUnit's own override of vtable slot 63
//  (unit_playertable.cpp) computes a real 0/1 answer, so the slot is int
//  and the chain from it down has to be able to carry one.
int SPlayerTable::SubmitWidget(unsigned int playerIndex, CWorldVec3* pos,
                               int flags, int a, int b)
{
    if ((flags & 1) != 0)
        return 1;
    //  `b` arrives as an int through vtable slot 63's own signature and
    //  is an out-pointer by the time it gets there; the cast is the seam.
    return ((SPlayerTableSubmit*)this)->SubmitMasked(1u << playerIndex, pos,
                                                     a, (int*)(size_t)b);
}
