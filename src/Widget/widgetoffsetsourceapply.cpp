//============================================================================
//  0x6F30A350 - one of the CWidget::NegateAndForwardOffset closure's leaves.
//
//  Whole-program register convention: eax = a source record (two floats at
//  +0x44/+0x48), edx = a destination record (a flags dword at +0x0, two
//  floats at +0x4/+0x8) - no `this` in ecx, no stack argument at all.
//  Nothing above naked asm reaches "both operands in registers with no ecx"
//  - the same class of thing CPathSearch::EnqueueNode and the 0x6F4E8xxx
//  animation-track module already are (docs/targets/CSelectable_downstream_
//  closure.md's own note on that module: "arguments in eax and edx ...
//  whole-program register conventions no MSVC spelling reaches").
//
//  If the destination's flags bit 0 is already set, or either source float
//  compares equal to 0.0f, it does nothing.  Otherwise it copies the two
//  source floats across - SWAPPED (dst+4 gets src+0x48, dst+8 gets
//  src+0x44) - and sets the flag.
//
//  The zero-compare is `fldz` / `fucom` / `fnstsw ax` / `test ah,44h` /
//  `jnp` - a mask docs/msvc-vc8-idioms.md's own two-operand `fcompp` table
//  does not carry, because this is the single-operand "compare against a
//  literal zero" spelling instead.  C3 (equal) and C2 (unordered) are the
//  two bits `0x44` masks; `test`'s parity flag is 0 only when exactly one
//  of them is set, which happens only for C3 alone (ST0 == 0.0f exactly),
//  so `jnp` branches on equality and falls through on "not equal, NaN
//  included" - the same NaN-safe `!=` semantics C gives a float compared to
//  a literal, expressed against zero rather than against a second operand.
//
//  A `__declspec(naked)` body is not eligible for the register/scheduling
//  excuses CLAUDE.md's behaviour-verdict section lists - MSVC does no
//  allocation or scheduling inside `__asm` - so this is transcribed
//  instruction for instruction and should be EXACT by construction.
//============================================================================
__declspec(naked) void ApplyWidgetOffsetSourceIfNonzero()
{
    __asm
    {
        sub     esp, 8
        fld     dword ptr [eax+44h]
        mov     ecx, [edx]
        test    cl, 1
        fstp    dword ptr [esp]
        fld     dword ptr [eax+48h]
        fstp    dword ptr [esp+4]
        jnz     short skip_all

        fldz
        fld     st(0)
        fld     dword ptr [esp]
        fucom   st(1)
        fnstsw  ax
        fstp    st(1)
        test    ah, 44h
        jnp     short cleanup_exit

        fld     dword ptr [esp+4]
        fucom   st(2)
        fnstsw  ax
        fstp    st(2)
        test    ah, 44h
        jnp     short cleanup_exit

        fstp    dword ptr [edx+4]
        or      ecx, 1
        mov     [edx], ecx
        fstp    dword ptr [edx+8]
        add     esp, 8
        retn

    cleanup_exit:
        fstp    st(1)
        fstp    st(0)
    skip_all:
        add     esp, 8
        retn
    }
}
