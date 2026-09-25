//============================================================================
//  TSExplicitList<NETCONN,-572662307>::UnlinkAll, instantiated for the
//  stack-local list TCPMGR::Destruct drains each CONNSTATE shard into.
//============================================================================
#include "ostcp.h"
#include "tslist.inl"

namespace OsNet { }

template void TSExplicitList<OsNet::NETCONN>::UnlinkAll();
