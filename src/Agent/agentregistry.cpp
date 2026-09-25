//============================================================================
//  See agentregistry.h.
//============================================================================
#include "agentregistry.h"
#include "game.h"   // placement new is already declared via storm.h

//  A throwaway K reference for PtrHashOnly()'s dead second argument (see
//  tshash.inl): any fixed global works, since it is never dereferenced, but
//  it has to be something with a static address - passing a local by
//  reference here would force that local to be stack-homed and change the
//  rest of the function's register allocation.
static HANDLE_ID s_unusedKey;

//----------------------------------------------------------------------------
//  0x6F42CB50 - grow the agent-slot array by exactly one element (the same
//  chunked-growth arithmetic TSGrowableArray<T>::SetCount already has,
//  inlined here for a fixed +1 rather than an arbitrary target count) and
//  placement-construct the fresh element's CAgentPtr, empty, with a __try
//  wrapped around just that one call so its own __finally has something to
//  clean up if a virtual call three frames down ever raises a real
//  structured exception (see CAgentPtr::CAgentPtr, agent.cpp).  This
//  particular __finally really is empty in the dump (a tail-call to
//  NoOpFinallyStub/nullsub_45): the array's own bookkeeping (m_count,
//  SetAlloc) has already committed by the time the placement-construct
//  runs, so there is nothing left for an abnormal exit to unwind here - only
//  the *element*, whose own constructor is the one with something to undo.
//
//  Byte-for-byte, the shipped __except_handler4/__try-__finally frame this
//  compiler cannot reproduce - see docs/msvc-vc8-idioms.md, "An
//  __except_handler4 frame under /GS- /EHs-c-": under this repo's fixed
//  /GS- flags, a __try/__finally here compiles to a plain, cookie-less
//  __except_handler3 frame with an EBP base, never the cookie-XORed,
//  frame-pointer-omitted, __CxxFrameHandler3-routed per-function trampoline
//  the dump shows (confirmed by hand-testing every /GS and /EHsc
//  combination this toolchain accepts - none reproduces it, so it is a
//  patch-level difference in the compiler itself, not a flag this repo
//  controls). What follows is the intended source shape; the SEH furniture
//  around it does not match and is not expected to.
//----------------------------------------------------------------------------
AGENT_SLOT* __fastcall AgentSlotArrayAppend(TSGrowableArray<AGENT_SLOT>* array)
{
    unsigned int count = array->m_count + 1;
    if (count > array->m_alloc)
    {
        unsigned int chunk = array->m_chunk;
        if (chunk == 0)
            chunk = array->ComputeChunk(count);

        unsigned int alloc = count;
        unsigned int over = count % chunk;
        if (over != 0)
            alloc = count + (chunk - over);

        array->SetAlloc(alloc);
    }

    AGENT_SLOT* slot = &(*array)[array->m_count];
    array->m_count = array->m_count + 1;

    if (slot)
    {
        slot->m_reserved00 = 0;
        __try
        {
            new (&slot->m_agent) CAgentPtr(0);
        }
        __finally
        {
        }
    }
    else
    {
        slot = 0;
    }

    return slot;
}

//----------------------------------------------------------------------------
//  0x6F430C80.  Rejects a handle that does not resolve, is not tagged as an
//  agile object, or already has an agent bound; otherwise hands back the
//  slot already registered for `unit`'s type tag if there is one and it
//  already names `unit`, or takes a slot - recycled from the free list if
//  one is waiting, freshly appended otherwise - binds the type tag to it in
//  the handle table, and assigns `unit` into it as the slot's agent.
//----------------------------------------------------------------------------
unsigned int CAgentRegistry::Register(CUnitAgent* unit, unsigned int extra)
{
    if (unit != 0)
    {
        unsigned int typeTag = unit->m_typeTag;
        CAgileHandleCheck* object =
            (CAgileHandleCheck*)LookupHandle(unit->m_handle, typeTag);
        if (object != 0)
        {
            object = (object->m_agileHandleTag == kAgileHandleTag) ? object : 0;
        }
        if (object != 0 && object->m_agentBinding == 0)
        {
            HANDLE2AGENT* existing =
                m_handleTable.PtrHashOnly(typeTag, s_unusedKey);
            if (existing != 0
                && m_slots[existing->m_slotToken - kAgentSlotTokenBias]
                       .m_agent.m_ptr == unit)
            {
                return existing->m_slotToken;
            }

            unsigned int token;
            AGENT_SLOT* slot;
            unsigned int freeCount = m_freeSlots.m_count;
            if (freeCount == 0)
            {
                token = m_slots.m_count + kAgentSlotTokenBias;
                slot = AgentSlotArrayAppend(&m_slots);
            }
            else
            {
                token = (unsigned int)m_freeSlots.m_data[freeCount - 1];
                m_freeSlots.SetCount(freeCount - 1);
                slot = &m_slots[token - kAgentSlotTokenBias];
                slot->m_reserved00 = 0;
            }

            HANDLE2AGENT* node = m_handleTable.NewNode(unit->m_typeTag, 0, 0);
            node->m_hashval = typeTag;
            node->m_slotToken = token;
            node->m_typeTagCopy = unit->m_typeTag;

            unit->NotifyRegistered(0x80267, this);
            slot->m_agent.AssignChecked(unit);

            slot->m_reserved00 += 1;
            slot->m_reserved08 = extra;
            return token;
        }
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F42D3F0 - the real, unhooked slot-freeing body: not reconstructed
//  (out of scope for this call tree - 63 instructions of handle-table and
//  free-list bookkeeping this dump never reaches), so called by raw
//  address the same way unitsubgrouprefresh.cpp's own AppendCandidateSlot
//  calls 0x6F0A4730 - a thiscall taking the token on the stack, `retn 4`
//  confirmed from its own listing.
//----------------------------------------------------------------------------
typedef void (__thiscall *FreeAgentRegistrySlotFn)(CAgentRegistry* self,
                                                    unsigned int token);
static FreeAgentRegistrySlotFn const FreeAgentRegistrySlot =
    (FreeAgentRegistrySlotFn)0x6F42D3F0;

//----------------------------------------------------------------------------
//  0x6F42DD40.  The refcount at AGENT_SLOT::m_reserved00 (Register's own
//  `+= 1` above) is read and compared signed (`jbe`, not `jz`, on a `test`
//  that never sets CF) - the shipped source reads as `<= 0`, not `== 0`,
//  even though the field only ever holds non-negative counts.
//----------------------------------------------------------------------------
void CAgentRegistry::ReleaseSlotRef(unsigned int token)
{
    if (token < kAgentSlotTokenBias)
        return;

    AGENT_SLOT* slot = &m_slots[token - kAgentSlotTokenBias];
    if (slot->m_reserved00 <= 0)
        return;

    slot->m_reserved00 -= 1;
    if (slot->m_reserved00 == 0)
        FreeAgentRegistrySlot(this, token);
}
