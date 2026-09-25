//============================================================================
//  0x6F46DCE0 - 0x6F4815A0 - one contiguous shipped module, confirmed by its
//  own embedded debug-tag string at 0x480188 (`aCagentbaseCpp`,
//  ".\\CAgentBase.cpp") inside sub_6F480150 below: this whole address range
//  is literally CAgentBase.cpp, right next to NIpse::CRlAgent's own ctor/dtor
//  (Pathfinding/crlagent_ctor.cpp/crlagent_dtor.cpp, 0x6F4801E0/0x6F4803B0,
//  sitting *between* two of this file's own functions) - so CRlAgent's own
//  implementation and CAgentBaseAbs's own vtable slots were shipped in the
//  same translation unit.  Kept as one TU here too, matching CLAUDE.md's
//  "one translation unit per original module" rule: every function below
//  calls at least one sibling in this same file (see the call graph in
//  asm/CAgentBaseAbs_vtable_remaining_calltree_asm.md), so none of them may
//  be split out where they could get inlined into each other.
//
//  ---------------------------------------------------------------------
//  The central finding: CAgentBaseAbs's own +0x54 field is a live
//  `NIpse::CAgent`-hierarchy... no, plain `CAgent*` "delegate" pointer.
//  ---------------------------------------------------------------------
//  Four of this file's own vtable-slot bodies (slots 11/13/14/16 below) each
//  read `this+0x54` and dispatch through *its* vtable at one of the exact
//  slot offsets Agent/agent.h's own `CAgent` vtable block already documents
//  (index 14 / +0x38 Save, index 15 / +0x3C Load, index 16 / +0x40
//  RefreshOwningPlayerColor, index 17 / +0x44 DumpState) - four different
//  slots, each landing on a real, independently-reconstructed `CAgent`
//  member, which is far too much agreement to be coincidence.  slot 0x38's
//  own resolve step (0x480510 below) even LookupHandle()s the delegate's own
//  +0x0C/+0x10 pair - CAgent's own `m_handle`/`m_typeTag` (agent.h) - and
//  checks the resolved object's kind tag against the exact same
//  `kAgentTagPathable` ('+agl', 0x2B61676C) constant Widget/widgetpathref.cpp
//  and a dozen siblings already use for a `CAgent`.  So `this+0x54` is typed
//  here as a real `CAgent*`, not a second "Raw" placeholder - Agent/agent.h
//  already carries its full vtable and this file leans on it directly.
//
//  Also established here: Agent/agenteventrouter.cpp's own `SAgentEventRouter`
//  (0x6F47FD00 - CAgentBaseAbs's own vtable slot 8) is the *same* class as
//  this file's `CAgentBaseAbs`, and its own "+0x54 sub-object, vtable slot
//  0x18" is this exact delegate, called at `CAgent`'s own slot 6
//  (Method_0x18) - a fifth independent confirmation of the same field.
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include "game.h"
#include "storm.h"
#include "crlagent.h"
#include "agentrelation.h"

//  Storm.dll ordinal 501 (SStrCopy) - declared locally rather than via
//  "os.h" to avoid that header's own <windows.h> include, whose WinUser.h
//  `#define GetClassName GetClassNameA/W` would otherwise rewrite
//  CAgent::GetClassName below into a name that does not exist - the same
//  declare-locally shape most other Storm_501 call sites in this codebase
//  already use (e.g. GameUI/chatfontname.cpp).
void __stdcall Storm_501(char* dest, const char* src, unsigned int maxLen);

//  '+agl' MSB first - see Widget/widgetpathref.cpp's own copy of this
//  constant (kAgentTagPathable) for the full note; declared again locally
//  per this codebase's own convention for a constant every reader keeps its
//  own copy of rather than sharing one header.
static const unsigned int kAgentTagPathable = 0x2B61676C;

//  0x6F46DCE0 - AgentBase_InvokeTypePoolSlot0 - own translation unit
//  (Agent/cagentbaseabs_typepoolslot0.cpp), reached only from
//  Agent/cagentbaseabs_binddelegate.cpp's own `BindDelegateFromField10`; not
//  used anywhere in this file.  Kept in its own TU rather than folded in
//  here because this compiler will happily inline a same-TU one-liner at
//  /Ob2, which the shipped dump's own real `call sub_6F46DCE0` shows did
//  not happen for the real binary - CLAUDE.md's own house rule ("put a
//  callee in its own TU rather than reaching for __declspec(noinline)").

//  0x6F471F90 - CAgentDelegateRaw::BindSourceHandle - own translation unit
//  (Agent/cagentbaseabs_delegateraw.cpp), same inlining reason as above:
//  ForwardLoadToDelegate below calls it once and the shipped dump's own
//  `call sub_6F471F90` proves it stayed out of line there too.  Declared
//  only (no definition visible here), so nothing in this file can inline it.
struct CAgentDelegateRaw
{
    void* BindSourceHandle(void* source);
};

//----------------------------------------------------------------------------
//  0x6F480150 - lazily allocate CAgentBaseAbs's own static free-list pool
//  (parked at `g_pGameData`'s own +0x34, a byte range game.h's `CGameData`
//  still carries as part of its `m_reserved34` block - read/written here by
//  raw offset rather than by editing that shared header's own field list out
//  from under whichever session eventually claims +0x34 for real): a 0x20
//  byte header (Storm_401/SMemAlloc, tagged ".\\CAgentBase.cpp" line 0x1D)
//  whose first 0x14 bytes an out-of-scope helper (sub_6F4C1A10 - not in this
//  session's own 15 addresses, and no reconstruction of it exists anywhere
//  in src/ yet) initializes as a fixed-block allocator of 0xBC-byte elements
//  (0xB8, CRlAgent's own established total size from crlagent.h, plus the
//  4-byte intrusive free-list link slot 0x4804C0 below pushes onto - see
//  that function's own comment) with capacity 0x200, followed by three more
//  explicit zero stores this function makes itself (+0x14 free-list head,
//  +0x18 free count, +0x1C reserved).
//
//  Carries the classic cookie-XORed, frame-pointer-omitted
//  `__CxxFrameHandler3` SEH frame (`push -1` / `push offset SEH_6F480150` /
//  `mov eax, fs:[0]` ... `dword_6FAAE140 ^ esp`, with a scope table tail
//  calling `nullsub_45` for an empty `__finally` and `sub_6F7E1059`
//  (SecurityCheckCookie) for its own outer cookie check) - the same
//  `__except_handler4`-shaped frame docs/msvc-vc8-idioms.md's "An
//  `__except_handler4`-shaped frame this toolchain cannot reproduce" already
//  documents for two unrelated functions in this exact binary, built at this
//  fixed `/GS- /EHs-c-` toolchain: not reproducible byte-for-byte here, so
//  the logic below matches the dump past the frame furniture and the
//  function is filed `DIFFERS`, not `EXACT`.
//----------------------------------------------------------------------------

//  The pool header's own shape, named only as far as this function writes
//  it - the first 0x14 bytes belong to the out-of-scope allocator
//  initializer above and are left an explicit reserved block.
struct SAgentBasePool
{
    char            m_reserved00[0x14];
    void*           m_freeListHead;      // 0x14
    unsigned int    m_freeCount;         // 0x18
    unsigned int    m_reserved1C;        // 0x1C
};

//  0x6F4C1A10 - a parallel session's own reconstruction landed while this
//  batch was in progress (Agent/agentbasefixedpoolheader.cpp -
//  `SFixedPoolHeader::SFixedPoolHeader(elementSize, capacity)`), so this
//  now goes through a real placement-new call to its own declared
//  constructor rather than the raw function-pointer cast this file used
//  before that address had a name - see that file's own header for the
//  clamp-to-minimum behaviour it establishes (elementSize>=4, capacity>=1).
struct SFixedPoolHeader
{
    SFixedPoolHeader(unsigned int elementSize, unsigned int capacity);

    unsigned int m_elementSize;    // +0x00
    unsigned int m_capacity;       // +0x04
    unsigned int m_reserved08;     // +0x08
    unsigned int m_reserved0C;     // +0x0C
    unsigned int m_reserved10;     // +0x10
};

void* AgentBase_GetOrCreatePool()
{
    void** slot = (void**)((char*)g_pGameData + 0x34);
    void* pool = *slot;
    if (pool == 0)
    {
        pool = SMemAlloc(0x20, ".\\CAgentBase.cpp", 0x1D, 0);
        if (pool != 0)
        {
            new (pool) SFixedPoolHeader(0xBC, 0x200);
            ((SAgentBasePool*)pool)->m_freeListHead = 0;
            ((SAgentBasePool*)pool)->m_freeCount = 0;
            ((SAgentBasePool*)pool)->m_reserved1C = 0;
        }
        *slot = pool;
    }
    return pool;
}

//----------------------------------------------------------------------------
//  CAgentBaseAbs - a partial, address-only interface (RTTI-confirmed
//  vtable at 0x6F9520D4, agent_worktrees/classes/0x6F9520D4.json): every
//  member below is one of this class's own vtable slots, named
//  `CAgentBaseAbs::` per this batch's own assignment even though the class's
//  full field layout (shared with NIpse::CRlAgent below it, Pathfinding/
//  crlagent.h - under separate, parallel reconstruction) is not modelled
//  here.  Every field this file touches is reached by raw offset instead,
//  the same "declare only what this address needs" shape
//  Agent/agentbaseabscomputechecksum.cpp's own `CAgentRelation` (formerly
//  `CAgentBaseAbsRaw`, before this session's vtable-derived rename showed
//  slot 15 - 0x6F4A60A0 - is actually NIpse::CPrRelation's own override,
//  not this class's) already uses.
//----------------------------------------------------------------------------
class CAgentBaseAbs
{
public:
    //  slot 0 / +0x00 (0x6F47FDF0)
    const char* GetDebugName();
    //  slot 1 / +0x04 (0x6F4804C0)
    void ReturnToStaticPool();
    //  slot 2 / +0x08 (0x6F4805E0) - scalar deleting destructor
    void* ScalarDeletingDestructor(unsigned int flags);
    //  slot 11 / +0x2C (0x6F47FDB0)
    void ForwardSaveToDelegate(CDataStoreScratch* store);
    //  slot 13 / +0x34 (0x6F47FDD0)
    void ForwardLoadToDelegate(CDataStore* store);
    //  slot 14 / +0x38 (0x6F480510)
    void RefreshDelegateOwningColor();
    //  slot 16 / +0x40 (0x6F47FE50)
    void ForwardDumpStateToDelegate(void* sink);

    //  0x6F47FCE0 - not itself a vtable slot (no `### ... (root)` heading in
    //  this batch's own dump section - it is a plain callee, reached only
    //  from BindDelegateFromField10 below), but it genuinely is one of
    //  CAgentBaseAbs's own members: sets this object's own +0x54 delegate
    //  pointer, add-refing the new delegate's refcount (+0x04, the same
    //  `TRefCnt`/`CObserver` slot agent.h's own note on CAgent's refcount
    //  already places it at) when non-null.  No release of whatever was
    //  there before - the shipped body genuinely has none, matching this
    //  class's own construction-time default of a null delegate (nothing
    //  else in this batch's own closure ever calls this a second time on
    //  the same object).
    void SetDelegate(CAgent* newDelegate);

    //  0x6F47FC50 - not itself a vtable slot in this session's own dump
    //  either (no caller in this batch's own call graph - most likely
    //  reached from CPrRelation's or CAgentBaseAbs's own real destructor,
    //  both under separate/parallel reconstruction): restamp `this`'s
    //  vtable pointer straight to NTempest::CPresence's own vtable
    //  (`??_7CPresence@NTempest@@6B@`, 0x6F951CA0) and return - the exact
    //  same single store Pathfinding/crlagent_dtor.cpp's own `~CRlAgent()`
    //  already makes inline for its own final restamp, just as its own
    //  out-of-line, separately-addressed function here.
    void RestampToPresenceVtable();
};

//  0x6F47FC50
void CAgentBaseAbs::RestampToPresenceVtable()
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@
}

//  0x6F47FCE0 - the shipped body genuinely duplicates the tail
//  "store newDelegate into +0x54, retn 4" pair on both sides of the null
//  test rather than falling through to one shared copy (`jnz` skips
//  forward to the non-null branch, which repeats the same store/return) -
//  the same "two early exits with identical tails" shape a natural
//  `if (x) { ...; return; }` set gives, and a natural if/else with a
//  shared fallthrough does not.
void CAgentBaseAbs::SetDelegate(CAgent* newDelegate)
{
    if (newDelegate == 0)
    {
        *(CAgent**)((char*)this + 0x54) = newDelegate;
        return;
    }

    *(int*)((char*)newDelegate + 4) += 1;
    *(CAgent**)((char*)this + 0x54) = newDelegate;
}

//  slot 0 / +0x00 (0x6F47FDF0) - "GetDebugName": if this object has a live
//  delegate (+0x54), defer to its own GetClassName (CAgent's own vtable
//  slot 22, +0x58, agent.h); otherwise format this object's own +0x10
//  FourCC-shaped type tag into a shared, module-local scratch buffer
//  (byte-swapped - PackFourCCString/UnpackFourCCString, Net/packfourcc.cpp,
//  already establish this binary's FourCC values are stored MSB-first, so a
//  little-endian dword holding one needs its bytes reversed before it reads
//  as text) and return that instead.  The destination the shipped code
//  copies into (`unk_6FAB73D0`) has no `dword_`/`byte_`-prefixed IDA name,
//  so - matching Agent/lazysingleton.cpp's own `unk_6F876330` precedent -
//  funcmap.py's binder would not bind an `extern` for it correctly; a real
//  local definition is used instead, which does not change the generated
//  code (an unresolved data-symbol operand canonicalises to `SYM` on both
//  sides regardless of which name or bytes back it, matching every other
//  string-literal/vtable-symbol operand this codebase already treats that
//  way).
static char g_agentBaseDebugNameBuf[8];

const char* CAgentBaseAbs::GetDebugName()
{
    unsigned int v = *(unsigned int*)((char*)this + 0x10);
    CAgent* delegate = *(CAgent**)((char*)this + 0x54);

    unsigned int swapped = ((v & 0xFF0000) | (v >> 16));
    unsigned int lo = ((v & 0xFF00) | (v << 16));
    swapped = (swapped >> 8) | (lo << 8);

    Storm_501(g_agentBaseDebugNameBuf, (const char*)&swapped, 5);

    if (delegate != 0)
        return delegate->GetClassName();

    return g_agentBaseDebugNameBuf;
}

//  slot 16 / +0x40 (0x6F47FE50): dump `this` object's own relation state
//  (`CAgentRelation::DumpDebugState`, 0x6F4A62D0,
//  Agent/agentbaseabsdebugdump.cpp - a parallel session's own reconstruction
//  of the same address this file used to reach only by raw cast; a real,
//  named call now that it exists) followed by forwarding to the delegate's
//  own DumpState (CAgent's own vtable slot 17, +0x44, agent.h) when a
//  delegate exists.  `CAgentRelation` here is `this` itself, not the +0x54
//  delegate: CAgentBaseAbs derives from NIpse::CPrRelation at `mdisp:0`
//  (docs/targets/vtables/CAgentBaseAbs.txt's own hierarchy), and
//  `CAgentRelation` is that same 0x6F4A5xxx-0x6F4A6xxx "relation" object
//  under its other investigation's own name (agentrelation.h) - no pointer
//  adjustment needed either way.
void CAgentBaseAbs::ForwardDumpStateToDelegate(void* sink)
{
    ((CAgentRelation*)this)->DumpDebugState(sink);

    CAgent* delegate = *(CAgent**)((char*)this + 0x54);
    if (delegate != 0)
        delegate->DumpState(sink);
}

//  slot 11 / +0x2C (0x6F47FDB0): a bare tail-jmp into the delegate's own
//  Save (CAgent's own vtable slot 14, +0x38, agent.h) - CAgentBaseAbs's own
//  slot 11 forwards unconditionally, with no null check (unlike slots 14/16
//  above), matching the shipped body's own unconditional
//  `mov ecx,[ecx+54h] / jmp [[ecx]+38h]`.
void CAgentBaseAbs::ForwardSaveToDelegate(CDataStoreScratch* store)
{
    CAgent* delegate = *(CAgent**)((char*)this + 0x54);
    delegate->Save(store);
}

//  slot 13 / +0x34 (0x6F47FDD0): bind the delegate's own handle pair from
//  `this` (CAgentDelegateRaw::BindSourceHandle above, `this` as the
//  `source`), then forward to the delegate's own Load (CAgent's own vtable
//  slot 15, +0x3C, agent.h).
void CAgentBaseAbs::ForwardLoadToDelegate(CDataStore* store)
{
    CAgent* delegate = *(CAgent**)((char*)this + 0x54);
    ((CAgentDelegateRaw*)delegate)->BindSourceHandle(this);
    delegate->Load(store);
}

//  slot 14 / +0x38 (0x6F480510): resolve the delegate's own registered
//  handle (its +0x0C/+0x10 `m_handle`/`m_typeTag` pair, agent.h) through
//  LookupHandle, and only if that resolves to a live, non-pending '+agl'
//  object, forward to the delegate's own RefreshOwningPlayerColor (CAgent's
//  own vtable slot 16, +0x40, agent.h).  Reuses game.h's own
//  `SItemHandleObject` shape for the resolved object purely because it
//  already carries the exact two fields this needs (`m_kindTag` at +0x0C,
//  a "pending" flag at +0x20) at the right offsets - not a claim that the
//  resolved object is really an item handle, the same "whatever
//  LookupHandle returns" genericity Widget/widgetpathref.cpp's own
//  `SAgentHandleObject` already uses for the identical two-field check.
void CAgentBaseAbs::RefreshDelegateOwningColor()
{
    CAgent* delegate = *(CAgent**)((char*)this + 0x54);

    SItemHandleObject* resolved = (SItemHandleObject*)
        LookupHandle(delegate->m_handle, (int)delegate->m_typeTag);
    if (resolved == 0)
        return;

    //  The tag test and its own null-out are one branch in the shipped code
    //  (`setnz`/`sub 1`/`and` turns "tag matched" into an all-ones mask and
    //  ANDs the pointer with it) - the same checked-cast-as-conditional-
    //  expression shape Widget/widgetpathref.cpp's own `AdjustWidgetPathRefs`
    //  already documents for the identical LookupHandle+kAgentTagPathable
    //  check, kept as two separate `if`s here to match: the plain null
    //  check above is genuinely a second, earlier branch in the dump, not
    //  folded into this one.
    SItemHandleObject* pathable =
        (resolved->m_kindTag == (int)kAgentTagPathable) ? resolved : 0;
    if (pathable == 0)
        return;
    if (pathable->m_pendingFlag != 0)
        return;

    delegate->RefreshOwningPlayerColor();
}

//  slot 1 / +0x04 (0x6F4804C0): unlink `this` from a doubly-linked list of
//  live instances (own +0x04/+0x08 prev/next pair - a second, separate list
//  from the CLinkedNode-shaped +0x2C/+0x3C empty-list head/tail
//  crlagent.h's own file header already establishes for the base chain),
//  then push `this` back onto CAgentBaseAbs's own static free-list pool
//  (AgentBase_GetOrCreatePool above) - the intrusive link slot lives 4 bytes
//  *before* the object itself (`this-4`), matching the pool's own 0xBC
//  element size being exactly 4 bytes more than CRlAgent's established
//  0xB8 total object size.
void CAgentBaseAbs::ReturnToStaticPool()
{
    void* pool = AgentBase_GetOrCreatePool();

    char* self = (char*)this;
    void* prev = *(void**)(self + 4);
    void* next = *(void**)(self + 8);
    if (prev != 0)
        *(void**)((char*)prev + 8) = next;
    if (next != 0)
        *(void**)((char*)next + 4) = prev;
    *(void**)(self + 4) = 0;
    *(void**)(self + 8) = 0;

    SAgentBasePool* p = (SAgentBasePool*)pool;
    void* node = self - 4;
    *(void**)node = p->m_freeListHead;
    p->m_freeCount -= 1;
    p->m_freeListHead = node;
}

//  slot 2 / +0x08 (0x6F4805E0): the classic MSVC scalar deleting destructor
//  shape - tear down through the already-reconstructed base chain
//  (Pathfinding/crlagent_dtor.cpp's `NIpse::CRlAgent::~CRlAgent()`, which
//  this class adds no further sub-object past - its own destructor makes no
//  other call before this one), then free the object's own storage through
//  Storm_403/SMemFree, tagged "delete", when bit 0 of the `flags` argument
//  is set (the standard "delete this too" flag a scalar deleting destructor
//  takes).  Returns `this`.
void* CAgentBaseAbs::ScalarDeletingDestructor(unsigned int flags)
{
    ((NIpse::CRlAgent*)this)->~CRlAgent();

    if (flags & 1)
    {
        if (this != 0)
            SMemFree(this, "delete", -1, 0);
    }

    return this;
}

//----------------------------------------------------------------------------
//  0x6F4815A0 - `this` (ecx) is `g_pTimeSync` at every real call site this
//  session found (both from Agent/agentbaseabsnotify.cpp, a parallel
//  session's own reconstruction of two of this function's callers,
//  0x6F4A4DF0/0x6F4A4E40 - CAgentBaseAbs's own still-unclaimed vtable slot
//  4/17 roots): it fetches a callback stored at g_pTimeSync's own +0x254 and
//  invokes it `__fastcall`-shaped (`code` in ecx, `context` in edx,
//  `subjectId` on the stack) with the caller's own three stack arguments
//  verbatim, none of which is `this` itself.  `SGlobalAgentNotifyHost` and
//  this exact signature are that file's own name for this address - defined
//  here for real rather than left as its own forward declaration, so both
//  files bind to the one symbol instead of two never-linking copies of the
//  same address (docs/notes/thunk-removal-pass.md's "one shipped address
//  carrying two C++ names" defect).
//----------------------------------------------------------------------------
typedef void (__fastcall *AgentNotifyCallbackFn)(unsigned int code,
                                                  const void* context,
                                                  unsigned int subjectId);

struct SGlobalAgentNotifyHost
{
    void Notify_6F4815A0(unsigned int code, const void* context,
                         unsigned int subjectId);
};

void SGlobalAgentNotifyHost::Notify_6F4815A0(unsigned int code,
                                             const void* context,
                                             unsigned int subjectId)
{
    AgentNotifyCallbackFn fn = *(AgentNotifyCallbackFn*)((char*)this + 0x254);
    if (fn != 0)
        fn(code, context, subjectId);
}
