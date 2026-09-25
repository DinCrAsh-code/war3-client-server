//============================================================================
//  TSList<OsNet::OUTPUT, 0>::UnlinkAll (0x6F6DE1B0).
//
//  The instantiation lives in its own translation unit for the reason
//  tslist.inl's own header comment gives: ~TCPCONN calls it rather than
//  inlining it, so its one caller has to see only the declaration.
//============================================================================
#include "ostcp.h"
#include "tsfixedlist.inl"

//  The one instantiation this module needs.  Naming the member is what makes
//  MSVC emit the body; the class itself is instantiated by TCPCONN's layout.
template void TSList<OsNet::OUTPUT, 0>::UnlinkAll();
