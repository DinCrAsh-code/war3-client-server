//============================================================================
//  0x6F472300 - WriteAgentQuery: serialise every registration the cursor can
//  reach under one message id, prefixed by how many there were.
//
//  Was a `mov eax,<address> / jmp eax` redirect in agent_slot20.cpp
//  ("out of scope; redirected"): the three query-object methods it needed
//  were themselves redirects then.  All five are reconstructed now
//  (agentquery.cpp), so this is a real body.
//
//  The shape:
//
//    * reserve a dword in the stream and remember where it went
//      (`m_field10` *before* the write is the offset of the slot the write
//      is about to fill), so the count can be patched in at the end;
//    * walk the cursor with SAgentQuery::NextEntry until it stops handing
//      back entries.  Each entry is a {value, resource} pair;
//    * for each, take a *type-checked, counted* reference to the resource
//      and write the {handle, typeTag} pair of whatever survived that check
//      (a null one writes -1/-1, which is WriteAgentHandlePair's own null
//      path), then the value itself;
//    * patch the count back into the reserved slot, and return it.
//
//  The type-checked reference is `CAgentPtr`'s converting constructor
//  (agent.h / agent.cpp, 0x6F02F780) **inlined**, not called: the shipped
//  code has GetAgileRootTypeId, the virtual GetAgileTypeId through slot 7,
//  AgileTypeIsDerivedFrom and the add-ref all in its own body, with the
//  argument order MSVC's right-to-left evaluation gives
//  (GetAgileRootTypeId first).  Spelling it as a real `CAgentPtr` local
//  would emit a call to that constructor instead, so the four steps are
//  written out here, with the matching release-if-last at the bottom of the
//  loop - the destructor, equally inlined.
//
//  Own translation unit: CAgent::Method_0x50 (agent_slot20.cpp) reaches this
//  with a real call, and this reaches five more.
//
//  **The `__except_handler4`-shaped frame does not reproduce**
//  (docs/msvc-vc8-idioms.md): two scope-table entries' worth of it here, one
//  per lifetime the shipped build tracks, plus the two release funclets at
//  0x6F022820 / 0x6F02F0B0 and the cookie trampoline the dump appends after
//  the body.  Those 28 instructions and the prologue/epilogue that carries
//  them are simply absent under /GS- /EHs-c-, and with them go the three
//  `[esp+var_4]` scope-state stores - one of which the shipped code reuses
//  as the loop counter's `+1` (`mov ebp,1` doubles as both the state number
//  and the increment) and another as the `-1` its release adds.
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include "cdatastorescratch.h"

//  agentquery.cpp owns the real definition; only the one method called here
//  is needed, and a member's mangled name does not depend on the layout.
struct SAgentQuery
{
    int NextEntry(void** value, void** resource);
};

//  0x6F2D0350 - agent_writehandlepair.cpp.
CDataStoreScratch* __fastcall WriteAgentHandlePair(CDataStoreScratch* store,
                                                   CAgent** slot);

unsigned int __fastcall WriteAgentQuery(CDataStoreScratch* store,
                                        SAgentQuery* query)
{
    //  Read before the write: this is the offset of the slot WriteDword is
    //  about to append, which the WriteDwordAt at the bottom patches.
    int countOffset = store->m_field10;
    store->WriteDword(0);

    unsigned int count = 0;

    void* value;
    void* resource;

    while (query->NextEntry(&value, &resource) != 0)
    {
        void*   payload = value;
        CAgent* obj     = (CAgent*)resource;

        //  CAgentPtr's converting constructor, inlined - see the header
        //  comment.  Two names for the one value on purpose: `slot` is the
        //  one whose address the writer gets, and `held` is the one the
        //  release at the bottom reads.  The shipped code holds it in a
        //  register (esi) *and* in the escaped stack slot, and keeps using
        //  the register across the call - which a single variable cannot
        //  give here, because taking its address forces MSVC to re-load it
        //  afterwards and to compare the reloaded value against zero
        //  instead of branching on the decrement's own flags.  Splitting
        //  them is exact and safe: WriteAgentHandlePair (agent_writehandle-
        //  pair.cpp) only reads through the pointer it is handed.
        CAgent* held = 0;
        CAgent* slot = 0;
        if (obj != 0 &&
            AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetAgileRootTypeId()))
        {
            obj->m_refcount += 1;
            held = obj;
            slot = obj;
        }

        WriteAgentHandlePair(store, &slot);
        store->WriteDword((unsigned int)payload);
        count += 1;

        //  ... and its destructor.
        if (held != 0)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    store->WriteDwordAt((unsigned int)countOffset, count);
    return count;
}
