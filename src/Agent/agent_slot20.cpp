//============================================================================
//  0x6F472550 - CAgentWar3's vtable slot 20 (+0x50), shared unchanged by
//  CWidget, CSelectable and CItem.  See agentwar3.h.
//
//  Build a query object over this agent, add the message id to it, and hand
//  the pair to the writer - then destroy the query.  Three of its four
//  calls are to the query type's own constructor, one method and its
//  destructor; all three are out of this session's scope and redirected.
//
//  The third argument (the shipped call site passes a literal 0x400) is
//  never read.
//
//  Same unreproducible __except_handler4-shaped SEH frame as every other
//  function in this family that builds a destructible object on the stack
//  (docs/msvc-vc8-idioms.md): a local with a destructor plus a call the
//  compiler cannot prove will not throw is enough on its own, with no
//  explicit __try/__finally in the source.
//============================================================================
#include "agentwar3.h"
#include "cdatastorescratch.h"

//  The stack-built query.  0x6F629FA0 / 0x6F629AB0 / 0x6F629770 are its
//  constructor, its one method and its destructor - all in CObserver's own
//  0x6F629xxx module, all now reconstructed in agentquery.cpp (they are a
//  cursor into the owner's observer registration table; see that file).
//  Declared, not defined, here: slot 20 must reach all three with real
//  calls.  Its size is read off the shipped frame: the local occupies
//  var_20..var_4, 0x1C bytes.
struct SAgentQuery
{
    SAgentQuery(CAgent* owner);
    ~SAgentQuery();
    void Add(unsigned int msgId);

    char m_storage[0x1C];
};

unsigned int __fastcall WriteAgentQuery(CDataStoreScratch* store,
                                        SAgentQuery* query);

//  0x6F472300 - write the query out.  __fastcall with the query's address
//  in edx, matching the shipped `lea edx,[esp+var_20]` before the call.
//  Reconstructed now, in its own translation unit
//  (agent_writequery.cpp) - this call has to stay a real call.  It returns
//  how many entries it wrote; this call site ignores that, as the shipped
//  one does.

void CAgent::Method_0x50(CDataStoreScratch* store, unsigned int msgId, int)
{
    SAgentQuery query(this);
    query.Add(msgId);
    WriteAgentQuery(store, &query);
}
