//============================================================================
//  0x6F30AFB0 - pack a widget's own byte/dword flag word into the flag set
//  Storm's sound-play call expects, reached through
//  CWidget::NegateAndForwardOffset's closure the same way
//  Widget/widgetsoundlabel.cpp's ResolveSoundLabelFlag is: the one call
//  site seen (`mov ecx, [esi+40h]; call sub_6F30AFB0`, in the still
//  out-of-scope enclosing function near 0x6F32F659) feeds the result
//  straight into a multi-argument sound-play call alongside two literal
//  Storm string pointers, matching a "play this named sound with these
//  flags" shape. `retn 0`, __fastcall(ecx=flags), no stack arguments.
//============================================================================

//----------------------------------------------------------------------------
//  Bit 0 of `flags` selects looping (0x2002) over one-shot (0x2000) as the
//  base value; every other bit tested just ORs in its own output bit,
//  except bit 0x200, which - uniquely - *clears* 0x2000 (the looping flag)
//  when set, overriding whatever bit 0 chose.  The low seven tested bits
//  are read off the byte (the shipped `test cl, ...`), the rest off the
//  full dword; both are the same `flags & mask` in source, and which width
//  the compiler picks for the `test` is its own choice for a mask that
//  fits a byte.
//----------------------------------------------------------------------------
unsigned int __fastcall PackSoundPlayFlags(unsigned int flags)
{
    unsigned int result = 0x2000;
    if (flags & 1)
        result = 0x2002;

    if (flags & 2)
        result |= 4;
    if (flags & 4)
        result |= 8;
    if (flags & 8)
        result |= 0x10;
    if (flags & 0x10)
        result |= 0x20;
    if (flags & 0x20)
        result |= 0x40;
    if (flags & 0x40)
        result |= 0x80;
    if (flags & 0x80)
        result |= 0x400;
    if (flags & 0x100)
        result |= 0x800;
    if (flags & 0x8000)
        result |= 0x8000;
    if (flags & 0x80000)
        result |= 0x20000;
    if (flags & 0x200)
        result &= 0xFFFFDFFFu;

    return result;
}
