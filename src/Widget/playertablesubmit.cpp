//============================================================================
//  0x6F38B5C0 - SPlayerTableSubmit::SubmitMasked: the player-table submit
//  every widget selection goes through (widgetregistersubmit.cpp).
//
//  Was a `mov eax,<address> / jmp eax` redirect, and one whose *declaration*
//  has already cost the operator one live crash (CLAUDE.md's own thunk-ABI
//  table lists it: declared `__fastcall`, it under-pushed four bytes the
//  shipped `retn 10h` then cleaned).  The arity has been right since that
//  fix; this replaces the redirect with the body.
//
//  Thirty-one instructions in two halves.  The first callee is handed the
//  *address of this function's own second parameter* as its third argument
//  and rewrites it in place, which is why the value is re-read off the
//  stack afterwards rather than kept in a register - `pos` is in/out, not
//  an in-only pointer.  The second callee then gets the pair the first one
//  produced plus the caller's first argument.
//
//  The last four instructions are the branchless `!= 0` this compiler emits
//  for an int-valued comparison (`xor ecx,ecx` / `cmp ecx,eax` / `sbb` /
//  `neg`), not a branch - so the return really is a 0/1 flag and not the
//  masked value itself.
//
//  The fourth parameter is an out-pointer, not the `int b` the redirect
//  declared: the shipped code tests it against zero and stores the raw
//  result through it.  Same four dwords either way, so no ABI changed.
//
//  Own translation unit: SPlayerTable::SubmitWidget reaches this with a real
//  call and it makes two of its own.
//============================================================================
#include "widgetregistersubmit.h"

//----------------------------------------------------------------------------
//  The two halves of the submit, both thiscall members of the sub-object at
//  +0x34 and both `retn 0Ch` - three stack arguments each, read off their
//  own shipped bodies, which is what tools/thunk_abi_audit.py checks these
//  declarations against.  0x6F00E7A0 is a real body now
//  (playertablesubmitparts.cpp), and 0x6F26D0C0 is one too now
//  (playertablesubmitprepare.cpp) - its own remaining callee, 0x6F192620,
//  is the redirect instead.
//----------------------------------------------------------------------------
struct SPlayerTableSubmitParts
{
    //  Both out-pointers are `unsigned short*`: 0x6F26D0C0's own two stores
    //  through them are `mov [ecx], dx` and `mov [ecx], ax`, word-sized.
    //  The second one is handed the address of *this function's own second
    //  parameter*, which is why `pos` is re-read afterwards.
    void __thiscall Prepare(CWorldVec3* pos, unsigned short* outCodeA,
                            unsigned short* outCodeB);
    //  0x6F00E7A0 - playertablesubmitparts.cpp.  Two 16-bit codes and the
    //  player mask; see that file for why they are not the (slot, pos) pair
    //  this declaration used to claim.
    int  __thiscall Submit(unsigned int codeA, unsigned int codeB,
                           unsigned int playerMask);
};

//  The same class widgetregistersubmit.cpp declares at its own call site -
//  one name, one mangled symbol, declared in both places rather than moved
//  to the header, which is what that file already does for it.
struct SPlayerTableSubmit
{
    int SubmitMasked(unsigned int playerMask, CWorldVec3* pos, int resultMask,
                     int* out);
};

struct SPlayerTableSubmitSelf
{
    char                     m_reserved00[0x34];
    SPlayerTableSubmitParts* m_pParts;   // +0x34
};

int SPlayerTableSubmit::SubmitMasked(unsigned int playerMask, CWorldVec3* pos,
                                     int resultMask, int* out)
{
    //  Read once into a local: the shipped code keeps it in esi across both
    //  calls (`mov esi,[ecx+34h]` / `mov ecx,esi` twice) rather than
    //  reloading +0x34 for the second one.
    SPlayerTableSubmitParts* parts = ((SPlayerTableSubmitSelf*)this)->m_pParts;

    //  A dword local whose *low word* Prepare fills: the shipped code reads
    //  the whole slot back with a plain `mov`, not a `movzx`, so the local
    //  is four bytes wide even though only two of them are written.
    unsigned int codeA;
    parts->Prepare(pos, (unsigned short*)&codeA, (unsigned short*)&pos);

    //  `pos` re-read after the call because Prepare wrote a word over it
    //  through the address it was handed - the shipped
    //  `mov ecx,[esp+8+arg_4]` is that reload, not a spill.  The cast is
    //  what makes the slot's new contents readable; nothing dereferences
    //  the pointer after this point.
    int result = parts->Submit(codeA, (unsigned int)(unsigned long)pos,
                               playerMask);

    if (out != 0)
        *out = result;

    //  `0 < (unsigned)x`, not `x != 0`: the shipped tail is
    //  `xor ecx,ecx` / `cmp ecx,eax` / `sbb eax,eax` / `neg eax`, which is
    //  what the ordered comparison against a materialised zero builds.
    //  `!= 0` gives the shorter `neg` / `sbb` / `neg` for the same answer.
    return 0 < (unsigned int)(result & resultMask);
}
