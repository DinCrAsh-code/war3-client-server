//============================================================================
//  0x6F30FE20 - SSoundLabelEntry::PickVariant.  Reached through
//  CWidget::NegateAndForwardOffset's closure (the same neighbourhood as
//  ResolveSoundLabelFlag/widgetsoundlabel.cpp): the object this runs on is
//  whatever `this` a successful SSoundLabelTableThunk::LookupSoundLabelTable
//  (0x6F3199F0) hands back - a sound label's own record - so it is named to
//  match that file's "SSoundLabelTable*" family rather than invented cold.
//  Only the three fields this body actually reads/writes get names; the rest
//  of the record's layout belongs to 0x6F3199F0's own out-of-scope Find.
//
//  Four ways to choose a variant, tried in the shipped order:
//    - `flags & 4`  : advance sequentially from the previous pick, wrapping
//                     via `%`  (a plain `div`, not the multiply-shift below);
//    - `flags & 1`  : an explicit index; out of range fails without a copy,
//                     leaving m_lastVariant == -1 as a "nothing valid" mark;
//    - one variant  : always index 0, no random draw needed;
//    - otherwise    : a random draw that avoids repeating the immediately
//                      previous pick, retried up to 10 times before giving
//                      up and accepting whatever the 10th draw was.
//
//  The random draw reuses the same global SHashState the widget-offset
//  randomizer advances (Math/hashstate.h, unk_6FAB58CC - already
//  SHashState::Advance, Widget/widgetoffsetrandom.cpp) and maps its raw
//  32-bit value into [0, count) with the classic 64-bit multiply-shift
//  (`index = (raw * count) >> 32`).  Both operands are cast to
//  `unsigned __int64` here (not just one) - with only the left operand cast,
//  this compiler recognises the "high half of a 32x32 product" shape and
//  emits a single hardware `mul`; casting both defeats that recognition and
//  reproduces the shipped `__allmul`/`__aullshr` pair instead of a `mul`.
//
//  A variant is a plain C string; the picked one is copied out through the
//  Storm import every other `Storm_501` call site in this repo already
//  declares (`Storm_501(dest, src, maxLen)`, __stdcall).
//
//  The retry loop's own exit test - `while (index == m_lastVariant &&
//  tries++ < 10)` - is load-bearing in both halves: the `&&` makes the
//  increment-and-compare conditional on the repeat check, matching the
//  shipped `jnz` past it, and the *postfix* `tries++` matters because the
//  shipped code compares the pre-increment counter (`mov ecx, ebp` /
//  `add ebp, 1` / `cmp ecx, 0Ah`) - a prefix `++tries` or a separate
//  `tries < 10` test would compare the post-increment value instead and
//  allow one fewer retry than the shipped body does.  `tries` is
//  `unsigned int`, not `int`: the comparison the compiler emits for it is
//  `jb` (unsigned), and a signed `tries` flips that to `jl`.
//============================================================================
#include "hashstate.h"

extern SHashState g_widgetRandomHashState;   // unk_6FAB58CC

void __stdcall Storm_501(char* dest, const char* src, unsigned int size);

struct SSoundLabelEntry
{
    char               m_reserved00[0x20];
    unsigned int       m_variantCount;    // +0x20
    const char* const* m_variantPaths;    // +0x24
    char               m_reserved28[0x70 - 0x28];
    int                m_lastVariant;     // +0x70

    void __thiscall PickVariant(char* dest, unsigned int destSize,
                                 unsigned char flags, unsigned int explicitIndex);
};

void __thiscall SSoundLabelEntry::PickVariant(char* dest, unsigned int destSize,
                                               unsigned char flags,
                                               unsigned int explicitIndex)
{
    dest[0] = 0;

    unsigned int count = m_variantCount;
    if (count == 0)
        return;

    int index;
    if (flags & 4)
    {
        index = (int)((m_lastVariant + 1) % count);
    }
    else if (flags & 1)
    {
        if (explicitIndex >= count)
        {
            dest[0] = 0;
            m_lastVariant = -1;
            return;
        }
        index = (int)explicitIndex;
    }
    else if (count == 1)
    {
        index = 0;
    }
    else
    {
        unsigned int tries = 0;
        do
        {
            unsigned int raw = (unsigned int)g_widgetRandomHashState.Advance();
            index = (int)(unsigned int)(((unsigned __int64)raw *
                                          (unsigned __int64)count) >> 32);
        } while (index == m_lastVariant && tries++ < 10);
    }

    m_lastVariant = index;
    Storm_501(dest, m_variantPaths[index], destSize);
}
