//============================================================================
//  The unit-query object BuildAndRegisterEventBinding (0x6F2A3270,
//  agenteventbinding.cpp) builds, and the two arrays of filter predicates
//  it fills.
//
//  Only the offsets that function touches are named, which is most of the
//  first 0x94 bytes and nothing after it.  What the object is used for is
//  read off the function itself: a callback plus its context, a player
//  mask, a flag word whose bits are the tag numbers of the variadic
//  options that set them, a handful of typed parameters, and two arrays of
//  `int (__fastcall*)(candidate, query)` predicates - one run while
//  candidates are being collected and one run over the collected set.
//============================================================================
#ifndef AGENTEVENTBINDING_H
#define AGENTEVENTBINDING_H

#include "storm.h"

class CUnit;
struct SUnitQuery;

//----------------------------------------------------------------------------
//  The JASS game-state singleton's own owner-relationship masks, tags
//  16..23.  Its object is dword_6FAB65F4, which itemhandlemain.h already
//  names g_unk6FAB65F4.
//----------------------------------------------------------------------------
//  DetectedMask reads the game-state block this singleton keeps at +0x30;
//  the other six touch nothing but the array `At` indexes, so +0x30 is the
//  only field named.  Bit 0x10000 of that block's own +0x38 is the "also
//  count what is merely visible" switch.
struct SJassGameStateBlock
{
    char         m_reserved00[0x38];
    unsigned int m_flags38;                         // +0x38
};

struct CJassGameStateMasks
{
    char                 m_reserved00[0x30];
    SJassGameStateBlock* m_pGameState;              // +0x30

    //  All seven in jassgamestatemasks.cpp.  Thiscall members whose `this`
    //  is the array they index - see that file's own note.
    unsigned int PlayerMask(int player);            // 0x6F3A3490
    unsigned int AllyMask(int player);              // 0x6F3A3510
    unsigned int EnemyMask(int player);             // 0x6F3A3730
    unsigned int VisibleMask(int player);           // 0x6F3A35A0
    unsigned int DetectedMask(int player);          // 0x6F3A3680
    unsigned int SelectableMask(int player);        // 0x6F3A3780
    int          SlotMatches(int slot, int value);  // 0x6F3A37A0
};

//  Every predicate and the visit callback share one shape, read off the
//  shipped call sites: `mov ecx,<candidate>` / `mov edx,<query or
//  context>` / `call` with nothing pushed, so __fastcall and `retn 0`.
typedef int (__fastcall *UnitQueryPredicateFn)(void* candidate, SUnitQuery* query);
typedef int (__fastcall *UnitQueryVisitFn)(void* candidate, void* context);

//  The centre a circle query is run around: `this` for 0x6F476DC0.  Its
//  one member takes seven stack words and nothing in ecx but the object
//  itself - see agenteventbinding.cpp for the call site that fixes that.
//
//  **It is a CPathRef** (game.h): 0x6F476DC0 calls CPathRef::ToWorldVec3 on
//  this same `this`, and 0x6F476670 below resolves the {handle, typeTag}
//  pair at +0x08/+0x0C that CPathRef's own layout has.  The two names are
//  kept apart only because the declared parameter types here are what
//  BuildAndRegisterEventBinding's call site already mangles against;
//  src/Agent/unitquerycircle.cpp casts once and says so.
struct SUnitQueryCircleCentre
{
    //  0x6F476DC0.  `origin` is really the query *radius* - a CFloat by
    //  address, kept `const int*` because that is the type the one shipped
    //  call site is already compiled against.  `altForm` picks the
    //  touching-circle rule, and with it the radius grows by the traced
    //  object's own (GetWorldRadius below) before the enumeration runs.
    void EnumerateUnits(const int* origin, int altForm, unsigned int mask,
                        int a, int b, UnitQueryVisitFn fn, void* ctx);

    //  0x6F476670 - the traced object's own collision radius, converted out
    //  of grid units into world units, written through `out` and handed
    //  back.  `retn 4`.
    int* GetWorldRadius(int* out);
};


struct SUnitQuery
{
    //  0x6F28C750 - the record's own constructor: zero the three words the
    //  finaliser does not, zero all three arrays, reserve 0x1F pre-filters
    //  and 0x100 candidate slots, and set the candidate array's growth
    //  granularity to 0x80.  See src/Agent/unitqueryrecord.cpp.
    SUnitQuery();

    //  0x00 / 0x04 - what to do with each accepted candidate.  The
    //  callback defaults to a shipped no-op when the caller passes null.
    UnitQueryVisitFn m_visit;           // 0x00
    void*            m_context;         // 0x04

    //  0x08 - a candidate selector, -1 for "any"; adds one post-filter.
    int              m_selector;        // 0x08
    //  0x0C - the player mask, 0xFFFF for "all".  Seven of the variadic
    //  tags narrow it, each through its own owner-relationship query.
    unsigned int     m_playerMask;      // 0x0C
    //  0x10 - the option bits.  Bit N is set by variadic tag N, which is
    //  why the shipped code writes them with `mov eax,1` / `shl eax,cl`
    //  off the tag value itself rather than from a constant.
    unsigned int     m_flags;           // 0x10
    //  0x14 - tag 15's parameter; nonzero adds one pre-filter.
    int              m_param15;         // 0x14
    //  0x18 / 0x1C - a world position, filled either from tag 14's two
    //  words directly or from tag 12's handle through
    //  CPathRef::ToWorldVec3.  Deliberately *not* in the initialiser
    //  block: the shipped code never zeroes them, so they are only
    //  meaningful when one of those two tags has set them - which is
    //  exactly what the 0x1000/0x4000 flag bits record.
    int              m_posX;            // 0x18
    int              m_posY;            // 0x1C
    //  0x20 - zeroed by the constructor alongside the position pair and
    //  never read again anywhere this repo has reconstructed.
    int              m_field20;         // 0x20
    //  0x24 / 0x28 - tag 12's two words, kept as given.
    int              m_param12b;        // 0x24
    int              m_param12a;        // 0x28
    //  0x2C - tag 28's parameter.
    int              m_param28;         // 0x2C
    //  0x30 - tag 27's; -1 and 0 both mean "unset".
    int              m_param27;         // 0x30
    //  0x34 / 0x38 - two independent classification masks.  Tags 1/3 set
    //  bit 16 in one each, tags 4/5/6 clear bits in them, and the
    //  finaliser folds their 0x100 bits into the mask it hands the
    //  enumerator.
    unsigned int     m_classMaskA;      // 0x34
    unsigned int     m_classMaskB;      // 0x38
    //  0x3C..0x44 - tag 29's three words.  The third defaults to -1 where
    //  the first two default to zero.
    int              m_param29a;        // 0x3C
    int              m_param29b;        // 0x40
    int              m_param29c;        // 0x44
    //  0x48 - tag 30's.
    int              m_param30;         // 0x48
    //  0x4C - the answer, returned when the flags ask for one.
    int              m_result;          // 0x4C
    //  0x50 / 0x54 - two words this function only ever initialises, both
    //  from the same global.  Nothing here reads them back.
    int              m_field50;         // 0x50
    int              m_field54;         // 0x54
    //  0x58 - tag 13's; 0x80 by default.
    int              m_param13;         // 0x58
    //  0x5C - whether the game-mode gate at 0x6F468FC0 is *shut*: the
    //  shipped code inverts its answer with the `neg`/`sbb`/`add 1`
    //  fold, so this is 1 when that function returns 0.
    int              m_modeGateShut;    // 0x5C
    //  0x60 - the pre-filters, run as candidates are collected.
    TSGrowableArray<UnitQueryPredicateFn> m_pre;    // 0x60
    //  0x70 - the post-filters, run over the collected set.
    TSGrowableArray<UnitQueryPredicateFn> m_post;   // 0x70
    //  0x80 - the collected candidates.  A third growable array, not a
    //  loose {pointer, count} pair: the record's own constructor
    //  (src/Agent/unitqueryrecord.cpp) reserves 0x100 entries in it through
    //  the same SetCount() it reserves 0x1F pre-filters with, and then
    //  stamps 0x80 into its m_chunk.  The element type is ground truth -
    //  0x6F0861E0's Storm tag is the RTTI descriptor `.PAVCUnit@@`.
    //  Clearing m_count is what used to be `m_field84 = 0`.
    TSGrowableArray<CUnit*> m_found;    // 0x80
    unsigned int     m_candidateCount;  // 0x90
};

//----------------------------------------------------------------------------
//  Everything BuildAndRegisterEventBinding calls that is below this
//  target's depth.  Declared here rather than in the .cpp so that
//  agenteventbinding_thunks.cpp's redirects are built against the same
//  declarations - MSVC puts the convention and every parameter type in
//  the mangled name.  Each convention is read off the shipped call site;
//  agenteventbinding.cpp quotes which push/register pattern fixes it.
//----------------------------------------------------------------------------
SUnitQuery* MakeUnitQuery();                                     // 0x6F28C830
int IsQueryModeGateOpen();                                       // 0x6F468FC0
//----------------------------------------------------------------------------
//  0x6F2552F0 - fill in the default bits of a unit-filter mask: every one
//  of its eight bit-groups that the caller left completely empty comes back
//  set to that group's own default.  __fastcall with the mask in ecx and
//  nothing pushed.  See src/Agent/unitfilterdefaults.cpp.
//----------------------------------------------------------------------------
unsigned int __fastcall ApplyUnitFilterDefaults(unsigned int filter);

unsigned int __fastcall FoldQueryClassMask(int relations, int source,
                                           int player);  // 0x6F25D830
void __fastcall EnumerateUnitsInRect(const int* lo, const int* hi,
                                     const int* origin, int altForm,
                                     unsigned int mask, int a, int b,
                                     UnitQueryVisitFn fn, void* ctx); // 0x6F467570
void __fastcall EnumerateUnitsOfType(unsigned int typeId,
                                     UnitQueryVisitFn fn,
                                     void* ctx, int zero);            // 0x6F46D1B0

#endif
