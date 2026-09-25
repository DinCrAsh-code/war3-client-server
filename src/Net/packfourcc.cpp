//============================================================================
//  0x6F4C3F50 - PackFourCCString: turn a four-character string into the
//  packed big-endian FourCC every type id in this binary is (agiletype.h).
//
//  Stops at the first zero byte, so a shorter string packs into fewer bytes
//  rather than reading past its terminator, and a null pointer packs to 0.
//  This is what identifies CItem's placement descriptor +0x78 as a *string*
//  and not a word (item_place.cpp).
//
//  Own translation unit: its caller reaches it with a real call, and eleven
//  instructions is well inside what /Ob2 would inline given the chance.
//============================================================================

unsigned int __fastcall PackFourCCString(const char* text)
{
    //  The null case guarded rather than returned early: the shipped code
    //  jumps forward to the shared exit, where an early `return 0` builds a
    //  second epilogue of its own (docs/msvc-vc8-idioms.md, "Shared exits
    //  need goto").
    unsigned int packed = 0;
    if (text != 0)
    {
        //  A counted loop over exactly four bytes with an inner break, not
        //  a `while (*p)`: the shipped code compares the index against 4 at
        //  the bottom and tests the byte at the top, which is what this
        //  shape gives.
        for (unsigned int i = 0; i < 4; i++)
        {
            unsigned char c = (unsigned char)text[i];
            if (c == 0)
                break;
            packed = (packed << 8) | c;
        }
    }
    return packed;
}

//============================================================================
//  0x6F4C3F80 - UnpackFourCCString: the inverse.
//
//  Writes the four bytes of `fourCC` most-significant first into `out`,
//  skipping a leading zero byte but keeping a zero once anything has been
//  written, NUL-terminates, and answers how many characters it wrote.  The
//  running length doubles as the "have I written anything yet" flag, which
//  is why every byte after the first tests both it and the byte
//  (`jnz write` / `test eax,eax` / `jz skip`).
//
//  The size argument is never read - the shipped body cleans it (`retn 4`)
//  and touches nothing but ecx and edx - so it is a debug-build bound the
//  release code drops, kept in the signature because the caller pushes it.
//
//  Same translation unit as the packer above: they are one module (0x30
//  bytes apart) and neither calls the other, so nothing can be inlined
//  across.
//============================================================================
int __fastcall UnpackFourCCString(unsigned int fourCC, char* out, int)
{
    int length = 0;

    unsigned char c = (unsigned char)(fourCC >> 24);
    if (c != 0)
    {
        out[0] = (char)c;
        length = 1;
    }

    c = (unsigned char)(fourCC >> 16);
    if (c != 0 || length != 0)
    {
        out[length] = (char)c;
        length++;
    }

    c = (unsigned char)(fourCC >> 8);
    if (c != 0 || length != 0)
    {
        out[length] = (char)c;
        length++;
    }

    c = (unsigned char)fourCC;
    if (c != 0 || length != 0)
    {
        out[length] = (char)c;
        length++;
    }

    out[length] = 0;
    return length;
}
