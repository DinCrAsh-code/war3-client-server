//============================================================================
//  0x6F2AD680 - CWidget vtable[0xFC] (index 63).
//
//  Ask a registry entry (slot 13) whether flag 0x200 is set on whatever it
//  points at, fold that into the caller's own flag word as bit 0, take the
//  widget's world position from its handle sub-object, and submit all of it
//  to the player-slot table.
//
//  Own translation unit: every call it makes is real.
//
//  The three position components are copied out of the returned vector into
//  three adjacent slots and passed as one block - the same "copy x/y/z into
//  their own contiguous locals" the other CWidget slots that forward a
//  position all do.
//============================================================================
#include "widget.h"
#include "game.h"
#include "widgetregistersubmit.h"
#include "gamecontext.h"

//  dword_6FAB65F4 - the player slot table; see playercolor.h for why this
//  symbol keeps the name itemhandlemain.h gave it.
extern void* g_unk6FAB65F4;

//  0x6F4C34D0 - one slot of this thread's pointer array, already
//  reconstructed as GetThreadLocalSlot (Sync/tlsslot.cpp) and declared in
//  gamecontext.h.  This file used to declare a second name for the same
//  address (`GetRegistryEntry`) backed by its own naked thunk in
//  Misc/unreconstructed_thunks.cpp - two symbols for one function, so this
//  call site reached the shipped code and the reconstruction was never
//  exercised by it.  Both are gone; the real name is used.

typedef CPathRef* (__thiscall *GetHandleObjectFn)(void*);

int CWidget::SubmitToPlayerTable(unsigned int playerIndex, int flags,
                                 int extra)
{
    void* vself = this;
    SPlayerTable* table = (SPlayerTable*)g_unk6FAB65F4;

    //  Registry slot 13, then *two* loads before the null test:
    //  `mov eax,[eax+10h]` reads the pointer parked at the slot's +0x10 and
    //  `mov eax,[eax]` follows it.  Taking the address of +0x10 and loading
    //  once (`add eax,16` / `mov eax,[eax]`) is one indirection short - it
    //  hands the test whatever sits *at* +0x10 rather than what that points
    //  at - and it costs the extra load the shipped stream has.  Neither
    //  pointer is null-checked; only the final subject is.
    void** entry = *(void***)((char*)GetThreadLocalSlot(13) + 0x10);
    void* subject = *entry;

    //  Materialised at a join (`mov edi,1` / `jmp` / `xor edi,edi`), not
    //  accumulated from a pre-zeroed local: the shipped code has both
    //  halves, where `int extraBit = 0; if (...) extraBit = 1;` gives a
    //  single `xor` up front and one `mov` in the taken arm.  Same
    //  distinction widget_slot61.cpp's own comment records.
    int extraBit;
    if (subject != 0 && (*(int*)((char*)subject + 4) & 0x200) != 0)
        extraBit = 1;
    else
        extraBit = 0;

    CWorldVec3 world =
        ((GetHandleObjectFn)(*(void***)vself)[0xB8 / 4])(vself)
            ->SmartPtrToWorldVec3();

    CWorldVec3 pos;
    pos.m_x = world.m_x;
    pos.m_y = world.m_y;
    pos.m_z = world.m_z;

    //  `extraBit != 0`, not `extraBit`: the shipped code re-normalises the
    //  flag to 0/1 at the point of use (`xor edx,edx` / `test edi,edi` /
    //  `setne dl`) before folding it in, which is what an explicit
    //  comparison gives and what handing the already-0-or-1 local straight
    //  to `|` does not.
    return table->SubmitWidget(playerIndex, &pos,
                               (extraBit != 0) | flags, extra, 0);
}
