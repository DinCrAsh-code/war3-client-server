//============================================================================
//  0x6F27BA70 - see the declaration in unit.h.  Reached from CUnit's own
//  slot 3 (Method_0x0C, one of its message-case handlers) and slot 72
//  (NegateAndForwardOffset).
//
//  Walks the +0x1DC ability chain the same way FindAbilityForOrder does -
//  QueryHandleField0x54 (Item/itemhandleresolve.h) resolving each
//  SRefMiniValue handle pair to an ability object - calling each resolved
//  object's own vtable slot 0x100 (+0x40) with (a, b) forwarded unchanged,
//  then following that object's own +0x24 handle pair as the chain's
//  "next" link.  The ability object's class is not established anywhere in
//  this tree, so only the two offsets this loop actually touches (the
//  +0x24 chain link, the +0x100 vtable slot) are read through raw
//  pointer arithmetic rather than a named struct.
//
//  Both resolve sites repeat the same "already known empty -> skip the
//  call and treat it as null" guard QueryHandleField0x54's own header
//  documents as the sentinel test - the shipped code re-runs it at the
//  call site rather than always paying for the call, so this does too
//  (ResolveOrNull below), once for the initial +0x1DC handle and once per
//  loop iteration for the chain link.
//============================================================================
#include "unit.h"
#include "../Item/itemhandleresolve.h"

typedef void (__thiscall *AbilityMessageFn)(void*, int, int);

void CUnit::NotifyAbilitiesOfMessage(int a, int b)
{
    SOptionalHandleRef* ref = (SOptionalHandleRef*)&m_ref1DC;
    if ((ref->m_handle & (unsigned int)ref->m_typeTag) == 0xFFFFFFFFu)
        return;

    void* node = QueryHandleField0x54(ref);
    if (!node)
        return;

    do
    {
        AbilityMessageFn fn = *(AbilityMessageFn*)(*(char**)node + 0x100);
        fn(node, a, b);

        ref = (SOptionalHandleRef*)((char*)node + 0x24);
        node = ((ref->m_handle & (unsigned int)ref->m_typeTag) == 0xFFFFFFFFu)
                   ? 0
                   : QueryHandleField0x54(ref);
    } while (node);
}
