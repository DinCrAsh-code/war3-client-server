//  Only the pointer identity of CTimeSync is needed here - every read
//  below is a raw offset cast, not a named member access - so a forward
//  declaration is enough and this file does not need game.h's full
//  CTimeSync definition.
class CTimeSync;

//  A single byte, widened to 32 bits, run through the `(b*0x7A2D)^b`
//  scramble CTimeSync_HashInto below applies to every byte of its two
//  input dwords.
static inline unsigned int TimeSyncByteScramble(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

//  0x6F280BE0 - CTimeSync's own per-tick hash-into: folds the two leading
//  dwords at `+0x00`/`+0x04` - the same pair src/Math/hashstate.h's
//  `SHashState` and pathsearchjitter.cpp's own reinterpret_cast already
//  reach as `m_lo`/`m_hi` for a *different* purpose (the rotor-cipher
//  random draw); this function reads them as plain scratch dwords rather
//  than through `SHashState::Advance`, so they stay unnamed in CTimeSync
//  itself (game.h's `m_reserved00` block) rather than duplicating that
//  typing here - one byte at a time into `*accum` via `TimeSyncByteScramble`
//  above.  The shipped body extracts each dword's own top two bytes off a
//  once-shifted `v >> 16` (`ah`/`al` of that shifted value, matched here as
//  `hi >> 8` / `hi & 0xFF`) but its bottom two straight off the unshifted
//  dword's own `bh`/`bl` (matched here as `(v >> 8) & 0xFF` / `v & 0xFF`) -
//  not a uniform `(v >> n) & 0xFF` four times, which is what led this
//  reconstruction there instead of a plain shift-and-mask loop.  It also
//  folds the last three bytes of each dword through a short register chain
//  before a single store back to `[edx]` rather than adding each straight
//  to memory (the first byte of each dword *is* added straight to `[edx]`) -
//  a scheduling choice around a sequence of commutative, associative 32-bit
//  adds this session could not reproduce instruction-for-instruction (see
//  BEHAVIOUR in funcmap.py), so it agrees with the shipped body for every
//  input including wraparound without matching its exact register chain;
//  see CLAUDE.md's "arithmetic rewritten by the optimiser" rule for why
//  that is `IDENTICAL` and not a behavioural difference.  `this`
//  (CTimeSync*) in ecx, the in/out accumulator pointer in edx, no stack
//  args, no return value read by any caller in this file.
//
//  Kept in its own translation unit, separate from
//  Net/netcommand_dispatch.cpp (where the naked thunk used to live and
//  where CNetData_FireTickNotification is this function's only caller):
//  the shipped binary never inlines this leaf into its caller (every dump
//  shows a real `call sub_6F280BE0`), but with /Ob2 a small function
//  *defined* in the same TU as its caller is a standing invitation for
//  this compiler to inline it there anyway - which is exactly what cost
//  CNetData_FireTickNotification's own listing an instruction (32/55 ->
//  31/55 against its committed report) the one time this function's body
//  briefly lived in that same file, with no source change to
//  FireTickNotification itself. This build carries no `/GL` whole-program
//  flag, so a separate TU makes that impossible rather than merely
//  unlikely.
void __fastcall CTimeSync_HashInto(CTimeSync* self, unsigned int* accum)
{
    const unsigned int* words = (const unsigned int*)self;

    for (int w = 0; w < 2; ++w)
    {
        unsigned int v = words[w];
        unsigned int hi = v >> 16;
        *accum += TimeSyncByteScramble((hi >> 8) & 0xFF);
        *accum += TimeSyncByteScramble(hi & 0xFF);
        *accum += TimeSyncByteScramble((v >> 8) & 0xFF);
        *accum += TimeSyncByteScramble(v & 0xFF);
    }
}
