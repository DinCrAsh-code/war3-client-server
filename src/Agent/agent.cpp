//============================================================================
//  See agent.h.
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include "tsarray.inl"
#include <excpt.h>   // AbnormalTermination(), for the converting constructor's __finally

//  0x6F427550 - the agent-slot array's own ComputeChunk: identical to the
//  bucket arrays' (see docs/msvc-vc8-idioms.md, "A generic container
//  method's body doesn't depend on the unmapped parts of T") because it
//  only ever depends on sizeof(AGENT_SLOT), which happens to be 12 as well.
template unsigned int TSGrowableArray<AGENT_SLOT>::ComputeChunk(unsigned int);
// 0x6F42B060
template void TSGrowableArray<AGENT_SLOT>::SetAlloc(unsigned int);

//----------------------------------------------------------------------------
//  0x6F02F780.  AssignChecked's virtual call through obj's vtable is the one
//  thing in this converting constructor the compiler cannot prove will not
//  raise a real structured exception (an unknown vtable slot, reached before
//  /GS- or /EHs-c- have any say - SEH unwind is independent of both), so it
//  wraps that one call in a __try/__finally: if AssignChecked does not
//  return normally, release whatever half-assigned pointer it left behind
//  before the exception continues past this frame.  m_ptr is zeroed first so
//  Release() (agent.h) has something well-defined to test even if the
//  exception fires before AssignChecked's own store.
//----------------------------------------------------------------------------
CAgentPtr::CAgentPtr(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//----------------------------------------------------------------------------
//  0x6F022AA0.  The shipped code tests `checked` and then `obj` separately
//  before the add-ref, rather than folding them into one condition - the two
//  are provably the same pointer once `checked` is non-null, but nothing
//  hoists it away, so the source is written the same way rather than
//  "cleaned up" into a single `if`.
//----------------------------------------------------------------------------
CAgentPtr* CAgentPtr::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetAgileRootTypeId()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//----------------------------------------------------------------------------
//  0x6F471A70 - kMsgRegistered is not readable from anything in this call
//  tree (no string, no other reference to 0x40190064); it is simply the
//  constant the disassembly pushes.
//----------------------------------------------------------------------------
const unsigned int kMsgRegistered = 0x40190064;

void CAgent::NotifyRegistered(unsigned int arg1, void* arg2)
{
    Dispatch(kMsgRegistered, arg1, arg2);
}

//----------------------------------------------------------------------------
//  This file used to carry trivial, invented bodies for CAgent's declared-
//  but-unreconstructed virtuals (ReleaseSelf, Dispatch, Reserved0C/10/14/18,
//  GetAgileTypeId), because giving CAgent a real constructor
//  (agent_ctor.cpp) forces MSVC to emit CAgent's vtable, and that needs
//  every declared virtual's address to resolve at link time.
//
//  None is left.  The vtable sweep found real bodies for ReleaseSelf
//  (agent_releaseself.cpp), Dispatch and slots 3-5 (observer.h/
//  observer_slots.cpp) and slot 6 (agent_slot18.cpp), and the last one -
//  GetAgileTypeId, slot 7, which every concrete class in the sweep
//  overrides - now lives in src/vtablestubs.cpp as a naked jump to the
//  shipped 0x6F471BD0, which is what docs/targets/vtables/CAgent.txt says
//  that slot actually is.
//
//  **Do not put a `{ return 0; }` back here.**  It was still present
//  alongside that stub for one commit, and two definitions of one symbol is
//  a one-definition-rule violation that only /FORCE:MULTIPLE hides: the
//  linker kept whichever object it saw first (this one) and reported
//  LNK4006, so the invented answer won over the real function.
//  build_mix.py's check_forced_duplicates() is what caught it, and it fails
//  the build rather than warn - see docs/notes/verifier-gate-link-
//  failures.md, cause 7.
//----------------------------------------------------------------------------
