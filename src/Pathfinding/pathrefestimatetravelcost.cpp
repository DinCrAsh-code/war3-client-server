//============================================================================
//  0x6F4758D0 - CPathRef::EstimateTravelCost.
//
//  Resolves this ref's own current world position, then marshals it -
//  together with the candidate ref/field pair and a small literal-flag
//  set - into the real travel-cost search at 0x6F475560 and hands its
//  result straight back.  One caller: Pathfinding/playerdistancescan.cpp
//  (0x6F2DEC20), which supplies the candidate's own world position as
//  `targetPos` (already resolved via CPathRef::SmartPtrToWorldVec3 on the
//  candidate's own ref) and a `field34` from each side's owning record.
//
//  0x6F475560 itself is not reconstructed - a ~130-instruction body,
//  reached through CTimeSync::GetPathOwner (0x6F481460), that is out of
//  scope for this session's assigned addresses.  Declared here with the
//  exact ecx/edx + 8-stack-dword ABI this call site uses (retn 20h,
//  confirmed off its own raw_asm) and kept as a naked redirect to the
//  real, unhooked shipped body - same treatment the prior
//  JASS_CreateUnit-closure integration pass gave the other out-of-slice
//  callees it found (SCheckedSlot_6F2D04D0::Assign and friends; see
//  docs/targets/JASS_CreateUnit_closure.md, "Integration fixes").
//
//  The frame below exists because the shipped code builds three small
//  scratch values back-to-back on the stack and passes pointers into the
//  *middle* of that same block for two of the eight stack arguments -
//  `&frame.selfPtr` (arg_8 of the real search) starts one dword past
//  `&frame.selfPos` (the ecx argument), and both are read through by the
//  real search, not just this caller.  A single struct in field order is
//  the only way to reproduce that overlap from source; splitting the scratch
//  values into separate locals would leave the compiler free to place them
//  anywhere and lose the exact layout.
//============================================================================
#include "game.h"

struct SEstimateTravelFrame
{
    int        targetField34Copy;   // read through arg_10 of the real search
    int        selfField34Copy;     // ... together with the above, as a pair
    CWorldVec3 selfPos;             // read through ecx of the real search
    CPathRef*  selfPtr;             // ... together with the above, as a pair
    CPathRef*  targetPtrCopy;       // read through arg_8, alongside selfPtr
    CPathRef*  selfPtrCopy2;        // never read back by this caller's own
                                     // code; still written, matching the
                                     // shipped stream's own dead store
};

//----------------------------------------------------------------------------
//  0x6F475560 - shipped code, not yet reconstructed.  See file comment.
//----------------------------------------------------------------------------
__declspec(naked) int __fastcall
RunTravelCostSearch(const CWorldVec3* selfPosAndSelf,
                    const CWorldVec3* targetPos, int flagA, int range,
                    CPathRef** selfAndTargetRefs, int litSearchKind,
                    int* targetAndSelfFields, int litTwo, int litZero,
                    int flagB)
{
    __asm { mov eax, 06F475560h
            jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F4758D0
//----------------------------------------------------------------------------
int CPathRef::EstimateTravelCost(const CWorldVec3* targetPos, int range,
                                 CPathRef* targetRef, int targetField34,
                                 CPathRef* selfRef, int selfField34,
                                 int flagA, int flagB) const
{
    SEstimateTravelFrame frame;

    ToWorldVec3(frame.selfPos);

    frame.targetPtrCopy     = targetRef;
    frame.selfPtrCopy2      = selfRef;
    frame.targetField34Copy = targetField34;
    frame.selfField34Copy   = selfField34;
    frame.selfPtr           = const_cast<CPathRef*>(this);

    return RunTravelCostSearch(&frame.selfPos, targetPos, flagA, range,
                               &frame.selfPtr, 3, &frame.targetField34Copy,
                               2, 0, flagB);
}
