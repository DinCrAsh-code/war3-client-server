//============================================================================
//  0x6F423D40 - SPlayerSubgroupTarget::NotifyAsyncRefreshSubgroups.
//
//  What this build cannot reproduce is the frame: the same VC8 unified
//  `__try` frame two `TSGrowableArray<CAgentPtr>` locals pull in that
//  docs/msvc-vc8-idioms.md's own "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce" section documents, confirmed the same way -
//  a scope table appended after the function's own `retn` (6F82ACF0/
//  6F82ACF8 here), each entry `lea ecx, [ebp-N]` / `jmp` into the
//  destructor, then a shared `SecurityCheckCookie`/`__CxxFrameHandler3`
//  tail.  SPlayerHeroes::SelectHeroesByRank (playerheroselect.cpp) is the
//  established precedent for the same two-array shape and the same
//  ceiling; its own idioms (the CAgentPtr release/addref/store pattern,
//  the branchless checked-cast against 'agl+') are reused below rather
//  than re-derived.
//
//  Logs "Async refresh subgroups for player %d" (this+0x1AC) through the
//  same no-op debug-log stub (nullsub_16, 0x6F41F5E0) already established
//  under a *different*, zero-argument signature elsewhere
//  (Misc/trivialaccessors_04.cpp) - re-declared locally as the genuinely
//  variadic cdecl shape this call site actually pushes and cleans itself
//  (`add esp, N` after the call): a second, distinct C++ name for the one
//  address rather than a second definition (its body is a bare `retn`, so
//  any caller-cleanup declaration is equally safe for it).
//
//  Builds a `TSGrowableArray<CAgentPtr>` of some ability-related agent set
//  (0x6F421A80 - out of scope, see below), narrows it to the ones whose
//  agile type derives from the unit type FourCC into a second array via a
//  raw out-of-line "append a slot" call (0x6F0A4730 - its own established
//  C++ name is a *different* template instantiation,
//  `TSGrowableArray<SCheckedUnitSlot>::New`; almost certainly the
//  identical-code-folding docs/msvc-vc8-idioms.md's own "IDA can fold a
//  game function onto a CRT symbol" section documents for a same-sized,
//  structurally-identical instantiation, so it is called here by raw
//  address rather than through that unrelated name), sorts/dispatches the
//  first array through a generic callback-driven helper (0x6F4C6210,
//  callback 0x6F4201D0 - a whole further, unidentified subsystem),
//  notifies each surviving second-array entry (0x6F423050), and - only
//  when `flag` is set - fires the two network command constructors
//  (0x6F2CC2E0/0x6F2CC240, CNetCommandUnitRefreshSubGroup/
//  CNetCommandUnitSelectSubGroup - their own names read straight off the
//  vftable symbols their own SEH-wrapped bodies stamp), validates a
//  second, `LookupHandle`-resolved object purely as a gate (its own
//  `m_kindTag`/`m_pendingFlag` check, the same branchless checked-cast
//  idiom `AgileTypeIsDerivedFrom`'s own callers already use), and
//  refreshes this object's own cached agent reference (+0x1D8/+0x1DC)
//  from the field-walker head object PeekFieldWalkerHead (0x6F421E50,
//  already established) hands back - not from the `LookupHandle` object,
//  which is read only for its gate.
//
//  Every callee this function's own body reaches that is not already
//  established elsewhere (0x6F421A80, 0x6F0A4730 by raw address, 0x6F4C6210,
//  0x6F423500, 0x6F423050, 0x6F2CC2E0, 0x6F423970, 0x6F2CC240) is a whole
//  further, unidentified subsystem two levels past this bounded target and
//  stays a documented naked redirect - the "genuinely unbounded, unrelated"
//  boundary this session's own instructions say to stop at.  Argument
//  counts and registers for each are read off this target's own dump
//  (thunk_abi_audit.py's own question), not guessed from the declaration.
//============================================================================
#include "unitsubgrouprefresh.h"
#include "itemhandleresolve.h"
#include "storm.h"

//----------------------------------------------------------------------------
//  0x6F421E50/0x6F26C1C0/AgileTypeIsDerivedFrom - already established;
//  re-declared locally (not redefined) the same way every other reader of
//  each does.
//----------------------------------------------------------------------------
struct SFieldWalkerHolder;
int __fastcall PeekFieldWalkerHead(SFieldWalkerHolder* self);
unsigned int GetUnitTypeFourCC();

//----------------------------------------------------------------------------
//  0x6F41F5E0 - nullsub_16/NoOp_6F41F5E0's own address, called here with a
//  genuinely variadic cdecl shape.
//----------------------------------------------------------------------------
extern "C" void __cdecl DebugLogStub(int level, const char* fmt, ...);

__declspec(naked) void __cdecl DebugLogStub(int, const char*, ...)
{
    __asm
    {
        mov     eax, 06F41F5E0h
        jmp     eax
    }
}

static const char aAsyncRefreshSu[] = "Async refresh subgroups for player %d";

//  dword_6FAB6E80 - a reentrancy-style flag this function sets around its
//  own network-command construction; not otherwise identified/used
//  elsewhere in this call tree.  funcmap.DATA names it
//  g_subgroupNotifyInProgress.
extern int g_subgroupNotifyInProgress;

//  AGENT_SLOT/CAgentPtr's own kind tag, already established the same way
//  in playerheroselect.cpp.
const int kAgentKindTagAgl = 0x2B61676C;

//----------------------------------------------------------------------------
//  The out-of-scope callees.  Each is declared only as precisely as its
//  call site's own register/stack shape requires - see the file header
//  for why none of these is reconstructed.
//----------------------------------------------------------------------------

//  0x6F421A80 - fills a TSGrowableArray<CAgentPtr> from `this`.  `retn 4`.
//  Real reconstruction now (Player/playerabilityagenttracker.cpp, from the
//  PlayerRecordChecksumHash batch, which reaches this same address from a
//  second, unrelated receiver) - declared here, not redefined, so both
//  callers agree on one mangled symbol.
struct SAbilityAgentCollector
{
    void CollectAgents(TSGrowableArray<CAgentPtr>* out);
};

//  0x6F0A4730 - append-a-slot, called by raw address (see file header for
//  why not through its own established, differently-typed C++ name).
//  thiscall(this = &array), no stack arguments, retn 0 - the same shape
//  its own established declaration (`SCheckedUnitSlot* TSGrowableArray<
//  SCheckedUnitSlot>::New()`, Containers/unitagentptrarray.cpp) has.
typedef CAgentPtr* (__thiscall *AppendSlotFn)(TSGrowableArray<CAgentPtr>* self);
static AppendSlotFn const AppendCandidateSlot = (AppendSlotFn)0x6F0A4730;

//  0x6F4C6210 - a generic callback-driven array walk (sort, or similar):
//  ecx = data pointer, edx = count, stack (elemSize, callback), cleaning
//  the two stack dwords itself; `retn 8`.
void __fastcall WalkArray(void* data, unsigned int count,
                          unsigned int elemSize, void* callback);
__declspec(naked) void __fastcall WalkArray(void*, unsigned int, unsigned int, void*)
{
    __asm
    {
        mov     eax, 06F4C6210h
        jmp     eax
    }
}

//  0x6F4201D0 - the walk's own callback.  Never called from here directly
//  (only its address is taken), so it needs no declared signature at all.
static void* const kArrayWalkCallback = (void*)0x6F4201D0;

//  0x6F423500 - this call tree's own per-candidate predicate.  `retn 8`.
struct SAbilityAgentPredicate
{
    int CheckAgent(void* agent, int extra);
};
__declspec(naked) int SAbilityAgentPredicate::CheckAgent(void*, int)
{
    __asm
    {
        mov     eax, 06F423500h
        jmp     eax
    }
}

//  0x6F423050 - per-surviving-agent notify.  `retn 4`.
struct SAbilityAgentNotify
{
    void NotifyAgent(void* agent);
};
__declspec(naked) void SAbilityAgentNotify::NotifyAgent(void*)
{
    __asm
    {
        mov     eax, 06F423050h
        jmp     eax
    }
}

//  0x6F2CC2E0 - construct and send a CNetCommandUnitRefreshSubGroup.  No
//  arguments of its own (it builds and stamps a local object internally);
//  `retn 0`.
__declspec(naked) void ConstructAndSendUnitRefreshSubGroup()
{
    __asm
    {
        mov     eax, 06F2CC2E0h
        jmp     eax
    }
}

//  0x6F423970 - a second per-target step, argument the field-walker head
//  result taken earlier.  `retn 4`.
struct SAbilitySecondaryNotify
{
    void NotifySecondary(void* walkerHead);
};
__declspec(naked) void SAbilitySecondaryNotify::NotifySecondary(void*)
{
    __asm
    {
        mov     eax, 06F423970h
        jmp     eax
    }
}

//  0x6F2CC240 - construct and send a CNetCommandUnitSelectSubGroup.
//  __fastcall(ecx, edx), no stack arguments; `retn 0`.
void __fastcall ConstructAndSendUnitSelectSubGroup(void* a, void* b);
__declspec(naked) void __fastcall ConstructAndSendUnitSelectSubGroup(void*, void*)
{
    __asm
    {
        mov     eax, 06F2CC240h
        jmp     eax
    }
}

//  0x6F03FA30 - LookupHandle, already established.
CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag);

void SPlayerSubgroupTarget::NotifyAsyncRefreshSubgroups(int flag)
{
    DebugLogStub(1, aAsyncRefreshSu, m_playerIndex);

    TSGrowableArray<CAgentPtr> candidates;
    candidates.m_alloc = 0;
    candidates.m_count = 0;
    candidates.m_data = 0;
    TSGrowableArray<CAgentPtr> matched;
    matched.m_alloc = 0;
    matched.m_count = 0;
    matched.m_data = 0;

    ((SAbilityAgentCollector*)this)->CollectAgents(&candidates);

    unsigned int candidateCount = candidates.m_count;
    int walkerHead = PeekFieldWalkerHead((SFieldWalkerHolder*)this);

    WalkArray(candidates.m_data, candidateCount, 4, kArrayWalkCallback);

    for (unsigned int i = 0; i < candidateCount; ++i)
    {
        CAgentPtr* slot = &candidates.m_data[i];
        CAgent* agent = slot->m_ptr;

        if (!((SAbilityAgentPredicate*)this)->CheckAgent(agent, 0))
            continue;

        CAgentPtr* dst = AppendCandidateSlot(&matched);
        agent = slot->m_ptr;

        CAgent* keep = 0;
        if (agent != 0
            && AgileTypeIsDerivedFrom(agent->GetAgileTypeId(), GetUnitTypeFourCC()))
        {
            keep = agent;
        }

        if (dst->m_ptr != keep)
        {
            if (dst->m_ptr != 0)
            {
                if (--dst->m_ptr->m_refcount == 0)
                    dst->m_ptr->ReleaseSelf();
            }
            if (keep != 0)
                agent->m_refcount++;
            dst->m_ptr = keep;
        }
    }

    for (unsigned int i = 0; i < matched.m_count; ++i)
        ((SAbilityAgentNotify*)this)->NotifyAgent(matched.m_data[i].m_ptr);

    g_subgroupNotifyInProgress = 1;

    if (flag)
        ConstructAndSendUnitRefreshSubGroup();

    g_subgroupNotifyInProgress = 0;

    ((SAbilitySecondaryNotify*)this)->NotifySecondary((void*)walkerHead);

    if (flag)
    {
        SFieldWalkerHolder* head = (SFieldWalkerHolder*)PeekFieldWalkerHead((SFieldWalkerHolder*)this);
        if (head)
        {
            CAgent* headAgent = (CAgent*)head;

            SOptionalHandleRef* ref = (SOptionalHandleRef*)((char*)head + 0xC);
            CHandleObject* resolved = LookupHandle(ref->m_handle, ref->m_typeTag);

            int gateOk = 0;
            if (resolved != 0)
            {
                int isAgl = (resolved->m_kindTag == kAgentKindTagAgl);
                SItemHandleObject* checked = isAgl ? (SItemHandleObject*)resolved : 0;
                gateOk = (checked != 0 && checked->m_pendingFlag == 0);
            }

            if (gateOk)
            {
                void* field30 = *(void**)((char*)head + 0x30);
                ConstructAndSendUnitSelectSubGroup(field30, head);

                CAgent* keep = 0;
                if (AgileTypeIsDerivedFrom(headAgent->GetAgileTypeId(), GetUnitTypeFourCC()))
                    keep = headAgent;

                if (m_cachedAgent.m_ptr != keep)
                {
                    if (m_cachedAgent.m_ptr != 0)
                    {
                        if (--m_cachedAgent.m_ptr->m_refcount == 0)
                            m_cachedAgent.m_ptr->ReleaseSelf();
                    }
                    if (keep != 0)
                        headAgent->m_refcount++;
                    m_cachedAgent.m_ptr = keep;
                }

                m_cachedField30 = *(void**)((char*)head + 0x30);
            }
        }
    }
}
