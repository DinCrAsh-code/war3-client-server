//============================================================================
//  0x6F2F7590 - assign a candidate into `this`'s own SCheckedSelectableSlot
//  at +0x3B8 (already reconstructed, SCheckedSelectableSlot::Assign,
//  agenttypedslots.h) and clear bit 0 of +0x26C through the same opaque
//  SField284Owner receiver GameUI/gameuisetgamepaused.cpp and
//  Player/playerscore_tallyapply.cpp already redeclare locally.
//============================================================================
#include "agenttypedslots.h"

//  Redeclared per file the same way every other user of this opaque
//  receiver does (GameUI/gameuisetgamepaused.cpp,
//  Player/playerscore_tallyapply.cpp) - the mangled name is what has to
//  match, not the declaration site.
struct SField284Owner
{
    void __thiscall OrField284SetField26C(unsigned int mask);
};

//  edx unused - candidate arrives on the stack (`retn 4`), and the dummy
//  edx parameter is what keeps __fastcall from putting it there instead.
void __fastcall AssignSelectableSlotAndClearField26C(
        void* thisObj, void* /*unused edx*/, CAgent* candidate)
{
    ((SCheckedSelectableSlot*)((char*)thisObj + 0x3B8))->Assign(candidate);
    ((SField284Owner*)thisObj)->OrField284SetField26C(0);
}
