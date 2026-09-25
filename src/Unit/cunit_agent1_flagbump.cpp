//============================================================================
//  0x6F26E710 - a depth-1/2 CUnit-vtable-BFS leaf (cunit_agent1_worklist.md)
//  that just forwards `this` into two much larger tally/bit-tracking
//  members (0x6F0768E0, `retn 4`; 0x6F0767F0, `retn 10h`) with fixed
//  arguments.  Neither callee's own class is established anywhere in this
//  call tree - both dig into a `+0x1B8..+0x1E0` bitfield/counter block this
//  dump gives no further name to - so both are kept as quick-confirmed
//  naked redirects to their real, unhooked bodies (the same "this batch's
//  own brief" reasoning src/Unit/unit_blockedclosureleaves.cpp already
//  documents for CUnit's permanent-blocker slots), rather than guessed at
//  as struct members.  0x6F26E710 itself is a real transcription: two
//  calls with fixed literal arguments, nothing else.
//============================================================================
#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  A single throwaway receiver: both callees take `this` in ecx and only
//  plain stack ints, so one thiscall declaration each is enough without
//  claiming a real class name this dump does not establish.
struct SCUnitTallyBlock
{
    void SetTallyFlag(int flag);                          // 0x6F0768E0, `retn 4`
    void BumpTally(int a, int b, int c, int d);            // 0x6F0767F0, `retn 10h`
};

ADDR_THUNK(void SCUnitTallyBlock::SetTallyFlag(int flag), 0x6F0768E0)
ADDR_THUNK(void SCUnitTallyBlock::BumpTally(int a, int b, int c, int d), 0x6F0767F0)

#undef ADDR_THUNK

struct SCUnitTallyLeaf { void __thiscall FlagBumpAndTally(); };

//  0x6F26E710
void __thiscall SCUnitTallyLeaf::FlagBumpAndTally()
{
    ((SCUnitTallyBlock*)this)->SetTallyFlag(1);
    ((SCUnitTallyBlock*)this)->BumpTally(1, 1, 0, 0);
}
