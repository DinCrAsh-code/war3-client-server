//============================================================================
//  NIpse::CPoPos - vtable slots 3 (Apply) and 4 (Slot4). See popos.h for
//  the class-level header comment. Apply was a THUNK left by an earlier
//  session (docs/targets/NTempestNIpse_clusterH_remainder.md); now real -
//  its own tail resolves through SRegistrationContext::AllocRegistration
//  (Pathfinding/gridregistrationalloc.cpp, 0x6F4A03F0), unblocked by
//  finding that function's second caller, SPathRefAllocator::Alloc
//  (Pathfinding/pathrefallocator.cpp).
//============================================================================
#include "popos.h"
#include "spatialgrid.h"
#include "game.h"
#include "handletable.h"

extern "C" void* __cdecl memset(void* dst, int val, unsigned int size);

//  Global-scope, not NIpse:: - matches its own real out-of-line definition
//  (link_check.py caught the earlier namespaced redeclaration: MSVC mangles
//  the namespace into the symbol, and neither of these two has one).
extern const CFloat g_unk6FAB7768;         // fixed class-id CFloat constant.
                                            // funcmap.py DATA: dword_6FAB7768.

//  0x6F481430 - Misc/misc_trivial_getters.cpp.  Same minimal local
//  redeclaration convention Pathfinding/pathrefarrayresize.cpp's own copy
//  already uses (g_pTimeSync itself is declared in game.h, included
//  above).
struct SField238Owner { int __thiscall GetField238(); };

namespace NIpse {

//  0x6F4A71F0 - Apply's own "resolve or default" helper, `req + 0xC`.
//  Own TU (popos_pickregistrationfloatordefault.cpp) - see that file's own
//  header comment for why keeping it out of this one is load-bearing.
unsigned int* __fastcall PickRegistrationFloatOrDefault_0xC(
    unsigned int* out, const SHandleRegistrationRequest* req);

//----------------------------------------------------------------------------
//  0x6F4A7570 - vtable slot 3, "Apply".  Base `CRlProperty::Apply` first,
//  then this class's own seven +0x78..+0x90 CFloats (see popos.h's own
//  header comment for the per-field story), then builds a local
//  `SHandleRegistrationRequest` (m_priorSlot resolved from this object's
//  own +0x14 flags bit 0x80000000: -2 "allocate now" when set, -1 "none
//  yet" otherwise; m_secondSlot always -1 "not yet known") and uses its
//  address as the "InitGrid" configuration argument for both of this
//  object's own +0x94/+0x98 registration allocations - the first against
//  `CTimeSync::GetCellGrid()`, the second against `CTimeSync`'s own
//  +0x238 "tick" field (SField238Owner::GetField238 above) - finally
//  setting the 0x1000000 bit on the SECOND registration's own m_typeMask
//  right after allocating it (matches the shipped
//  `or dword ptr [eax+34h], 1000000h` immediately following the
//  `mov [esi+98h], eax` store).
//----------------------------------------------------------------------------
void CPoPos::Apply(SHandleRegistrationRequest* arg)
{
    ((CRlProperty*)this)->Apply(arg);

    m_baseX = g_unk6FAB7768;
    m_baseY = g_unk6FAB7768;
    m_velX  = g_CFloatZero;
    m_velY  = g_CFloatZero;

    unsigned int resolved;
    PickRegistrationFloatOrDefault_0xC(&resolved, arg);
    *(unsigned int*)&m_fieldD = resolved;

    m_fieldE = g_CFloatZero;
    m_fieldF = g_CFloatZero;

    SHandleRegistrationRequest localRequest;
    memset(&localRequest, 0, 0x24);
    localRequest.m_priorSlot =
        (*(unsigned int*)((char*)this + 0x14) & 0x80000000u) != 0 ? -2 : -1;
    localRequest.m_secondSlot = -1;

    CCellGrid* grid = g_pTimeSync->GetCellGrid();
    m_syncChannelA = ((SRegistrationContext*)grid)
                         ->AllocRegistration(this, (int)&localRequest);

    int tick = ((SField238Owner*)g_pTimeSync)->GetField238();
    m_syncChannelB = ((SRegistrationContext*)tick)
                         ->AllocRegistration(this, (int)&localRequest);
    m_syncChannelB->m_typeMask |= 0x1000000;
}

//----------------------------------------------------------------------------
//  0x6F4A7650 - vtable slot 4, "Slot4". Sets the 0x1000000 flags bit
//  (+0x4C, same field every sibling in this chain's own Slot4 touches),
//  detaches the base's own dependents (`CRlProperty::DetachDependents`,
//  a protected base method - widened from `private` this session, see
//  rlproperty.h's own updated comment), tears down both +0x94/+0x98
//  registration handles through `CGridRegistration::TeardownRegistration`
//  (no null guard at either call site, matching the shipped code exactly),
//  then tail-calls the base `CRlProperty::Slot4`.
//----------------------------------------------------------------------------
void CPoPos::Slot4(int arg)
{
    *(unsigned int*)((char*)this + 0x4C) |= 0x1000000;

    ((CRlProperty*)this)->DetachDependents();

    m_syncChannelA->TeardownRegistration();
    CGridRegistration* channelB = m_syncChannelB;
    m_syncChannelA = 0;
    channelB->TeardownRegistration();
    m_syncChannelB = 0;

    ((CRlProperty*)this)->Slot4(arg);
}

}  // namespace NIpse
