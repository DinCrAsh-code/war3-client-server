//============================================================================
//  0x6F308CD0 - CCameraWar3::RefreshHighlightCandidate, and its own qsort
//  comparator 0x6F3056F0.  Both close CCameraWar3::Update's (0x6F308E70)
//  candidate-rescan leaf.
//
//  RefreshHighlightCandidate walks m_agentListStorage (the already-typed
//  TSGrowableArray<CAgentPtr> at +0xBC - gameui.h) looking for the first
//  entry that is still a live, non-busy "pathable" handle - most frames a
//  cheap incremental scan resuming at m_highlightScanIndex+1, but once
//  every g_cameraUpdateDebounceBias-or-so seconds (the same shared bias
//  GameUI/cameracursorpreset.cpp's own UpdateCursorPresetPosition reads) a
//  full qsort re-rank first, so a stale highlight does not linger forever
//  behind a candidate the sort would have moved to the front.  Both loops
//  share one candidate test; the comparator's own version of it is one
//  condition lighter (no +0x5C busy-flag test - see IsPathableHighlightCandidate
//  vs IsUsableHighlightCandidate below), read straight off each function's
//  own dump rather than assumed to be the same helper.
//
//  **The candidate's handle/type pair (+0x0C/+0x10) and the resolved
//  object's own type-tag/busy pair (+0x0C/+0x20) are exactly
//  Widget/widgetpathref.cpp's already-established `LookupHandle` idiom**
//  (checked-cast-as-ternary, `setnz`/`sub 1`/`and` collapsing the null and
//  tag tests into one branch) - re-declared locally rather than shared,
//  the way every other TU in this repo that touches the same shape does,
//  since the two candidate structs here are a different object than
//  widgetpathref.cpp's own CWidget receiver.
//
//  **The winning candidate's own vtable slot +0x130 is reached through a
//  raw function-pointer cast, not through CWidget::GetMaxLife by name.**
//  The array is a TSGrowableArray<CAgentPtr>, so at this file's own type
//  boundary the elements are CAgent*, and CAgent's own vtable (24 slots,
//  ending at +0x5C - agent.h) does not reach +0x130 at all; the shipped
//  code plainly does reach a slot there on whatever the *dynamic* type
//  behind that handle really is (almost certainly a live CWidget-derived
//  unit/destructible, per widget.h's own GetMaxLife declaration at that
//  exact offset).  Reaching it through the raw vtable pointer instead of a
//  named virtual call sidesteps needing a complete type this array's own
//  storage does not carry, exactly the pattern CLAUDE.md's own
//  vtable_dispatch_audit example uses.
//
//  0x6F2781F0 is reached through this file's own thiscall-correct thunk
//  rather than through Misc/citemclosure_batchL_thunks.cpp's existing
//  `RefreshPositionDeltaState` declaration: that one is a plain untyped
//  `__stdcall(int,int)` placeholder (its own header comment says so), but
//  0x6F2781F0's own prologue opens with `mov ebx, ecx` - it reads a real
//  `this` in ecx that a stdcall call site would never set up.  Same
//  address, same `retn 8` (two stack dwords either way), a different,
//  locally-scoped signature - the established way this codebase handles
//  one shipped address reached two different ways (see refcnt.h's own
//  note on TRefCnt::Release vs DeleteSelf).  Genuinely out of this pass's
//  claimed addresses: 0x6F2781F0 and 0x6F3078B0's own SnapPanTargetToGround
//  already exist (GameUI/cameracursorpreset.cpp) and take it from here.
//============================================================================
#include "gameui.h"
#include "game.h"
#include "storm.h"
#include "CFloat.h"

extern "C" void __cdecl qsort(void* base, unsigned int num, unsigned int size,
                              int (__cdecl *cmp)(const void*, const void*));

//  dbl_6F875118 - GameUI/cameracursorpreset.cpp's own g_cameraUpdateDebounceBias,
//  read here as the same shared rebuild-interval constant.
extern const double g_cameraUpdateDebounceBias;

//  0x6F6C4E80 - GameUI/cameramiscleaves.cpp.
float __fastcall GetTickCountSeconds();

//  '+agl' MSB first - the agile type id every handle-registered object
//  carries (kAgentTagPathable, GameUI/gameuiagenthandle.h - not reused
//  directly since that header's own SAgentHandlePair does not carry this
//  file's +0x5C busy flag).
static const unsigned int kAgentTagPathable = 0x2B61676C;

namespace
{
    //  One array element, as this loop's own two field reads see it: the
    //  CAgent handle/type pair every LookupHandle call site in this repo
    //  shares, plus this file's own +0x5C busy-suppression flag.
    struct SHighlightCandidate
    {
        char         m_reserved00[0x0C];
        unsigned int m_handle;      // +0x0C
        int          m_handleType;  // +0x10
        char         m_reserved14[0x5C - 0x14];
        unsigned int m_flags;       // +0x5C, bit 0x100 = busy/suppressed
    };

    //  What LookupHandle hands back for one of these - the object's own
    //  type tag and a second "busy" flag, at the same +0x0C/+0x20 offsets
    //  Widget/widgetpathref.cpp's own SAgentHandleObject already uses.
    struct SResolvedAgentHandle
    {
        char         m_reserved00[0x0C];
        unsigned int m_typeTag;   // +0x0C, must equal kAgentTagPathable
        char         m_reserved10[0x10];
        unsigned int m_busy;      // +0x20, must be zero
    };

    typedef void (__thiscall *ReleaseSelfFn)(void*);
    typedef CFloat* (__thiscall *GetMaxLifeFn)(void*, CFloat*);

    //  0x6F2781F0, reached with a real `this` in ecx - see this file's own
    //  header comment on why this is not Misc/citemclosure_batchL_thunks.cpp's
    //  existing declaration of the same address.
    struct SPositionDeltaTarget
    {
        void RefreshPositionDeltaChecked(int outXYAddr, int mode);
    };
    __declspec(naked) void SPositionDeltaTarget::RefreshPositionDeltaChecked(int, int)
    {
        __asm { mov eax, 0x6F2781F0 }
        __asm { jmp eax }
    }
}

//  The tag test and the null test share one branch, the same
//  checked-cast-as-ternary shape widgetpathref.cpp's own AdjustWidgetPathRefs
//  already established for this exact LookupHandle/type-tag/busy triple.
static int IsPathableHighlightCandidate(void* obj)
{
    SHighlightCandidate* candidate = (SHighlightCandidate*)obj;
    SResolvedAgentHandle* agent = (SResolvedAgentHandle*)
        LookupHandle(candidate->m_handle, candidate->m_handleType);
    if (agent == 0)
        return 0;

    SResolvedAgentHandle* pathable =
        (agent->m_typeTag == kAgentTagPathable) ? agent : 0;
    if (pathable == 0)
        return 0;

    return pathable->m_busy == 0;
}

//  The scan/qsort loops' own candidate test: everything above, plus the
//  array element's own +0x5C busy-suppression bit.  0x6F3056F0's own
//  comparator does not carry this third condition - only these two loops
//  inside 0x6F308CD0 do.
static int IsUsableHighlightCandidate(void* obj)
{
    if (!IsPathableHighlightCandidate(obj))
        return 0;
    return (((SHighlightCandidate*)obj)->m_flags & 0x100) == 0;
}

//  The compiler's own "drop one reference, self-destruct once the count
//  hits zero" shape - slot 0 (TRefCnt::ReleaseSelf) through the raw
//  vtable pointer, exactly GameUI/escmenudtor.cpp's own ReleaseRefSlot.
static void ReleaseHighlightCandidateRef(void* obj)
{
    if (obj != 0 && --*(int*)((char*)obj + 4) == 0)
        ((ReleaseSelfFn)(*(void***)obj)[0])(obj);
}

//----------------------------------------------------------------------------
//  0x6F3056F0 - the qsort CompareFunction 0x6F308CD0's own full rebuild
//  passes: rank a still-live, non-busy "pathable" candidate ahead of one
//  that is not; between two live candidates, the one with the larger
//  GetMaxLife() first, then by raw handle as a last tiebreak.  A genuine
//  `__except_handler4`-shaped SEH frame (the classic cookie-XORed,
//  EBP-omitted shape with an appended scope-table trampoline) wraps the
//  whole body in the dump - unreproducible at this toolchain's fixed
//  `/GS- /EHs-c-` (docs/msvc-vc8-idioms.md, "An `__except_handler4`-shaped
//  frame this toolchain cannot reproduce") - so this scores well under
//  1.000 by construction; the body past that frame matches the dump one
//  AddRef/candidate-test/GetMaxLife/Release group at a time.  `retn 0`
//  (`__cdecl`, qsort's own convention).
//----------------------------------------------------------------------------
static int __cdecl CompareHighlightCandidates(const void* pa, const void* pb)
{
    void* a = *(void* const*)pa;
    void* b = *(void* const*)pb;

    if (a != 0) ++*(int*)((char*)a + 4);
    if (b != 0) ++*(int*)((char*)b + 4);

    int candA = (a != 0) && IsPathableHighlightCandidate(a);
    int candB = (b != 0) && IsPathableHighlightCandidate(b);

    int result;
    if (candA)
    {
        if (candB)
        {
            CFloat lifeA(kCFloatNoInit);
            CFloat lifeB(kCFloatNoInit);
            ((GetMaxLifeFn)(*(void***)a)[0x130 / 4])(a, &lifeA);
            ((GetMaxLifeFn)(*(void***)b)[0x130 / 4])(b, &lifeB);

            result = CFloatToInt(lifeB - lifeA);
            if (result == 0)
                result = *(int*)((char*)a + 0x10) - *(int*)((char*)b + 0x10);
        }
        else
        {
            result = -1;
        }
    }
    else if (candB)
    {
        result = 1;
    }
    else
    {
        result = 0;
    }

    ReleaseHighlightCandidateRef(b);
    ReleaseHighlightCandidateRef(a);
    return result;
}

//----------------------------------------------------------------------------
//  0x6F308CD0 - CCameraWar3::RefreshHighlightCandidate.  `retn 0`.
//----------------------------------------------------------------------------
void CCameraWar3::RefreshHighlightCandidate()
{
    TSGrowableArray<CAgentPtr>* list = (TSGrowableArray<CAgentPtr>*)m_agentListStorage;
    if (list->m_count == 0)
        return;

    int winner = -1;
    float now = GetTickCountSeconds();

    if (now < m_highlightRebuildTime + g_cameraUpdateDebounceBias)
    {
        //  Cheap path: resume scanning where the last call left off.
        unsigned int idx = (unsigned int)(m_highlightScanIndex + 1) % list->m_count;
        unsigned int remaining = list->m_count;
        if (remaining != 0)
        {
            do
            {
                void* obj = ((void**)list->m_data)[idx];
                remaining -= 1;
                if (obj != 0 && IsUsableHighlightCandidate(obj))
                {
                    winner = (int)idx;
                    break;
                }
                idx = (idx + 1) % list->m_count;
            } while (remaining != 0);
        }
    }
    else
    {
        //  The debounce interval elapsed: re-rank the whole array and take
        //  the first usable entry off the front.
        qsort(list->m_data, list->m_count, sizeof(void*), CompareHighlightCandidates);

        for (unsigned int i = 0; i < list->m_count; ++i)
        {
            void* obj = ((void**)list->m_data)[i];
            if (obj != 0 && IsUsableHighlightCandidate(obj))
            {
                winner = (int)i;
                break;
            }
        }
    }

    if ((unsigned int)winner < list->m_count)
    {
        m_highlightRebuildTime = now;
        m_highlightScanIndex = winner;

        float outXY[2];
        void* target = ((void**)list->m_data)[winner];
        ((SPositionDeltaTarget*)target)->RefreshPositionDeltaChecked((int)&outXY[0], 0);
        SnapPanTargetToGround(outXY[0], outXY[1]);
    }
}
