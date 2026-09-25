//============================================================================
//  0x6F00E7A0 - SPlayerTableSubmitParts::Submit, the second half of
//  SPlayerTableSubmit::SubmitMasked (playertablesubmit.cpp).  A naked
//  redirect there until now.
//
//  No calls at all: a twelve-entry table built on the stack, one row picked
//  by two of the object's own fields, one column picked by two 16-bit
//  tests, and that entry returned.
//
//  **Both codes are 16-bit values in 32-bit slots**, which is what the
//  `test dx, dx` and the two `movzx`es say, and it is what settles what
//  SubmitMasked is really passing: Prepare writes a *word* through each of
//  the two out-pointers it is given, and one of those points at
//  SubmitMasked's own second parameter slot.  The redirect used to declare
//  that parameter `CWorldVec3*` because that is what the caller's slot held
//  before Prepare overwrote it; the declaration is honest now.  Four
//  dwords either way, so no ABI moved and SubmitMasked's own instruction
//  stream is unchanged.
//
//  The table is a local, rebuilt on every call - twelve `mov`s with the
//  three distinct values hoisted into registers (`mov eax, 4` /
//  `mov edx, 2` and two immediate ones), which is what MSVC expands a
//  braced aggregate initialiser into and not what a `static const` table
//  gives (that is one `lea` at a .rdata address).
//
//  One instruction of thirty-nine is not reproducible: the shipped `test`
//  puts the mask first and the complement second, and writing the `&` either
//  way round leaves MSVC emitting them the other way.  Measured both ways.
//
//  Own translation unit: SubmitMasked reaches it with a real call.
//============================================================================

struct SPlayerTableSubmitParts
{
    int __thiscall Submit(unsigned int codeA, unsigned int codeB,
                          unsigned int playerMask);
    //  0x6F00E830 - see below.
    int __thiscall SubmitFromGrid(int rowArg, int colArg,
                                  unsigned int playerMask);

    char m_reserved00[0x10];
    int  m_rowLow;      // +0x10
    int  m_rowHigh;     // +0x14 - contributes two to the row index
    char m_reserved18[0x14];   // +0x18 .. +0x2B
    const unsigned short* m_colCodes;  // +0x2C - masked with ~0xF000
    const unsigned short* m_rowCodes;  // +0x30 - tagged with 0xF000
    char m_reserved34[0x2C];   // +0x34 .. +0x5F
    //  Unsigned: the shipped first-stage clamp compiles to `jb` (unsigned
    //  below), not `jl` - only reproducible if the bound side of the compare
    //  is unsigned so the signed `rowArg`/`colArg` promote to match it.
    unsigned int m_rowBound;   // +0x60 - exclusive upper bound for rowArg
    int          m_reserved64; // +0x64
    int          m_colShift;   // +0x68 - `colIdx << m_colShift` combines the two
    unsigned int m_colBound;   // +0x6C - exclusive upper bound for colArg
};

//============================================================================
//  0x6F00E830 - SPlayerTableSubmitParts::SubmitFromGrid: clamps a (row, col)
//  pair into the object's own grid bounds, folds them into one index with a
//  shift instead of a multiply (`shl ebx, cl` off m_colShift), looks up the
//  two 16-bit codes the grid holds for that cell, and tail-calls Submit with
//  them - a sibling of Prepare (0x6F26D0C0, playertablesubmitprepare.cpp)
//  that starts from precomputed grid indices instead of a world position.
//
//  Both clamps are two *chained* ternary selects (docs/msvc-vc8-idioms.md,
//  "A clamp written as two chained selects duplicates the tail") rather than
//  an `if` or a single three-way clamp: `(x < bound-1) ? x : bound-1` first
//  (`sub eax,1` / `cmp`/`jb`), then `(0 < that) ? that : 0` separately
//  (`xor`/`test`/`setl`/`sub`/`and`) - an `if`/`else if` chain collapses to
//  a branch here and does not reproduce this.
//
//  The two stores back through [esp+arg_0]/[esp+arg_4] before the tail jump
//  are this function reusing its own caller-supplied stack slots as Submit's
//  first two arguments - `playerMask` (arg_8) is never touched and passes
//  through unread, which is what makes the plain `jmp sub_6F00E7A0` (no
//  `add esp`) a real tail call rather than a bug: this function and Submit
//  share one `retn 0Ch` frame.
//
//  0.122 (5/42) is every instruction reproduced in the same order with the
//  same opcodes, offsets and immediates - side by side with
//  `tools/compare.py`, the two streams line up one-for-one except for which
//  physical register (`eax`/`ecx`/`edx`/`esi`/`ebx`) holds which value, plus
//  a harmless reordering of which of `this`/`rowArg` gets loaded first.  No
//  spelling of the two chained clamps tried moved that allocation; see
//  BEHAVIOUR in funcmap.py.  DIFFERS, not IDENTICAL, per this repo's current
//  rule that only the (not yet built) angr checker may claim IDENTICAL.
//============================================================================
int __thiscall SPlayerTableSubmitParts::SubmitFromGrid(int rowArg, int colArg,
                                                        unsigned int playerMask)
{
    int rowIdx = (m_rowBound - 1 < (unsigned int)rowArg) ? (m_rowBound - 1) : rowArg;
    rowIdx = (rowIdx < 0) ? 0 : rowIdx;

    int colIdx = (m_colBound - 1 < (unsigned int)colArg) ? (m_colBound - 1) : colArg;
    colIdx = (colIdx < 0) ? 0 : colIdx;

    int index = rowIdx + (colIdx << m_colShift);

    unsigned int codeB = m_colCodes[index] & ~0xF000;
    unsigned int codeA = m_rowCodes[index] | 0xF000;

    return Submit(codeA, codeB, playerMask);
}

int __thiscall SPlayerTableSubmitParts::Submit(unsigned int codeA,
                                               unsigned int codeB,
                                               unsigned int playerMask)
{
    //  Four rows of three.  The row is m_rowLow + 2*m_rowHigh, which is
    //  the shipped `lea eax,[ecx+eax*2]`; the column is chosen below.
    int answers[4][3] =
    {
        { 4, 4, 4 },
        { 4, 4, 1 },
        { 4, 2, 2 },
        { 4, 2, 1 },
    };

    const int* row = answers[m_rowLow + 2 * m_rowHigh];

    //  Column 0: the two codes overlap in their low sixteen bits.
    if ((unsigned short)(codeA & playerMask) != 0)
        return row[0];

    //  Column 1: the mask has a bit the second code does not.  The
    //  complement is taken of the *widened* word, not of the 16-bit value -
    //  the shipped `movzx` then `not` is a 32-bit complement of a
    //  zero-extended word, so the high half comes out all ones and the
    //  AND against the zero-extended mask still answers the same question.
    if (((unsigned short)playerMask & ~(unsigned int)(unsigned short)codeB) != 0)
        return row[1];

    return row[2];
}
