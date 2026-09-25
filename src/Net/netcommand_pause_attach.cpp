//============================================================================
//  0x6F5546E0 - CNetCommandPause's own copy of the trivial per-class
//  "attach" hook every builder in this module calls right after stamping
//  every field.  Its own TU, like every callee in this module that is
//  called for real rather than inlined (CLAUDE.md, "One translation unit
//  per original module") - kept in the same .cpp as CNetCommandPause_
//  BuildAndFire it collapsed to a pure identity function the optimiser
//  inlined away entirely, losing both the call itself and, by freeing the
//  scheduler of an opaque-call barrier, most of the surrounding function's
//  instruction order.
//============================================================================
#include "netcommand.h"
#include "cdatastore.h"

void* __fastcall CNetCommandPause_Attach(CDataStore* reader, CNetCommandBase* cmd)
{
    (void)cmd;
    return reader;
}
