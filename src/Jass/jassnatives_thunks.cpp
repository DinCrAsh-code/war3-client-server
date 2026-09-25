//============================================================================
//  Pass-through definitions for the callees this session's JASS-native and
//  CUnit-inventory reconstructions declare and call but do not reconstruct.
//
//  Same contract as unreconstructed_thunks.cpp: without these the tree does
//  not link (LNK1120) and nothing here can be tested in a running game; each
//  body jumps to the shipped function at its real address, so an
//  unreconstructed callee simply runs original code.
//
//  NOT reconstructions.  Never offer one as hookable - a detour cannot detour
//  a detour.  Delete each as its real body lands.
//
//  Every one of these is spelled with the *same* declaration the caller uses
//  (member vs free function, calling convention, parameter types), because
//  MSVC encodes all of that into the mangled name: a `__fastcall` definition
//  against a `__thiscall` declaration yields two symbols that never meet, and
//  verify.py cannot see it because it never links.  The declared argument
//  bytes are what tools/thunk_abi_audit.py checks against each shipped
//  callee's own `retn <n>`.
//============================================================================
#include "jassnatives.h"
#include "unitinventory.h"
#include "jasscreateunit.h"
#include "unitcreateparams.h"
#include "fvec3.h"
#include "itemhandlemain.h"

#define JASS_THUNK(addr)         \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

//----------------------------------------------------------------------------
//  The per-type handle resolvers used to be here - all of them __fastcall,
//  all `retn 0`.  All but one are reconstructed now, one translation unit
//  each (jassresolve*handle.cpp): the `Location` native's call tree
//  identified the type-checked slot each one holds, and from there the nine
//  JASS ones are a single 115-instruction body with a different slot type
//  baked in.  ResolveUnitHandle (0x6F3BDCB0) and ResolveItemHandle
//  (0x6F3BEB50) had already gone the same way, in earlier sessions.
//----------------------------------------------------------------------------
//  Deleting a thunk rather than leaving it beside the real body matters:
//  with /FORCE:MULTIPLE the first object on the link line wins, so a stale
//  thunk keeps shadowing the reconstruction that replaced it and
//  link_check.py's LNK2005 turns into an LNK4006 warning nobody reads.

//  ResolveWidgetHandle is the one member of this family still redirected:
//  its own type-checked slot has no dump here yet.  `retn 0` in the shipped
//  image, so __fastcall with its one argument in ecx pushes nothing and the
//  callee cleans nothing - which is what thunk_abi_audit.py checks this
//  declaration against.
__declspec(naked) CWidget*       __fastcall ResolveWidgetHandle(int)         JASS_THUNK(0x6F3BE5B0)

//----------------------------------------------------------------------------
//  The game state's own members (all __thiscall on dword_6FAB65F4's object).
//----------------------------------------------------------------------------
//  GetPlayerTeam (0x6F3A1660) and SetPlayerTeam (0x6F3A82B0) used to be here;
//  both are reconstructed now, in jassgamestate_playerteam.cpp.
//  DefineStartLocation (0x6F3A1320) and ForcePlayerStartLocation
//  (0x6F3A3240) used to be here; both are reconstructed now, in
//  jassgamestate_startlocation.cpp.

//----------------------------------------------------------------------------
//  Per-object members the natives dispatch.
//----------------------------------------------------------------------------
//  0x6F7527F0 - the Storm-side water-height query jasswaterheight.cpp
//  forwards to.  Inside Storm's own address range, so there is nothing here
//  to reconstruct it from; a __thiscall member with two stack arguments.
struct SStormWaterQuery
{
    int QueryHeightAt(const FVec3* point, float* outHeight);
};
__declspec(naked) int SStormWaterQuery::QueryHeightAt(const FVec3*, float*)   JASS_THUNK(0x6F7527F0)

//  SetCell (0x6F47CFB0) and SetRect (0x6F47CBA0) used to be here; both are
//  reconstructed now, in jassregioncellrect.cpp.
__declspec(naked) int   SJassRegionTests::ContainsPoint(const int*, const int*) JASS_THUNK(0x6F47D080)
__declspec(naked) int   SJassRegionTests::ContainsUnitPosition(void*)         JASS_THUNK(0x6F47D3E0)
//  Start (0x6F3F6890) and GetElapsed (0x6F3ECD30) are reconstructed now, in
//  jasstimerstartelapsed.cpp.
__declspec(naked) void  SJassTimer::Resume()                                  JASS_THUNK(0x6F3E8F30)
//  GetCentre (0x6F3B1FB0) used to be here; it is reconstructed now, in
//  jassrectgetcentre.cpp.
__declspec(naked) void  SJassRectCentre::OffsetBy(const SJassPoint*)          JASS_THUNK(0x6F3B1F10)
//  MoveTo (0x6F3B2360) used to be here; it is reconstructed now, in
//  jassrectmoveto.cpp, which is what put its two callees above and beside it
//  in this list.
//  GetRateA/GetRateB (0x6F40F7F0/0x6F40F820) and their setters
//  SetRateA/SetRateB (0x6F40F800/0x6F40F830) used to be here; all four are
//  reconstructed now, in jassplayerrates.cpp.
//  SetCrippled (0x6F416670) used to be here; reconstructed now, in
//  jassplayercripple.cpp.
__declspec(naked) void  CJassHandleRegistry::ReleaseObject(void*)             JASS_THUNK(0x6F42DCC0)
//----------------------------------------------------------------------------
//  Free functions.
//----------------------------------------------------------------------------
//  NotifyPlayerControllerChanged (0x6F416700) used to be here; reconstructed
//  now, in jassplayercontrollernotify.cpp.
//  JassStringToCString (0x6F3BAA40) used to be here; reconstructed now, in
//  jassstringtocstring.cpp.
//  0x6F39F5F0 - CItemSlotSingletonHost::InternCString (itemhandlemain.h) -
//  see jassmakestringhandle.cpp for the one real caller in this batch.
__declspec(naked) unsigned int CItemSlotSingletonHost::InternCString(const char*)
                                                                             JASS_THUNK(0x6F39F5F0)

//  0x6F2B9940 (CreateItemAtPosition) and the inventory gate's own five
//  callees (CUnit::IsOwnedByAPlayer, CAbilityInventory::AcceptsPowerup/
//  IsFull/StoreItem/ApplyPowerup) used to be thunked here.  All six are
//  real bodies now - see docs/targets/JASS_UnitAddItemById.md's second
//  batch and CreateItemAtPosition's own file (item_createatposition.cpp).

//----------------------------------------------------------------------------
//  The unit factory under CreateUnitForPlayer (jasscreateunit.h).  `retn 18h`
//  read off its own epilogue: six stack dwords past the two __fastcall
//  registers.  JassNativePrologue and CreateUnitForPlayer used to be thunks
//  here too - both are reconstructed now (jassnativeprologue.cpp,
//  jasscreateunitforplayer.cpp), so the thunks are gone rather than
//  shadowing the real bodies at link time.
//----------------------------------------------------------------------------
__declspec(naked) CUnit* __fastcall CreateUnitAtPosition(int, int, const CFloat*, const CFloat*, const CFloat*, int, int, int)
                                                                             JASS_THUNK(0x6F29F990)

//----------------------------------------------------------------------------
//  The parameter block's sub-object constructor (unitcreateparams.h) and the
//  two creation notifies (unit_creationgates.cpp).  `retn 4` on the first,
//  read off its own epilogue; the other two take both arguments in registers
//  and clean nothing.
//----------------------------------------------------------------------------
__declspec(naked) SUnitCreateParamsSub::SUnitCreateParamsSub(int)            JASS_THUNK(0x6F011300)
__declspec(naked) void __fastcall UnitCreationNotifyA_6F332F00(CUnit*, int)  JASS_THUNK(0x6F332F00)
__declspec(naked) void __fastcall UnitCreationNotifyB_6F333C10(CUnit*, int)  JASS_THUNK(0x6F333C10)

#undef JASS_THUNK
