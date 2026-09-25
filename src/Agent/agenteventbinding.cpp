//============================================================================
//  0x6F2A3270 - BuildAndRegisterEventBinding: run a filtered walk over the
//  game's units and hand each survivor to a callback.
//
//  829 instructions and the largest function in this target by a wide
//  margin.  It is genuinely variadic: after the callback and its context
//  comes a stream of option words, each identifying itself by a small tag
//  and consuming however many further words that tag needs, terminated by
//  tag 31.  JASS_UnitItemInSlot's own tree reaches it through
//  CAgentRegistrySingleton::RebuildDayNightListeners (agentdaynight.cpp),
//  which calls it with tags 4, 6 and the terminator.
//
//  Three things about the shape are worth stating before the body, because
//  none of them is a guess:
//
//   * **the flag bit is the tag number.**  Most tags do nothing but
//     `m_flags |= 1 << tag`, and the shipped code spells that with
//     `mov eax,1` / `shl eax,cl` off the switch value still sitting in
//     ecx rather than with a per-case constant.  Writing the constants
//     out instead would emit sixteen `or [esi+10h],<imm>` where the
//     shipped stream has a shared two-instruction shift.
//
//   * **the option stream is walked with a pointer, not with va_arg.**
//     The shipped code keeps `edi` one word behind and reads `[edi+4]`,
//     bumping by 4 after every read - which is what `*++p` over the
//     argument block produces and what `va_arg` (which reads at the
//     pointer and then bumps) does not.  `*++p` also beats the
//     `p[1]; p++;` spelling by a few instructions, because the explicit
//     step is sequenced with each read rather than sitting after it where
//     this toolchain can merge two of them into one `add edi,8`.
//
//   * **the two filter arrays are filled by an inlined push.**  The
//     grow-by-one/round-to-chunk/store sequence appears fifteen times in
//     the shipped body with no call between the copies, which is what
//     TSGrowableArray's own Add does at /Ob2.  Its first copy also has
//     ComputeChunk inlined where the other fourteen call it - MSVC's
//     inline budget running out, not two different operations.
//
//  Own translation unit: it calls into eight other modules and is called
//  from one.
//
//  **Best effort, not a finished match.**  What is confirmed line for line
//  is the option table (every tag's argument count and what it writes),
//  the finaliser's mask folding, the order and the condition of all
//  fifteen predicate pushes, the three-way enumerator choice and the
//  post-filter loop's own two-level break.  What this build does not
//  reproduce is the shipped compiler's jump-table layout for the 32-case
//  switch and the exact register allocation across a body this size; see
//  the BEHAVIOUR entry for the measured number.
//============================================================================
#include "agenteventbinding.h"
#include "CFloat.h"
#include "game.h"

//----------------------------------------------------------------------------
//  Everything below this target's depth.  Argument counts are read off the
//  shipped call sites in this function's own body - the `push` count
//  before each `call` - since none of them has a dump heading of its own
//  for tools/thunk_abi_audit.py to check against.
//----------------------------------------------------------------------------
//  0x6F28C830 (`retn 0`) - hand out a query object from the pool.
//  0x6F468FC0 (`retn 0`) - is the game in the mode that gates one option?
//  0x6F25D830 - fold tag 29's three words into a mask bit.  __fastcall
//  plus one stack word.

//  CJassGameStateMasks and SUnitQueryCircleCentre are declared in
//  agenteventbinding.h so that agenteventbinding_thunks.cpp defines its
//  redirects against the same types - MSVC puts the class name in the
//  mangled name, so a second local copy would be a second symbol.
extern void* g_unk6FAB65F4;   // dword_6FAB65F4, itemhandlemain.h's

//  0x6F2867E0 (`retn 0`) - the pre-filter array's own "replace the last
//  entry" hook, used by exactly one of the fifteen pushes.  __fastcall,
//  not __cdecl: the shipped call is `mov ecx,edi` / `call` with nothing
//  pushed, so its one argument travels in ecx and the callee cleans
//  nothing.
//  0x6F26C1C0 - misc_rawcode_getters.cpp's '+w3u'.
unsigned int GetUnitTypeFourCC();

//  0x6F4743A0 - CPathRef::SmartPtrToWorldVec3, already reconstructed
//  (CPathRef.cpp) and declared in game.h.  Reached here on tag 12's first
//  word: `lea ecx,<buffer>` / `push ecx` / `mov ecx,<handle>` / `call`,
//  which is a value return through MSVC's hidden buffer, so `retn 4`.

//: dword_6FAB4270 - the re-entrancy depth this walk is nested at.  The
//: shipped code only ever decrements it here; MakeUnitQuery
//: (unitquerypool.cpp) is what increments it.
//
//: `unsigned int`, and it MUST match that file's declaration exactly - the
//: note fifteen lines below about g_unk6FAAE624 applies word for word to
//: this symbol too, and it was declared `int` here against `unsigned int`
//: there until a gate run failed with LNK2019 on ?g_unitQueryDepth@@3HA.
//: MSVC puts the type in the mangled name, gen_data.asm binds only the
//: spelling it saw, and link_check.py cannot see it because BOTH spellings
//: are funcmap.DATA globals and therefore both are expected-unresolved.
//: unsigned is the shipped type: MakeUnitQuery compares it against
//: TSGrowableArray::m_count with an unsigned branch.
extern unsigned int g_unitQueryDepth;

//: dword_6FAAE624 - the default for two of the query's own words.
//
//: `const unsigned int`, matching floatmini.h, and not a local `extern
//: int`: MSVC puts the type in the mangled name, so `?g_unk6FAAE624@@3HA`
//: and `?g_unk6FAAE624@@3IB` are two different symbols and gen_data.asm
//: only ever binds the one it saw.  That is an LNK2019 in the operator's
//: build_mix.py and nowhere else - link_check.py counts it under the
//: expected DATA-global noise because a funcmap.DATA row does exist for
//: the name.  Found by the live gate, not by anything here.
extern const unsigned int g_unk6FAAE624;

//----------------------------------------------------------------------------
//  The eighteen predicate and callback constants whose *addresses* this
//  function stores.  None of them is called from here, so a naked
//  forwarding thunk is exactly right: the address stored is the thunk's,
//  the thunk preserves every register and the stack, and whatever calls it
//  later reaches the real body with its own convention intact.  Their
//  argument counts therefore do not have to be re-derived - which matters,
//  because none of these has a dump of its own.
//----------------------------------------------------------------------------
//  Written out one by one rather than through a macro: tools/
//  thunk_abi_audit.py finds a thunk by the literal `mov eax, 0x<addr>` in
//  the source, so a macro that takes the address as a parameter hides
//  eighteen of them from the one check that could catch a wrong
//  declaration here.
//
//  The signature is not a guess even though none of these is called from
//  this function: the shipped post-filter loop at the bottom of this same
//  body calls one through `mov edx,esi` / `mov ecx,ebx` / `call eax` with
//  nothing pushed, so every entry in either array is __fastcall(candidate,
//  query) and `retn 0`.

int __fastcall UnitQueryVisitNoOp(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryVisitNoOp(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F26E480 } __asm { jmp eax } }

int __fastcall UnitQueryFilterClassMask(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryFilterClassMask(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F2776E0 } __asm { jmp eax } }

int __fastcall UnitQueryFilterFlag80(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryFilterFlag80(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F277710 } __asm { jmp eax } }

int __fastcall UnitQueryFilterFlag02(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryFilterFlag02(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F26E4A0 } __asm { jmp eax } }

int __fastcall UnitQueryFilterFlag04(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryFilterFlag04(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F26E490 } __asm { jmp eax } }

int __fastcall UnitQueryFilterFlag01(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryFilterFlag01(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F282AC0 } __asm { jmp eax } }

int __fastcall UnitQueryFilterFlag08(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryFilterFlag08(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F277700 } __asm { jmp eax } }

int __fastcall UnitQueryFilterParam15(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryFilterParam15(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F26E4B0 } __asm { jmp eax } }

int __fastcall UnitQueryFilterPlayerMask(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryFilterPlayerMask(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F26E4E0 } __asm { jmp eax } }

int __fastcall UnitQueryFilterParam27(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryFilterParam27(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F2A01F0 } __asm { jmp eax } }

int __fastcall UnitQueryFilterFlag1000_0(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryFilterFlag1000_0(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F277720 } __asm { jmp eax } }

int __fastcall UnitQueryFilterFlag2000_0(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryFilterFlag2000_0(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F26E510 } __asm { jmp eax } }

int __fastcall UnitQueryFilterParam30(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryFilterParam30(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F2A3260 } __asm { jmp eax } }

int __fastcall UnitQueryPostNearPosition(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryPostNearPosition(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F26E530 } __asm { jmp eax } }

int __fastcall UnitQueryPostAtPosition(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryPostAtPosition(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F26E620 } __asm { jmp eax } }

int __fastcall UnitQueryPostSelector(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryPostSelector(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F26E4C0 } __asm { jmp eax } }

int __fastcall UnitQueryCollectSorted(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryCollectSorted(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F28C890 } __asm { jmp eax } }

int __fastcall UnitQueryCollectPlain(void*, SUnitQuery*);
__declspec(naked) int __fastcall UnitQueryCollectPlain(void*, SUnitQuery*)
{ __asm { mov eax, 0x6F277730 } __asm { jmp eax } }


//----------------------------------------------------------------------------
//  The option tags, named for what each one does rather than numbered.
//  Only the ones with a distinguishable effect get a name; the rest are
//  reached by value through the shared `m_flags |= 1 << tag` case.
//----------------------------------------------------------------------------
const unsigned int kQueryTagCircle       = 8;
const unsigned int kQueryTagRect         = 9;
const unsigned int kQueryTagCircleAlt    = 10;
const unsigned int kQueryTagRectAlt      = 11;
const unsigned int kQueryTagFromHandle   = 12;
const unsigned int kQueryTagParam13      = 13;
const unsigned int kQueryTagAtPosition   = 14;
const unsigned int kQueryTagParam15      = 15;
const unsigned int kQueryTagOnlyPlayer   = 16;
const unsigned int kQueryTagNotPlayer    = 17;
const unsigned int kQueryTagAllies       = 18;
const unsigned int kQueryTagEnemies      = 19;
const unsigned int kQueryTagVisible      = 20;
const unsigned int kQueryTagDetected     = 21;
const unsigned int kQueryTagSelectable   = 22;
const unsigned int kQueryTagSlotScan     = 23;
const unsigned int kQueryTagOnlySlot     = 24;
const unsigned int kQueryTagMaskAnd      = 25;
const unsigned int kQueryTagSelector     = 26;
const unsigned int kQueryTagParam27      = 27;
const unsigned int kQueryTagParam28      = 28;
const unsigned int kQueryTagTriple       = 29;
const unsigned int kQueryTagParam30      = 30;
const unsigned int kQueryTagEnd          = 31;

const unsigned int kQueryClassBit        = 0x10000;
const unsigned int kQueryMaskDefault     = 0x14FD0000;
const unsigned int kQueryMaskCircleBias  = 0x15FD0000;
const unsigned int kQueryAllPlayers      = 0xFFFF;
const unsigned int kQuerySlotCount       = 16;

int __cdecl BuildAndRegisterEventBinding(void* callback, void* context, ...)
{
    //  The eight locals the finaliser and the enumerators read.  Their
    //  initial values are the shipped ones, stored before anything else
    //  happens.
    unsigned int mask     = kQueryMaskDefault;
    int          altForm  = 0;
    int          origin   = (int)g_CFloatZero.m_bits;
    int          haveCircle = 0;
    int          haveRect   = 0;
    int          circleAt   = 0;
    int          rectHi   = (int)g_CFloatZero.m_bits;
    int          rectLo   = (int)g_CFloatZero.m_bits;

    SUnitQuery* q = MakeUnitQuery();

    //  A null callback is not an error - it is replaced by the shipped
    //  no-op, which is what makes a query with no visit still collect.
    q->m_visit = callback ? (UnitQueryVisitFn)callback
                          : (UnitQueryVisitFn)UnitQueryVisitNoOp;
    q->m_context        = context;
    q->m_selector       = -1;
    q->m_playerMask     = kQueryAllPlayers;
    q->m_param15        = 0;
    q->m_flags          = 0;
    q->m_result         = 0;
    q->m_field50        = (int)g_unk6FAAE624;
    q->m_field54        = (int)g_unk6FAAE624;
    q->m_param13        = 0x80;

    //  Inverted: the shipped code folds the gate's answer with
    //  `neg`/`sbb`/`add 1`, which is 1 when it returned 0.
    q->m_modeGateShut   = IsQueryModeGateOpen() ? 0 : 1;

    q->m_param29a       = 0;
    q->m_param29b       = 0;
    q->m_param30        = 0;
    q->m_classMaskA     = 0;
    q->m_param27        = -1;
    q->m_param29c       = -1;
    q->m_classMaskB     = 0x118;

    CJassGameStateMasks* game = (CJassGameStateMasks*)g_unk6FAB65F4;

    //  One word behind, reading `[p+1]` and stepping - the shipped walk.
    const unsigned int* p = (const unsigned int*)&context;

    for (;;)
    {
        unsigned int tag = *++p;

        switch (tag)
        {
        //  Case order is the shipped jump table's own layout order, read
        //  off the case-body addresses (10, 8, 11, 9, 12, 13, 14, 24, 25,
        //  26, 27, 28, 15, 16, 17, 18, 20, 21, 19, 22, 23, 29, 30, 1, 0/7,
        //  5, 4, 6, 3, 2, 31).  MSVC lays a dense switch's bodies out in
        //  source order, so this is the one thing about the table that is
        //  source-addressable.
        //
        //  Each argument is pulled into a named local *before* the option
        //  is applied, which is what puts the pointer step ahead of the
        //  store or the call the way the shipped stream has it.
        case kQueryTagCircleAlt:
            altForm = 1;
            //  falls through - the shipped case bodies really do share
            //  their tails, which is why tag 10's own store is followed
            //  by tag 8's entry point rather than by a jump.
        case kQueryTagCircle:
            {
                int a = (int)*++p;
                int b = (int)*++p;
                origin     = a;
                circleAt   = b;
                haveCircle = 1;
            }
            break;

        case kQueryTagRectAlt:
            altForm = 1;
            //  falls through, same reason.
        case kQueryTagRect:
            {
                int a = (int)*++p;
                int b = (int)*++p;
                int c = (int)*++p;
                origin   = a;
                rectLo   = b;
                rectHi   = c;
                haveRect = 1;
            }
            break;

        case kQueryTagFromHandle:
            {
                int a = (int)*++p;
                int b = (int)*++p;

                q->m_param12a = a;
                q->m_param12b = b;

                //  Only the first two words of the returned vector are
                //  read; m_z is stored by the return and never looked at,
                //  exactly as in JASS_UnitAddItemById.
                CWorldVec3 world = ((CPathRef*)a)->SmartPtrToWorldVec3();

                q->m_flags |= 0x1000;
                q->m_posX = *(const int*)&world.m_x;
                q->m_posY = *(const int*)&world.m_y;
            }
            break;

        case kQueryTagParam13:
            {
                int v = (int)*++p;
                q->m_param13 = v;
            }
            break;

        case kQueryTagAtPosition:
            {
                int x = (int)*++p;
                int y = (int)*++p;
                q->m_flags |= 0x4000;
                q->m_posX = x;
                q->m_posY = y;
            }
            break;

        case kQueryTagOnlySlot:
            {
                int slot = (int)*++p;
                q->m_playerMask &= 1u << slot;
            }
            break;

        case kQueryTagMaskAnd:
            {
                unsigned int m = *++p;
                q->m_playerMask &= m;
            }
            break;

        case kQueryTagSelector:
            {
                int v = (int)*++p;
                q->m_selector = v;
            }
            break;

        case kQueryTagParam27:
            {
                int v = (int)*++p;
                q->m_param27 = v;
            }
            break;

        case kQueryTagParam28:
            {
                int v = (int)*++p;
                q->m_param28 = v;
                q->m_flags |= 1u << tag;
            }
            break;

        case kQueryTagParam15:
            {
                int v = (int)*++p;
                q->m_param15 = v;
            }
            break;

        case kQueryTagOnlyPlayer:
            {
                int player = (int)*++p;
                q->m_playerMask &= game->PlayerMask(player);
            }
            break;

        case kQueryTagNotPlayer:
            {
                int player = (int)*++p;
                q->m_playerMask &= ~game->PlayerMask(player) & 0xFFFF;
            }
            break;

        case kQueryTagAllies:
            {
                int player = (int)*++p;
                q->m_playerMask &= game->AllyMask(player);
            }
            break;

        case kQueryTagVisible:
            {
                int player = (int)*++p;
                q->m_playerMask &= game->VisibleMask(player);
            }
            break;

        case kQueryTagDetected:
            {
                int player = (int)*++p;
                q->m_playerMask &= game->DetectedMask(player);
            }
            break;

        case kQueryTagEnemies:
            {
                int player = (int)*++p;
                q->m_playerMask &= game->EnemyMask(player);
            }
            break;

        case kQueryTagSelectable:
            {
                int player = (int)*++p;
                q->m_playerMask &= game->SelectableMask(player);
            }
            break;

        case kQueryTagSlotScan:
            {
                //  The one tag that does not read `[p+1]`: it steps first
                //  and then reads `[p]` sixteen times, once per slot.
                p++;

                unsigned int slots = 0;
                for (unsigned int slot = 0; slot < kQuerySlotCount; slot++)
                {
                    if (game->SlotMatches((int)slot, (int)*p))
                        slots |= 1u << slot;
                }
                q->m_playerMask &= slots;
            }
            break;

        case kQueryTagTriple:
            {
                q->m_flags |= 1u << tag;
                int a = (int)*++p;
                int b = (int)*++p;
                int c = (int)*++p;
                q->m_param29a = a;
                q->m_param29b = b;
                q->m_param29c = c;
            }
            break;

        case kQueryTagParam30:
            {
                int v = (int)*++p;
                q->m_param30 = v;
            }
            break;

        case 1:
            q->m_classMaskB |= kQueryClassBit;
            //  falls through
        case 0:
        case 7:
            q->m_flags |= 1u << tag;
            break;

        case 5:
            q->m_classMaskA |= 0x100;
            //  falls through
        case 4:
            q->m_classMaskB &= ~0x100u;
            break;

        case 6:
            q->m_classMaskB &= ~0x18u;
            break;

        case 3:
            q->m_classMaskA |= kQueryClassBit;
            //  falls through
        case 2:
            q->m_flags |= 1u << tag;
            break;

        case kQueryTagEnd:
            goto finish;

        default:
            break;
        }
    }

finish:
    //  Only a spatial query folds the class masks into the enumerator's
    //  own mask; a plain one leaves both standing for the pre-filters.
    if (haveCircle || haveRect)
    {
        q->m_classMaskB &= ~0x18u;

        if (q->m_classMaskB & 0x100)
        {
            q->m_classMaskB &= ~0x100u;
            mask = kQueryMaskCircleBias;
        }
        if (q->m_classMaskA & 0x100)
        {
            mask |= 0x2000000;
            q->m_classMaskA &= ~0x100u;
        }
        if (q->m_playerMask != kQueryAllPlayers)
        {
            mask |= ~q->m_playerMask & 0xFFFF;
            q->m_playerMask = kQueryAllPlayers;
        }
        if (q->m_flags & 0x20000000)
            mask |= FoldQueryClassMask(q->m_param29a, q->m_param29b, q->m_param29c);
    }

    //  The pre-filter array is reached through a pointer of its own, not
    //  through `q`: the shipped code takes `lea edi,[esi+60h]` once and
    //  addresses all twelve of its pushes off edi, where naming the
    //  member each time re-forms the absolute offset every push.
    TSGrowableArray<UnitQueryPredicateFn>* pre = &q->m_pre;

    pre->m_count = 0;
    q->m_post.m_count = 0;
    q->m_found.m_count = 0;

    //  Fifteen conditional pushes, in the shipped order.  Each is
    //  TSGrowableArray's own grow-by-one Add, which /Ob2 inlines.
    if (q->m_classMaskA != 0 || q->m_classMaskB != 0)
        pre->Add(UnitQueryFilterClassMask);
    if (q->m_flags & 0x80)
        pre->Add(UnitQueryFilterFlag80);

    //  Three-way, not three independent tests: the shipped code jumps the
    //  whole tail once the first match is taken.
    if (q->m_flags & 0x02)
        pre->Add(UnitQueryFilterFlag02);
    else if (q->m_flags & 0x04)
        pre->Add(UnitQueryFilterFlag04);
    else if (q->m_flags & 0x01)
        *pre->Grow() = UnitQueryFilterFlag01;

    if (q->m_flags & 0x08)
        pre->Add(UnitQueryFilterFlag08);
    if (q->m_param15 != 0)
        pre->Add(UnitQueryFilterParam15);
    if (q->m_playerMask != kQueryAllPlayers)
        pre->Add(UnitQueryFilterPlayerMask);
    if (q->m_param27 != -1 && q->m_param27 != 0)
        pre->Add(UnitQueryFilterParam27);
    if (q->m_flags & 0x10000000)
        pre->Add(UnitQueryFilterFlag1000_0);
    if (q->m_flags & 0x20000000)
        pre->Add(UnitQueryFilterFlag2000_0);
    if (q->m_param30 != 0)
        pre->Add(UnitQueryFilterParam30);

    if (q->m_flags & 0x1000)
        q->m_post.Add(UnitQueryPostNearPosition);
    else if (q->m_flags & 0x4000)
        q->m_post.Add(UnitQueryPostAtPosition);

    if (q->m_selector != -1)
        q->m_post.Add(UnitQueryPostSelector);

    q->m_candidateCount = 0;

    //  Who actually receives each candidate.  A post-filtered query
    //  collects into the query itself and filters afterwards; a
    //  pre-filtered one collects plainly; a query with neither goes
    //  straight to the caller's own callback.
    UnitQueryVisitFn fn;
    void* ctx;
    if (q->m_post.m_count > 0)
    {
        fn = (UnitQueryVisitFn)UnitQueryCollectSorted;
        ctx = q;
    }
    else if (pre->m_count > 0)
    {
        fn = (UnitQueryVisitFn)UnitQueryCollectPlain;
        ctx = q;
    }
    else
    {
        fn = q->m_visit;
        ctx = q->m_context;
    }

    if (haveCircle)
        ((SUnitQueryCircleCentre*)circleAt)->EnumerateUnits(
                &origin, altForm, mask, 0, 0, fn, ctx);
    else if (haveRect)
        EnumerateUnitsInRect(&rectLo, &rectHi, &origin, altForm, mask, 0, 0, fn, ctx);
    else
        EnumerateUnitsOfType(GetUnitTypeFourCC(), fn, ctx, 0);

    //  The post-filter pass.  Two levels of break: a candidate stops at
    //  its first failing filter, and the whole walk stops as soon as the
    //  caller's callback says so.
    if (q->m_post.m_count > 0)
    {
        unsigned int total = q->m_candidateCount;
        int stop = 0;

        for (unsigned int i = 0; i < total && !stop; i++)
        {
            void* candidate = q->m_found.m_data[i];

            unsigned int filters = q->m_post.m_count;
            int rejected = 0;
            unsigned int j = 0;
            while (j < filters)
            {
                rejected = (q->m_post.m_data[j](candidate, q) == 0) ? 1 : 0;
                j++;
                if (rejected)
                    break;
            }

            if (j >= filters && !rejected)
                stop |= (q->m_visit(candidate, q->m_context) != 0) ? 1 : 0;
        }
    }

    g_unitQueryDepth -= 1;

    if (q->m_flags & 0x5000)
        return q->m_result;

    return 0;
}
