//============================================================================
//  0x6F3BC2E0 - 0x6F3BC380: the four natives that read and write the JASS
//  *game state* values - `SetFloatGameState` / `GetFloatGameState` and
//  `SetIntegerGameState` / `GetIntegerGameState`.
//
//  What a "game state" indexes is not one array but two, both inline on the
//  '+gam' agent CItemSlotSingletonHost::GetSlotTable (itemhandlemain.h)
//  hands back, both 16 bytes to the element, and each with its own address
//  helper in Misc/misc_field_getters.cpp:
//
//    * `fgamestate` -> 0x6F427F40's array at +0x08, whose elements are
//      SUnitTrackedRefs (unittrackedref.h).  Only index 2 is in range
//      (`>= 2 && < 3`) - the game clock, which is exactly the element
//      CAgentRegistrySingleton::FinishSingleton initialises from
//      Misc/DayHours (agentregistrysingleton.cpp).  That is
//      GAME_STATE_TIME_OF_DAY.
//    * `igamestate` -> 0x6F427EC0's array at +0x3C, whose elements are the
//      published {handle, typeTag} fields misc_handle_lookups.cpp reads and
//      writes through QueryField78 / SetField78.  Indices 0 and 1 are in
//      range (`>= 0 && < 2`) - DIVINE_INTERVENTION and DISCONNECTED.
//
//  Out of range, the setters do nothing and the getters answer zero.  The
//  bounds are two independent signed comparisons in the shipped code
//  (`test edx,edx` / `jl` then `cmp edx,2` / `jge`), and getting *that* out
//  of MSVC needs both halves of the shape below:
//
//    * the index is copied into a named local first.  Compared straight
//      through the parameter, MSVC folds any spelling of the pair - `||`,
//      `&&`, two `if`s, nested `if`s, `goto`s, a `switch` - into the single
//      unsigned range check `cmp ecx,1` / `ja`, which is four instructions
//      short and the wrong ones.  A local copy inhibits the fold and the
//      two signed comparisons survive;
//    * the two ends are two separate statements, not one boolean
//      expression.  `||` on the local costs one instruction in the getters
//      (it puts the shared `xor eax,eax` / `ret` at the very end instead of
//      after the first guard) - measured, 14/18 against 15/18.
//
//  In all four the singleton fetch happens *before* the bounds test: the
//  shipped code calls GetSlotTable first and only then loads the index.
//  That is a named local, not a subexpression of the indexing.
//
//  A JASS `real` crosses this boundary as its raw 32-bit encoding, so
//  SetFloatGameState takes a `const int*` and GetFloatGameState returns an
//  `int` (jassnatives.h); the two SUnitTrackedRef members they call are
//  declared in terms of CFloat, which is the same four bytes.
//
//  One translation unit: none of the four calls another, and every callee
//  is a real call into a different module.
//============================================================================
#include "jassnatives.h"
#include "itemhandlemain.h"
#include "unittrackedref.h"

//  Both re-declared exactly as Misc/misc_field_getters.cpp defines them -
//  MSVC puts the `struct` keyword, the class name, the return type and the
//  parameter type all in the mangled name, so these declarations and those
//  definitions are the same symbol only while all four agree.  See
//  docs/notes/verifier-gate-link-failures.md.
struct IndexedElementOwner_6F427F40
{
    char pad[8];
    char m_data[1][16];
    void* At(unsigned int index);
};

struct IndexedElementOwner_6F427EC0
{
    char pad[0x3C];
    char m_data[1][16];
    void* At(unsigned int index);
};

//----------------------------------------------------------------------------
//  0x6F3BC2E0 - `SetFloatGameState` "(Hfgamestate;R)V".
//----------------------------------------------------------------------------
void __cdecl JASS_SetFloatGameState(int state, const int* value)
{
    void* registry = g_pItemSlotHost->GetSlotTable();

    int index = state;

    if (index < 2)
        return;
    if (index >= 3)
        return;

    SUnitTrackedRef* slot = (SUnitTrackedRef*)
        ((IndexedElementOwner_6F427F40*)registry)->At(index);

    slot->SetValue((const CFloat*)value);
}

//----------------------------------------------------------------------------
//  0x6F3BC310 - `GetFloatGameState` "(Hfgamestate;)R".
//
//  GetValue writes through the caller's buffer, and the buffer the shipped
//  code hands it is the *parameter's own* stack slot (`lea ecx,[esp+arg_0]`)
//  - which is why the result is written over `state` rather than into a
//  local of its own.  A declared `CFloat value;` is not the same thing: it
//  has a zeroing constructor, so it costs a `mov [esp+4],0` the shipped code
//  does not have and a `push ecx`/`pop ecx` frame to hold it in.
//
//  Three of the eighteen instructions differ, and they are the same three
//  in JASS_GetIntegerGameState: MSVC lays the shared `xor eax,eax` / `ret`
//  out immediately after the first guard, where the shipped build puts it
//  after the *second*, so both guard jumps come out with the opposite sense
//  and the opposite target.  Same instructions, same comparisons, same
//  order of tests - block placement only, and not source-addressable: the
//  `||` form, the `&&` form with the body inside, and a literal
//  `goto notFound` / `goto found` transcription all produce this build's
//  layout, MSVC normalising the last one straight back to it.
//----------------------------------------------------------------------------
int __cdecl JASS_GetFloatGameState(int state)
{
    void* registry = g_pItemSlotHost->GetSlotTable();

    int index = state;

    if (index < 2)
        return 0;
    if (index >= 3)
        return 0;

    SUnitTrackedRef* slot = (SUnitTrackedRef*)
        ((IndexedElementOwner_6F427F40*)registry)->At(index);

    slot->GetValue((CFloat*)&state);
    return state;
}

//----------------------------------------------------------------------------
//  0x6F3BC350 - `SetIntegerGameState` "(Higamestate;I)V".
//----------------------------------------------------------------------------
void __cdecl JASS_SetIntegerGameState(int state, int value)
{
    void* registry = g_pItemSlotHost->GetSlotTable();

    int index = state;

    if (index < 0)
        return;
    if (index >= 2)
        return;

    HandleRefFieldOwner_6F473170* slot = (HandleRefFieldOwner_6F473170*)
        ((IndexedElementOwner_6F427EC0*)registry)->At(index);

    slot->SetField78(value);
}

//----------------------------------------------------------------------------
//  0x6F3BC380 - `GetIntegerGameState` "(Higamestate;)I".
//
//  The read is a tail `jmp` into QueryField78, which is what MSVC emits for
//  a call with the same argument list as the last statement - and only once
//  the receiver has been resolved into a local first (docs/msvc-vc8-idioms.md).
//
//  Three of the fourteen differ, for exactly the reason
//  JASS_GetFloatGameState's note gives: where the shared `xor eax,eax` /
//  `ret` block lands.
//----------------------------------------------------------------------------
int __cdecl JASS_GetIntegerGameState(int state)
{
    void* registry = g_pItemSlotHost->GetSlotTable();

    int index = state;

    if (index < 0)
        return 0;
    if (index >= 2)
        return 0;

    HandleRefFieldOwner_6F473170* slot = (HandleRefFieldOwner_6F473170*)
        ((IndexedElementOwner_6F427EC0*)registry)->At(index);

    return slot->QueryField78();
}
