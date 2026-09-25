//============================================================================
//  0x6F44D140 - `TSList<T,0>::UnlinkAll()`, instruction for instruction the
//  same body as CStatus::STATUSENTRY's own instantiation
//  (Containers/statusentrylist.cpp) and OsNet::OUTPUT's (0x6F6DE1B0,
//  storm.h's own note) - no `/OPT:ICF`, so a third copy of the identical
//  template body is compiled out-of-line here too.  Reached in the JASS VM
//  module's own address neighbourhood; nothing in this closure's own reach
//  identifies the element type, so it stays an incomplete forward
//  declaration the same way STATUSENTRY does.
//============================================================================
#include "tsfixedlist.inl"

struct JassStrayListNode;

template void TSList<JassStrayListNode, 0>::UnlinkAll();
