//============================================================================
//  0x6F266D00 - CAgentWar3::~CAgentWar3().  See agentwar3.h.
//
//  Own translation unit: reached only as an exception-unwind funclet
//  target from CItem__Constructor's own SEH table (0x6F2B7520), a
//  different address neighbourhood (0x6F266xxx, CSelectable/CItem's own
//  cluster) from agent.cpp's 0x6F4Cxxxx or agent_ctor.cpp/agent_dtor.cpp's
//  0x6F001xxx.
//
//  Trivial: reset the vtable pointer to CAgentWar3's own (the standard
//  MSVC-generated-destructor shape, same as TRefCnt::~TRefCnt(), refcnt.h,
//  now defined inline there) and tail-call straight into the base class
//  destructor, CAgent::~CAgent() - there is nothing else to unwind since
//  CAgentWar3 itself owns no fields.
//============================================================================
#include "agentwar3.h"
#include "../Task/task.h"
#include "../Task/taskparam.h"
#include "../Task/taskrotate.h"
#include "../Task/taskaction.h"
#include "../Task/taskorderparam.h"
#include "../Task/taskghost.h"

CAgentWar3::~CAgentWar3()
{
}

//============================================================================
//  0x6F267180 / 0x6F2671B0 - CWidget's and CSelectable's vtable slot 1: the
//  scalar deleting destructors.
//
//  They live *here*, in ~CAgentWar3's own translation unit, and not in
//  widget_vtable_leaves.cpp/selectable_vtable.cpp, for one reason: the
//  shipped code has ~CAgentWar3 *inlined* into both of them (a single
//  `mov [esi], offset ??_7CAgentWar3@@6B@` followed by a real
//  `call sub_6F0017F0`), and MSVC can only inline it where its definition
//  is visible.  Out of this file the pair would compile to a single
//  `call ??1CAgentWar3@@QAE@XZ` instead - correct, but one instruction
//  short of the shipped shape - while ~CAgentWar3 itself still has to keep
//  its own out-of-line body at 0x6F266D00 for the unwind funclet that
//  reaches it.  Defining it non-inline in this file gives both.
//
//  CWidget's and CSelectable's own destructors contribute nothing visible:
//  each would reset the vtable pointer to its own class's table and is then
//  immediately overwritten by the base's, so /O2 drops the dead stores and
//  the one surviving store is CAgentWar3's.  That is the same adjacent-
//  store elision agentwar3.h's header comment already describes, and it is
//  why two classes' deleting destructors come out byte-identical here -
//  and, since this image was linked without /OPT:ICF (agentdefaults.cpp),
//  why they still occupy two separate addresses.
//============================================================================
#include "selectable.h"
#include "war3image.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

//----------------------------------------------------------------------------
//  0x6F267180 - CWidget's slot 1.  Returns `this` unchanged, which is the
//  MSVC scalar-deleting-destructor ABI.
//----------------------------------------------------------------------------
CWidget* CWidget::DeleteSelf(int flags)
{
    this->CWidget::~CWidget();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F2671B0 - CSelectable's slot 1.  Identical body one class down.
//----------------------------------------------------------------------------
CSelectable* CSelectable::DeleteSelf(int flags)
{
    this->CSelectable::~CSelectable();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//============================================================================
//  0x6F2B7800 - CItem's vtable slot 1.
//
//  Unlike the two above, this one is *not* fused with the destructor it
//  calls: the shipped code makes a real `call sub_6F2B7780`, so ~CItem has
//  to live in a different translation unit (item_dtor.cpp) or MSVC inlines
//  it here and the call disappears.  The cost of that split is one
//  instruction inside ~CItem itself (a `call ~CAgentWar3` where the shipped
//  code has the inlined vtable store) - see item_dtor.cpp.
//
//  The extra `test esi,esi` the two destructors above do not have is a real
//  null check in front of the free, not part of the flags test.
//============================================================================
#include "item.h"

CItem* CItem::DeleteSelf(int flags)
{
    this->CItem::~CItem();
    if ((flags & 1) != 0 && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}


//============================================================================
//  0x6F001B30 and 0x6F03FAC0 - CAgentWar3's and CWar3Image's own slot 1.
//
//  They belong in this file for exactly the reason CWidget's and
//  CSelectable's above do: both shipped bodies stamp ??_7CAgentWar3@@6B@
//  and then make a real `call sub_6F0017F0` into ~CAgent, which is
//  ~CAgentWar3 inlined - and MSVC can only inline it where the definition
//  at the top of this file is visible.
//
//  All four are the same nine instructions at four addresses, and none of
//  the four has the `this != 0` test that CAgent's and CObserver's carry
//  (agent_dtor.cpp, observer.cpp).  That is not a different source
//  spelling: the inlined destructor's first act is a store through `this`,
//  which faults on a null pointer, so the compiler already knows the
//  pointer is non-null by the time it reaches the test and drops it.
//  CWar3Image contributes no vtable stamp of its own for the same
//  dead-store reason the header comment gives for CWidget's.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F001B30 - CAgentWar3's slot 1.
//----------------------------------------------------------------------------
CAgentWar3* CAgentWar3::DeleteSelf(int flags)
{
    this->CAgentWar3::~CAgentWar3();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F03FAC0 - CWar3Image's slot 1.  Its address is in a different
//  neighbourhood from the other three, which is the one thing about it that
//  is not shared; the body is identical.
//----------------------------------------------------------------------------
CWar3Image* CWar3Image::DeleteSelf(int flags)
{
    this->CWar3Image::~CWar3Image();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F28A8A0 - CTask's own slot 1 (task.h).  Same shape, same reason for
//  living here rather than in a Task/-local file: CTask and CUnitRefList
//  each add no member of their own that needs destroying, so the
//  compiler-generated ~CTask() collapses down to CAgentWar3's own inlined
//  vtable-stamp-then-tail-call - which only happens where
//  ~CAgentWar3()'s definition (top of this file) is visible to inline.
//  Declared in task.h, defined here for that reason alone.
//----------------------------------------------------------------------------
CTask* CTask::DeleteSelf(int flags)
{
    this->CTask::~CTask();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F28A8D0 - CTaskParam's own slot 1 (taskparam.h).  Same shape, same
//  reason for living here rather than in a Task/-local file: CTaskParam
//  adds only a plain dword (m_field34) that needs no destructor work of its
//  own, so the compiler-generated ~CTaskParam() collapses down to
//  CAgentWar3's own inlined vtable-stamp-then-tail-call the same way
//  CTask's own slot 1 does immediately above - which only happens where
//  ~CAgentWar3()'s definition (top of this file) is visible to inline.
//  Declared in taskparam.h, defined here for that reason alone.
//----------------------------------------------------------------------------
CTaskParam* CTaskParam::DeleteSelf(int flags)
{
    this->CTaskParam::~CTaskParam();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F28A900 - CTaskRotate's own slot 1 (Task/taskrotate.h).  Same shape
//  again: the compiler-generated ~CTaskRotate() has a FloatMini
//  sub-object's own trivial virtual destructor to run
//  (Task/taskrotate.h's own field comment), but nothing else, so /O2 drops
//  that dead store the same way it drops CDestructable's own m_floatB
//  teardown below - the shipped disassembly never touches +0x34 here at
//  all, only the inlined ~CAgentWar3() tail.  Declared in taskrotate.h,
//  defined here for the same cross-TU-inlining reason as CTask's/
//  CTaskParam's own, immediately above.
//----------------------------------------------------------------------------
CTaskRotate* CTaskRotate::DeleteSelf(int flags)
{
    this->CTaskRotate::~CTaskRotate();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F28A930 - CTaskAction's own slot 1 (Task/taskaction.h).  Same shape,
//  same reason for living here rather than in a Task/-local file:
//  CTaskAction adds no field of its own at all, so the compiler-generated
//  ~CTaskAction() collapses down to CAgentWar3's own inlined
//  vtable-stamp-then-tail-call the same way CTask's/CTaskParam's own slot 1
//  does above - which only happens where ~CAgentWar3()'s definition (top of
//  this file) is visible to inline.  Declared in taskaction.h, defined here
//  for that reason alone.
//----------------------------------------------------------------------------
CTaskAction* CTaskAction::DeleteSelf(int flags)
{
    this->CTaskAction::~CTaskAction();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F28A960 - CTaskOrderParam's own slot 1 (Task/taskorderparam.h).  Same
//  shape, same reason for living here rather than in a Task/-local file:
//  CTaskOrderParam adds no field of its own at all, so the
//  compiler-generated ~CTaskOrderParam() collapses down to CAgentWar3's own
//  inlined vtable-stamp-then-tail-call the same way CTask's/CTaskParam's/
//  CTaskAction's own slot 1 does above - which only happens where
//  ~CAgentWar3()'s definition (top of this file) is visible to inline.
//  Declared in taskorderparam.h, defined here for that reason alone.
//----------------------------------------------------------------------------
CTaskOrderParam* CTaskOrderParam::DeleteSelf(int flags)
{
    this->CTaskOrderParam::~CTaskOrderParam();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F28AA80 - CTaskGhost's own slot 1 (Task/taskghost.h).  Same shape,
//  same reason for living here rather than in a Task/-local file:
//  CTaskGhost's own six new fields (four plain dwords, two untyped-storage
//  FloatMini blocks) need no real destructor work of their own, so the
//  compiler-generated ~CTaskGhost() collapses down to CAgentWar3's own
//  inlined vtable-stamp-then-tail-call the same way every sibling's own
//  slot 1 does above - which only happens where ~CAgentWar3()'s definition
//  (top of this file) is visible to inline.  Declared in taskghost.h,
//  defined here for that reason alone.  This is the last class in the
//  CTaskXxx family (task.h's own list) - see taskghost.h's own header
//  comment.
//----------------------------------------------------------------------------
CTaskGhost* CTaskGhost::DeleteSelf(int flags)
{
    this->CTaskGhost::~CTaskGhost();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//============================================================================
//  0x6F267420 - CDestructable::~CDestructable().  See destructable.h.
//
//  Lives here rather than in destructable_dtor.cpp (which still declares
//  it, forwarding to this definition) for the same reason CWidget's and
//  CSelectable's own slot-1 bodies do: the shipped tail calls straight
//  into CAgent::~CAgent(), skipping CAgentWar3's own out-of-line
//  ~CAgentWar3(), which only happens when that trivial destructor's
//  definition is visible right here to be inlined away.  0x6F266D00
//  (~CAgentWar3, above) sits in the same 0x6F266xxx/0x6F267xxx address
//  neighbourhood as 0x6F267420 itself - the same "compiled together"
//  evidence this file's own header comment already used for CWidget's and
//  CSelectable's pair.
//
//  CDestructable::DeleteSelf (destructable_deleteself.cpp) is *not* fused
//  with this the way CWidget's/CSelectable's/CAgentWar3's/CWar3Image's own
//  slot 1 bodies are above: the shipped code makes a real `call` from
//  DeleteSelf into this destructor (item_dtor.cpp's own CItem shape, not
//  this file's), so it has to stay in a different translation unit or
//  MSVC inlines it there instead and DeleteSelf loses its own call.
//
//  Two members get real teardown, in reverse declaration order (the same
//  rule unit_dtorbody.cpp's own note documents):
//
//    * m_handleObject (+0x70) is a TRefCnt-derived sub-object with nothing
//      else owned inside it, so its own destructor is just the inlined
//      base-class vtable stamp - the same `StampTRefCnt` shape this file's
//      own CAgentWar3/CWar3Image destructors give a name to nowhere
//      explicitly (they have no fields of their own), reached here through
//      TRefCnt's own destructor directly (refcnt.h);
//    * m_ref58 is a refcounted `CAgentTimer*` - test, decrement, dispatch
//      slot 0 if that was the last reference.  No null-out: the object is
//      going away (same note unit_dtorbody.cpp makes for CUnit's own
//      refcounted members).
//
//  m_floatB (+0x60, between the two) contributes nothing visible: its own
//  destructor is the same one-instruction vtable stamp as m_handleObject's,
//  and /O2 drops it as a dead store because nothing after it ever reads
//  +0x60 again before the object is freed.
//
//  Same unreproducible __except_handler4-shaped SEH frame as every other
//  multi-member destructor in this family (docs/msvc-vc8-idioms.md,
//  CObserver::~CObserver's own case): two member teardowns the compiler
//  cannot prove will not throw is enough on its own, with no explicit
//  __try/__finally in the source - not fixable at this toolchain's fixed
//  /GS- /EHs-c-.
//============================================================================
#include "destructable.h"
#include "agenttimer.h"

static void StampTRefCnt58(void* field)
{
    ((TRefCnt*)field)->TRefCnt::~TRefCnt();
}

static void ReleaseTimerRef58(CAgentTimer** field)
{
    TRefCnt* p = *(TRefCnt**)field;
    if (p != 0 && --p->m_refcount == 0)
        p->ReleaseSelf();
}

CDestructable::~CDestructable()
{
    StampTRefCnt58(&m_handleObject);
    ReleaseTimerRef58(&m_ref58);
}
