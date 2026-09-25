//============================================================================
//  The fog-of-war side of the player table: "what can player P see at this
//  world point?"
//
//  The three queries below are one function three times over - clamp the
//  point into the playable rectangle, turn it into the pair of 16-bit cell
//  codes Prepare() produces, hand those plus a player mask to Submit(), and
//  compare the answer against one of Submit()'s three possible results.
//  Submit is the twelve-entry table at 0x6F00E7A0 (playertablesubmitparts.cpp)
//  and its three values are the three fog states: 4 visible, 2 fogged,
//  1 masked - which is what names these three functions and what the three
//  JASS natives that reach them (IsVisibleToPlayer, IsFoggedToPlayer,
//  IsMaskedToPlayer) confirm one for one.
//
//  The class keeps the name SPlayerTableSubmit*Parts* it already had in
//  playertablesubmit.cpp and playertablesubmitparts.cpp - same object, same
//  two method signatures, declared here a third time so the query bodies
//  below can call them.  Only the members each file touches are declared in
//  it, which is why the three declarations differ in their data and agree
//  exactly in their functions.
//============================================================================
#ifndef PLAYERTABLEFOG_H
#define PLAYERTABLEFOG_H

class CWorldVec3;
class CFloat;
class CUnit;
struct SJassBounds;

struct SPlayerTableSubmitParts
{
    //  0x6F26D0C0 - playertablesubmitprepare.cpp.  Both out-pointers are
    //  `unsigned short*`; see that file and playertablesubmit.cpp.
    void __thiscall Prepare(CWorldVec3* pos, unsigned short* outCodeA,
                            unsigned short* outCodeB);
    //  0x6F00E7A0 - playertablesubmitparts.cpp.
    int  __thiscall Submit(unsigned int codeA, unsigned int codeB,
                           unsigned int playerMask);

    //  0x6F3BA430 - playertablepointvisible.cpp.
    int IsPointVisible(CWorldVec3 pos, unsigned int playerMask);
    //  0x6F276240 / 0x6F276290 - playertablepointfog.cpp.
    int IsPointFogged(CWorldVec3 pos, unsigned int playerMask);
    int IsPointMasked(CWorldVec3 pos, unsigned int playerMask);

    //  The JASS SetFogState* family - fogstatetablewriter.cpp and
    //  jassnatives_fogstate.cpp.
    //
    //  0x6F408110 (turn a player slot + useSharedVision into the 16-bit
    //  mask word these writers want) is NOT declared here: it is
    //  FogStatePlayerMaskOrRecordMask, a free function declared in
    //  src/Jass/jassfogstateplayermask.cpp - desync_mechanism_worklist.json
    //  batch B reconstructed it independently under that name/shape before
    //  this batch's own merge surfaced the address collision, and its
    //  version is the one kept (see that file and jassnatives_fogstate.cpp
    //  for the callers).

    //  0x6F3B76E0 - write `fogWord` through the grid at `bounds` with the
    //  given mode (JASS_SetFogStateRect).
    void ApplyFogRect(int mode, unsigned short fogWord,
                      const SJassBounds* bounds);
    //  0x6F3BA480 - the point-plus-radius counterpart
    //  (JASS_SetFogStateRadius / JASS_SetFogStateRadiusLoc).  Still
    //  unhooked: its own `__except_handler4`-shaped SEH frame is the same
    //  one this repo's Resolve*Handle family already gave up matching (see
    //  jassresolveplayerhandle.cpp), and its own callee
    //  (0x6F408FD0, the circle-span table builder) has no dump yet.
    //  `xy` is a 3-int point (x, y, and a hard-zeroed z - both JASS callers
    //  build one even though only x/y are real, per the disassembly's own
    //  otherwise-dead `g_CFloatZero` store into the third slot).
    void ApplyFogRadius(int mode, unsigned short fogWord, const int* xyz,
                        const CFloat* radius);

    //  0x6F3B0E90 - the rectangle body ApplyFogRect calls once it has both
    //  corners as cells: clamp to the grid, then write every row in
    //  [rowStart, rowEnd] across [colStart, colEnd].  Naked: three
    //  near-identical bit-twiddling loops (one per `mode`), transcribed
    //  exactly rather than re-derived, the way
    //  docs/msvc-vc8-idioms.md's naked-body rule asks for.
    void WriteFogRegionRows(int mode, unsigned short value, int colStart,
                            int rowStart, int colEnd, int rowEnd);
    //  0x6F3B0E20 - the single-cell counterpart ApplyFogRadius's own corner
    //  cases use.  Naked, same reason.
    void WriteFogCell(int mode, unsigned short value, int col, int row);

    //  0x6F406850 / 0x6F4069F0 / 0x6F406920 - the three single-row writers
    //  0x6F3BA480's own row loop calls once for each row of a filled span
    //  (the same three `mode`-selected AND/OR combinations
    //  WriteFogRegionRows and WriteFogCell switch on, applied to one row
    //  instead of a range).  Not reached from anything reconstructed for
    //  real yet - 0x6F3BA480 itself is still a thunk - but real,
    //  addressable bodies on the same object, in their own right.  Naked,
    //  same reason as WriteFogRegionRows.
    //
    //  mode 4 (falls through the switch's default case): AND-clear the high
    //  word, OR-set the low word.
    void WriteRowClearHighOrLow(int row, int colStart, int colEnd,
                                unsigned short value);
    //  mode 1: OR-set the high word, AND-clear the low word.
    void WriteRowOrHighClearLow(int row, int colStart, int colEnd,
                                unsigned short value);
    //  mode 2: AND-clear both words with the same mask.
    void WriteRowClearBoth(int row, int colStart, int colEnd,
                           unsigned short value);

    //  desync_mechanism_worklist.json's "closure650" batch - a second,
    //  per-unit fog path distinct from the JASS SetFogState* family above
    //  (Widget/playertableunitfogrefresh.cpp).
    //
    //  0x6F40A650 - refresh what `unit` publishes into the selection/
    //  relation-mask machinery and this grid's own fog word for its
    //  owning player, then forward to ApplyUnitFogRadius.  `this` is
    //  never read directly (every field access happens inside
    //  ApplyUnitFogRadius) - the same "receiver forwarded, not touched"
    //  shape GetPlayerFogWord already has.
    //
    //  `presetMaskOrAll`: its low 16 bits are read *once*, before the
    //  compiler reuses the same stack slot to hold the freshly computed
    //  mask - the literal sentinel 0xFFFF asks this call to compute the
    //  mask fresh from `unit`'s relation-agent/vision state; any other
    //  value is used as the fog mask outright and every relation/vision
    //  query below is skipped.  See playertableunitfogrefresh.cpp.
    void RefreshUnitFog(CUnit* unit, unsigned int presetMaskOrAll);
    //  0x6F409E00 - the 676-instruction radius/shape worker RefreshUnitFog
    //  forwards to (four rows of near-identical span-fill loops indexed by
    //  a 0..2 case in `[esp+arg_4]`/`edi` and `ebx`, all reading/writing
    //  this object's own `m_lowCodes`/`m_highCodes`/`m_columnCount`/
    //  `m_rowCount`/`m_rowShift` fields).  Still a thunk: its own dump is
    //  fully in hand (asm/sub_6F40A650_0x6F40A650_calltree_asm.md) but is
    //  well past this batch's remaining budget - a concrete next step, not
    //  a guess.  `retn 10h` confirmed off that same dump's own tail.
    void ApplyUnitFogRadius(CUnit* unit, unsigned int fogMask, int, int);
};

//  The object at dword_6FAB65F4 seen as the player table: the same +0x34
//  sub-object SPlayerTableSubmit::SubmitMasked (playertablesubmit.cpp)
//  reads off its own `this`.
struct SPlayerTableFogHost
{
    char                     m_reserved00[0x34];
    SPlayerTableSubmitParts* m_pParts;   // +0x34
    //  0x6F3A1460 - playertablerebuildmasks.cpp.
    void RebuildPlayerMasks(const CWorldVec3* pos, unsigned short* outVisibleMask,
                            unsigned short* outFoggedMask,
                            const unsigned short* players);

    char                     m_reserved38[0x3C0 - 0x38];
    //  +0x3C0 - read by SPlayerTableFogHost::RebuildPlayerMasks
    //  (playertablerebuildmasks.cpp) as a boolean: when set, a player Submit
    //  ranks merely "fogged" (bit 1 of its answer) is folded into that
    //  function's second output mask unconditionally; when clear, that fold
    //  only happens for a player Submit ranks fogged specifically.  What the
    //  flag itself represents (shared vision, an observer flag, a replay
    //  setting) is not established by this call tree - only the direction
    //  of the test is.
    unsigned int             m_field3C0;   // +0x3C0
};

#endif
