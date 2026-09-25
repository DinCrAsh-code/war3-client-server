//============================================================================
//  TSList<CStatus::STATUSENTRY, 0>::UnlinkAll (0x6F007C20).
//
//  Instruction for instruction the same body as the OsNet::OUTPUT
//  instantiation at 0x6F6DE1B0 (ostcp_sendqueue.cpp); the shipped link had
//  no /OPT:ICF, so the two really are emitted twice.
//
//  The element type is the shipped binary's: this one is called from
//  ~CStatus (0x6F4C3E90, which stamps `??_7CStatus@@6B@`) on the list at
//  CStatus+4, and the type descriptor `??_R0?AUSTATUSENTRY@CStatus@@@8`
//  (0x6FA85CAC) is the only nested struct CStatus has.  Nothing in this
//  closure reaches a STATUSENTRY's own fields, so it stays incomplete - a
//  list template only ever needs its element's address.
//============================================================================
#include "tsfixedlist.inl"

class CStatus
{
public:
    struct STATUSENTRY;
};

template void TSList<CStatus::STATUSENTRY, 0>::UnlinkAll();
