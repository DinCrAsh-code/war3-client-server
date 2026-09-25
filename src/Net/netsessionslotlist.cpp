//============================================================================
//  0x6F549D40 - TSExplicitList<CNetData::CPlayer>::UnlinkAll, the slot
//  table's own table-wide list drained.
//
//  Nothing but the instantiation: the body is Containers/tslist.inl's,
//  which is where the reasoning for walking m_prevlink rather than Tail()
//  lives.  Its own translation unit because the one caller
//  (CNetData::SLOTTABLE::ReleaseAll) reaches it with a real call.
//============================================================================
#include "netdata.h"
#include "tslist.inl"

template void TSExplicitList<CNetData::CPlayer>::UnlinkAll();
