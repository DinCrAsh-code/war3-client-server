//============================================================================
//  0x6F4CCD70 - lay a string out into a fresh TEXTBLOCK.
//
//  The font arrives in ecx and the *text* in edx, with seven stack arguments
//  behind them (`retn 1Ch`), so this is a free `__fastcall` and not a member
//  of the font - docs/msvc-vc8-idioms.md, "A receiver in ecx with a second
//  argument in edx is a free `__fastcall`".  While 0x6F4CCD70 was a redirect
//  this repo declared it as `SFontHandle::BuildTextBlock`, a __thiscall with
//  seven stack arguments: the byte count matched, so neither audit could see
//  it, but the text pointer the shipped caller leaves in edx was never
//  passed - the redirect worked only because this build happened to leave
//  the same value there.
//
//  Three coordinate conversions, all of them ScaleFromUiSpan: the position
//  into a fresh three-float vector, the b/c pair in place in their own
//  argument slots, and `a` on its own with a null X out-pointer.  Writing
//  back into the parameter slots is the shipped code's own shape - see
//  "The shipped code writes its results back into its own parameter slots".
//============================================================================
#include "text.h"
#include "renderthunks.h"
#include "uispan.h"

static const char kTextBlockTag[] = "HTEXTBLOCK";

void* __fastcall BuildTextBlock(void* font, const char* text,
                                const void* color, const float* position,
                                float a, float b, float c,
                                int style, float depth)
{
    //  Not placement new, which is the obvious reading of an allocation
    //  followed by a guarded three-store fill: `new (p) TEXTBLOCK` gives a
    //  *join* - the filled pointer on one edge and a fresh `xor` on the
    //  other - and the shipped stream has none.  It starts at the failure
    //  value and overwrites, which lets MSVC reuse the zero it already has
    //  in a register as the null test's comparand (`cmp eax, edi`) exactly
    //  as it does for the `push 0` argument just above.
    TEXTBLOCK* block = (TEXTBLOCK*)g_textBlockPool.Alloc(0, kTextBlockTag, -2);
    if (block != 0)
    {
        block->m_refcount = 0;
        block->m_engineBlock = 0;
        block->m_vtable = (void*)kTextBlockVftable;
    }

    float where[3];
    where[0] = 0.0f;
    where[1] = 0.0f;
    where[2] = position[2];
    ScaleFromUiSpan(&where[0], &where[1], position[0], position[1]);

    ScaleFromUiSpan(&b, &c, b, c);
    ScaleFromUiSpan(0, &a, 0.0f, a);

    int horizontal = 1;
    if (style & 4)
        horizontal = 2;
    else if ((style & 2) == 0 && (style & 1) != 0)
        horizontal = 0;

    int vertical = 1;
    if (style & 8)
        vertical = 0;
    else if (style & 0x20)
        vertical = 2;

    //  RemapTextFlags is called *inside* the argument list rather than
    //  through a named local: MSVC evaluates arguments right to left, so
    //  this is what puts the call between the colour push and the alignment
    //  pushes, exactly where the shipped stream has it.
    TextEngineLayout(font, text, a, where, b, c, 0.0f, &block->m_engineBlock,
                     vertical, horizontal, RemapTextFlags(style), color,
                     depth);

    return ((TRefCnt*)block)->AddRef("HTEXTBLOCK");
}
