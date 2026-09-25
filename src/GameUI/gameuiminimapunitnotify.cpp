//============================================================================
//  0x6F2F9CC0 - reached from UnitCreationNotifyA_6F332F00 (0x6F332F00,
//  Unit/unit_creationgates.cpp's own forward declaration) with `this` =
//  GetGameUI(0, 0)'s own return, `unit` = the newly created CUnit, `flag` =
//  the caller's own flag argument, both pushed onto the stack (`retn 8`) -
//  a thiscall member, not a `__fastcall` free function.
//
//  If `unit` belongs to the locally acting player (Widget/
//  cunit_agent3_islocallyowned.cpp's own IsOwnedByLocalPlayer, re-declared
//  below to mangle onto that already-reconstructed body rather than a
//  second symbol the linker never joins), construct a SCheckedUnitSlot
//  in-place over `unit` (Agent/agenttypedslots.h, 0x6F038450) and then call
//  through the mini-map panel's own vtable slot 0x6C/4 (27) at
//  CGameUI+0x3E0 (CGameUIMiniMapPanel* m_pMiniMap, gameui.h) - the panel's
//  own layout past +0x134/+0x150 is not otherwise modelled here, so the
//  slot is reached through the raw vtable-typedef-and-cast idiom
//  (pipeline/CLAUDE.md's own example) rather than a declared `virtual`.
//
//  The constructed SCheckedUnitSlot is placement-new'd into a raw byte
//  buffer, not a real local of that type: the shipped stream calls the
//  constructor (a real, unhooked `call sub_6F038450`) and then runs
//  straight into the vtable call with no matching Release() anywhere
//  before the epilogue - no inline decrement, no call.  A genuine scoped
//  local (`SCheckedUnitSlot guard(unit);`) gets its destructor's
//  `~SCheckedUnitSlot() { Release(); }` inlined at the end of the block by
//  this compiler (confirmed by trying it: the extra `add [x+4],-1` /
//  `cmp`/`call ReleaseSelf` tail it adds is not in the target stream at
//  all), so the shipped source cannot have written that - either a
//  deliberate held reference with no matching release on this path, or a
//  refcount leak already present in the original game.  Reproducing the
//  raw construct-and-never-destroy shape is what matches, not "fixing" it
//  into an RAII guard.
//============================================================================
#include "gameui.h"
#include "widget.h"
#include "unit.h"
#include "agenttypedslots.h"
#include "storm.h"   //  placement operator new(unsigned int, void*)

struct CWidgetLocalOwnerCheck : CWidget
{
    int IsOwnedByLocalPlayer();
};

typedef void (__thiscall *MiniMapNotifyFn)(void*);

void CGameUI::NotifyMiniMapIfLocallyOwned(CUnit* unit, int flag)
{
    if (((CWidgetLocalOwnerCheck*)unit)->IsOwnedByLocalPlayer())
    {
        volatile int flagCopy = flag;
        char guardStorage[sizeof(SCheckedUnitSlot)];
        new (guardStorage) SCheckedUnitSlot(unit);
        (void)flagCopy;

        void* panel = m_pMiniMap;
        ((MiniMapNotifyFn)(*(void***)panel)[0x6C / 4])(panel);
    }
}
