//============================================================================
//  0x6F472420 - CAgent's own vtable slot 14 (+0x38), the save-side preamble
//  CAgentWar3::Save (agentwar3_savemessages.cpp) runs before its 0x400-wide
//  message sweep.  Was a `mov eax,<address> / jmp eax` redirect.
//
//  Three statements and a loop:
//
//   * hand the derived class its own chance to write flags, through slot 18
//     (+0x48, CAgent::SaveFlags - nullsub_48 in this class, overridden
//     below it);
//   * walk this agent's own observer registration table in key order and
//     write one (key, registrations) pair per key that has anything under
//     it, with the pair count back-patched into a placeholder dword once
//     the walk is done - which is what the `m_field10` read before the
//     placeholder and the WriteDwordAt after the loop are;
//   * then run slot 20 (+0x50, CAgent::Method_0x50, agent_slot20.cpp) five
//     times, for message ids 0x40190064..0x40190068, with the same literal
//     0x400 count CAgentWar3::Save's own sweep passes.
//
//  The cursor is built over `this + 0x14` - CAgent's *embedded* CObserver
//  (agent.h), not the base subobject at offset 0 - which is the same
//  distinction SAgentNotifyTarget::Notify's own `lea ecx,[esi+14h]` makes.
//  agentquery.cpp's constructor is declared taking a `CAgent*` because that
//  is what mangles into the symbol its one other caller (agent_slot20.cpp)
//  is compiled against; the cast here is where the +0x14 is stated.
//
//  **This cannot reach 1.000.**  The whole body sits inside the
//  `__except_handler4`-shaped SEH frame docs/msvc-vc8-idioms.md documents as
//  unreproducible at this repo's fixed `/GS- /EHs-c-` - a stack local with a
//  destructor (the query) plus calls the compiler cannot prove will not
//  throw is exactly the case it exists for.  That frame costs ~20
//  instructions of prologue/epilogue/unwind-state/funclet *and* puts four
//  more bytes under every `[esp+N]` in the body, so even a body that lines
//  up call for call scores well under half.  See the BEHAVIOUR entry.
//
//  Own translation unit: six real calls, three of them into agentquery.cpp.
//============================================================================
#include "agent.h"
#include "cdatastorescratch.h"

//  The stack-built cursor, declared exactly as agent_slot20.cpp declares it
//  (agentquery.cpp holds the bodies) plus the one method only this caller
//  needs.  Size read off the shipped frame there: 0x1C bytes.
struct SAgentQuery
{
    SAgentQuery(CAgent* owner);
    ~SAgentQuery();

    //  0x6F629B60 - move on to the next key anything is registered under
    //  and hand it back; 0 once the table is exhausted.
    int NextKey(unsigned int* key);

    char m_storage[0x1C];
};

//  0x6F472300 - write everything registered under the cursor's current key.
//  __fastcall with the query's address in edx, matching the shipped
//  `lea edx,[esp+var_20]` before the call.  Reconstructed in
//  agent_writequery.cpp; it returns how many entries it wrote, which this
//  call site ignores exactly as the shipped one does.  The return type is
//  part of the mangled name, so it has to be spelled the same here as
//  there.
unsigned int __fastcall WriteAgentQuery(CDataStoreScratch* store,
                                        SAgentQuery* query);

void CAgent::Save(CDataStoreScratch* store)
{
    SaveFlags(store);

    SAgentQuery query((CAgent*)((char*)this + 0x14));

    //  Where the count is going to go.  The placeholder is written first and
    //  patched afterwards rather than the pairs being counted up front,
    //  because counting them means walking the table twice.
    unsigned int countSlot = (unsigned int)store->m_field10;
    int pairs = 0;
    store->WriteDword(0);

    unsigned int key;
    while (query.NextKey(&key))
    {
        store->WriteDword(key);
        WriteAgentQuery(store, &query);
        pairs++;
    }

    store->WriteDwordAt(countSlot, (unsigned int)pairs);

    Method_0x50(store, 0x40190064, 0x400);
    Method_0x50(store, 0x40190065, 0x400);
    Method_0x50(store, 0x40190066, 0x400);
    Method_0x50(store, 0x40190067, 0x400);
    Method_0x50(store, 0x40190068, 0x400);
}
