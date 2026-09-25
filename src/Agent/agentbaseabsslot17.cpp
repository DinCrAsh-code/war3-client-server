//============================================================================
//  0x6F4A5A30 - NIpse::CRlAgent::Slot17.
//
//  Reached through CAgentBaseAbs's own vtable slot 17 (+0x44, confirmed by
//  docs/targets/vtables/CAgentBaseAbs.txt), but not owned by that class:
//  slot 17 does not even exist at CPresenceTagged (whose own vtable has
//  only 17 slots, 0-16); NIpse::CPrRelation *introduces* it as a no-op
//  (`nullsub_1486`), and NIpse::CRlAgent is the first class to give it a
//  real body - CAgentBaseAbs never overrides it again. So this is
//  CRlAgent's own method, per CLAUDE.md's "lowest class in the chain whose
//  own address for that slot differs from its own base's".
//
//  Reads `arg`'s own +0x0C field into this object's own +0x50 (the same
//  field `NIpse::CRlAgent::Apply` stamps, per Agent/agentbaseabs_slot3.cpp's
//  own comment) and, only if that read a non-null value, forwards it
//  through MaybeDispatchOrder (0x6F4A59F0, agentbaseabsorderdispatch.cpp)
//  with a hard-coded 0 selector - a real, guaranteed no-op call at this one
//  call site (see that file's own header comment for why).  Then reads
//  `arg`'s own +0x10 field and reports it via NotifySubjectField2
//  (0x6F4A4E40, out of this session's own batch, owned by a parallel
//  session - see agentbaseabsrelatedcascade.cpp's own note on this exact
//  cross-batch calling-convention seam).
//
//  `sub_6F4A45E0` (`mov eax,[ecx+0Ch]; retn`) is the same one-instruction
//  accessor `agentbaseabs_slot3_callees.cpp` already reconstructs as
//  `CAgentBaseAbsSlot3Arg::LookUp_6F4A45E0` - reused here by that name
//  rather than re-declared locally, since a second C++ name for one
//  address is exactly what `gen_rename_from_funcmap.py` refuses. Its
//  sibling `sub_6F4A4600` (`mov eax,[ecx+10h]; retn`) has no reconstruction
//  anywhere else yet, so it's defined directly below, on `arg`, not `this`.
//  `NotifyField2` (0x6F4A4E40) is the real `SAgentBaseAbsNotifySubject`
//  from `agentbaseabsnotify.cpp` - reused by that name for the same reason,
//  with `notifyValue` reinterpreted as its real `unsigned int subjectId`
//  parameter (same bit pattern, passed through unexamined either way).
//============================================================================
#include "crlagent.h"
#include "agentrelation.h"
#include "agentbaseabs_related.h"

struct CAgentBaseAbsSlot17Arg
{
    void* GetField0x10();
};

struct SAgentBaseAbsNotifySubject
{
    void NotifyField2(unsigned int subjectId);
};

//  0x6F4A59F0 - MaybeDispatchOrder(orderBase, table), agentbaseabs-
//  orderdispatch.cpp's own real declaration order.
struct CAgentBaseAbsOrderDispatch
{
    void MaybeDispatchOrder(void* orderBase, void* table);
};

//  0x6F4A4600 - a one-instruction field accessor, `this+0x10`, thiscall,
//  no stack args (`retn` with no operand) - same shape as
//  `CAgentBaseAbsSlot3Arg::LookUp_6F4A45E0` (`+0x0C`), just the next field.
void* CAgentBaseAbsSlot17Arg::GetField0x10()
{
    return *(void**)((char*)this + 0x10);
}

namespace NIpse {

void CRlAgent::Slot17(void* arg)
{
    void* self = this;
    void* value = ((CAgentBaseAbsSlot3Arg*)arg)->LookUp_6F4A45E0();
    *(void**)((char*)self + 0x50) = value;

    if (value != 0)
    {
        //  `selector` is a hard-coded 0 at this one call site, so this is
        //  a guaranteed no-op - see agentbaseabsorderdispatch.cpp's own
        //  header comment.
        ((CAgentBaseAbsOrderDispatch*)self)->MaybeDispatchOrder(0, value);
    }

    void* notifyValue = ((CAgentBaseAbsSlot17Arg*)arg)->GetField0x10();
    ((SAgentBaseAbsNotifySubject*)self)->NotifyField2((unsigned int)notifyValue);
}

}  // namespace NIpse
