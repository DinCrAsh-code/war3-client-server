//============================================================================
//  0x6F41F1C0 (`retn 8`) - set or clear one bit of this object's own +0x2CC
//  flags word, where the bit index is looked up from a candidate object
//  handed in.  `this` in ecx, two stack arguments: the candidate pointer
//  and a bool ("set" when non-zero, "clear" when zero).
//
//  Three out-of-scope callees, none reconstructed anywhere in this tree
//  yet - each declared and self-redirect-thunked (retn size read off its
//  own `worktree_store.py show`, the real function's own tail rather than
//  the shared release-helper snippet every dump appends after it):
//    - 0x6F40F890 (`retn 4`) - a method of *this* object (the flag owner,
//      not the candidate), taking the candidate as its one stack argument,
//      and resolving it to a bit index - confirmed from the call site:
//      `push edi(candidate); mov esi,ecx(this)` happens *before*
//      `call sub_6F40F890`, and ecx is never reloaded from edi in between,
//      so the call still runs with ecx==this;
//    - 0x6F41C3C0 (`retn 4`) - a second method of *this*, same (this,
//      candidate) shape, unconditional side effect;
//    - 0x6F2970B0 (`retn 0`) - the one call that really does run with
//      ecx==candidate (`mov ecx,edi` appears right before it) - a side
//      effect on the candidate alone, no stack argument.
//
//  This TU declares its own minimal struct rather than including unit.h:
//  whether this is really CUnit (whose own +0x2CC is already named
//  m_float2CC, a float, in unit.h) is not established by anything in this
//  call tree - the shipped code here treats the same four bytes as a
//  32-bit bitmask, which cannot both be true of one field's one C++ type,
//  so rather than guess which reading is "the" field this stays its own,
//  independently-offset local struct (this repo's usual "redeclare rather
//  than share, when a shared header would force a claim this dump does
//  not support" convention).
//============================================================================

struct SFlagBitIndexCandidate
{
    void __thiscall NotifySideEffect_6F2970B0();      //  0x6F2970B0
};

struct SFlagBit2CCOwner
{
    int  __thiscall ResolveBitIndex_6F40F890(SFlagBitIndexCandidate* candidate);    //  0x6F40F890
    void __thiscall UnusedSideEffect_6F41C3C0(SFlagBitIndexCandidate* candidate);   //  0x6F41C3C0
    void __thiscall SetFlagBitByLookup_6F41F1C0(SFlagBitIndexCandidate* candidate, int enable);  //  0x6F41F1C0

    unsigned int m_reserved00[0x2CC / 4];
    unsigned int m_flags2CC;   // +0x2CC
};

void __thiscall SFlagBit2CCOwner::SetFlagBitByLookup_6F41F1C0(SFlagBitIndexCandidate* candidate, int enable)
{
    int bitIndex = ResolveBitIndex_6F40F890(candidate);
    UnusedSideEffect_6F41C3C0(candidate);
    candidate->NotifySideEffect_6F2970B0();

    if (enable)
        m_flags2CC |= (1 << bitIndex);
    else
        m_flags2CC &= ~(1 << bitIndex);
}

__declspec(naked) int __thiscall SFlagBit2CCOwner::ResolveBitIndex_6F40F890(SFlagBitIndexCandidate*)
{
    __asm { mov eax, 06F40F890h }
    __asm { jmp eax }
}

__declspec(naked) void __thiscall SFlagBit2CCOwner::UnusedSideEffect_6F41C3C0(SFlagBitIndexCandidate*)
{
    __asm { mov eax, 06F41C3C0h }
    __asm { jmp eax }
}

__declspec(naked) void __thiscall SFlagBitIndexCandidate::NotifySideEffect_6F2970B0()
{
    __asm { mov eax, 06F2970B0h }
    __asm { jmp eax }
}
