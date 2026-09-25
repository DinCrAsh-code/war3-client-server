//============================================================================
//  0x6F3D03B0 - JASS_Location, the `Location` native's own entry point.
//
//  The registration table at 0x6F3D4020 registers it as
//
//      push offset aRrHlocation;     "(RR)Hlocation;"
//      mov  edx, offset aLocation;   "Location"
//      mov  ecx, offset sub_6F3D03B0
//      call sub_6F455110
//
//  i.e. (real, real) -> location handle, and the bare `retn` makes it
//  __cdecl like every other native in this module.  See
//  docs/targets/jass-natives-registration-table.md.  Both reals arrive as
//  *pointers* to their raw 32-bit encodings and are stored into the new
//  object without ever reaching the FPU - the same convention
//  JASS_MoveLocation reads them back out with (jassnatives_location.cpp).
//
//  What the native does is make a fresh '+loc' agent and register it:
//
//    * look '+loc' up in the game-wide agile type registry and take that
//      type record's own allocator, which is the pool the object comes
//      from - the identical five lines CWidget::ClearPlayerMaskBits
//      (widget_slot68.cpp) and CAgentRegistrySingleton::FinishSingleton
//      (agentregistrysingleton.cpp) open with for their own type ids;
//    * hand the pool and the type to the global agent factory through the
//      same 0x2C-byte request block, and take the CAgent* the returned
//      holder carries at +0x54;
//    * write the coordinate pair into +0x24/+0x28, which is exactly where
//      GetLocationX/GetLocationY read it from;
//    * register it with the agent registry and return the slot token, which
//      is the `Hlocation;` handle JASS sees.
//
//  The type-checked slot the made object is parked in is a scoped local, so
//  the whole body sits inside the same unreproducible __except_handler4-
//  shaped frame every destructible local in this family carries - that is
//  the only reason this function is not 1.000 (see below).
//
//  Own translation unit: five real calls out of it, and it is the only
//  function in the 0x6F3D0xxx factory module this target reaches.
//============================================================================
#include "jassnatives.h"
#include "agenttypedslots.h"
#include "agiletype.h"
#include "widgetagentquery.h"
#include "itemhandlemain.h"
#include "agentregistry.h"
#include "game.h"

//  0x6F4326D0 - '+loc' (misc_rawcode_getters.cpp).  Called *twice*, once to
//  find the pool and once to fill the request block in, rather than kept in
//  a register: that is the shipped code, and it is the same double call
//  widget_slot68.cpp records for 'hgw+'.
unsigned int GetLocationAgileTypeId();

//  What the factory hands back: only the CAgent* at +0x54 is reached, the
//  same holder shape widget_slot68.cpp and agentregistrysingleton.cpp each
//  name for their own made object.
struct SMadeLocationAgent
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

int __cdecl JASS_Location(const int* x, const int* y)
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F3B8FA0 constructor is (checkedlocationslot.cpp).
    SCheckedLocationSlot held(0);

    AGILE_TYPE_ID id(GetLocationAgileTypeId());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetLocationAgileTypeId(), type->m_allocator);
    //  Stored again after Init has already stored it: the shipped code has
    //  both stores, and dropping this one loses an instruction.  Same in
    //  itemslotsingleton.cpp and agentregistrysingleton.cpp.
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeLocationAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    //  One read of the slot for the coordinate pair - the shipped code loads
    //  it into eax once and stores through it twice - and a second, separate
    //  read below for the registration, which is the `mov esi,[esp+...]` it
    //  issues again rather than keeping eax alive across GetSlotTable.
    SJassLocation* location = (SJassLocation*)held.m_value;
    location->m_x = *x;
    location->m_y = *y;

    //  Same tail as JASS_CreateUnit (jasscreateunit_native.cpp): the holder
    //  is *named* before the call, because the shipped code issues
    //  GetSlotTable before pushing Register's own two arguments and that is
    //  the order a named receiver forces.
    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)held.m_value, 1);
}
