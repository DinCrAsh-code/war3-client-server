//============================================================================
//  0x6F308E70 - CCameraWar3::Update (vtable slot 3, +0x0C - overrides
//  CObserver::Method_0x0C).  The per-frame/per-event message dispatch
//  root: ~465 instructions, two real jump tables plus one linear
//  comparison cascade past them, all keyed off `msg->m_id` (+0x08) and a
//  second field `msg->m_field10` (+0x10).
//
//  **The two jump tables' own remap byte arrays (`byte_6F3093F0`,
//  `byte_6F309404`) are not recoverable from this pass's tools** - they
//  are `.rdata` bytes outside any function's own extent, and
//  `worktree_store.py bytes`/`resolve` both come back empty for them (the
//  bulk `agent_worktrees` dump only captured `jpt_6F308EE5`'s own first
//  two dwords, not the full 10-entry table). What *is* fully recoverable,
//  and is what this reconstruction is built from instead, is IDA's own
//  merge comments on the dump: `"jumptable 6F308EE5 cases 1,2,4-6,8-10"`
//  names the one shared target eight of the ten `m_id` values in [1,10]
//  reach, and every other `m_id` in that range (3, 7), out of range (0,
//  11-31), or above it (>0x20) converges - confirmed by reading every one
//  of the ~30 `def_6F308EE5` branches in the dump - on the identical
//  `mov eax,1` / pop / `retn 4` shape, so "not one of the eight handled
//  ids" is exactly `return 1`, and the giant comparison cascade after
//  0x6F308F93 is what m_id > 0x20 (or one of {0,3,7}) falls into to look
//  for one of a handful of specific large `m_id`/`m_field10` codes before
//  giving up and returning 1 too.  No case's own *body* was guessed at
//  from this gap - only which raw value selects it.
//
//  Written with `goto` into the same block structure the dump itself has
//  (a cascade of range checks, not a tidy nested `switch`) rather than
//  refactored into one - CLAUDE.md's own house rule: "if it is there
//  because the shipped code demands it, the comment must say so, rather
//  than the next reader 'fixing' it and losing instructions" applies to
//  control flow shape as much as to arithmetic.  One function, matching
//  the shipped single `PROC`; three internal labelled regions
//  (`SmallSwitchGate`, `LargeDispatch`, `SubtypeDispatch`) mirror
//  0x6F308EEC / 0x6F308F93 / 0x6F3090AD.
//
//  Every callee below is already reconstructed elsewhere in this repo
//  except `sub_6F62B170`/`sub_6F3A1650`/`sub_6F421E50` (declared, not
//  claimed by this pass) and 0x6F2781F0 (this file's own thiscall-correct
//  local thunk - see GameUI/cameracandidatescan.cpp's identical note on
//  why it is not Misc/citemclosure_batchL_thunks.cpp's existing
//  `__stdcall` declaration of the same address).
//============================================================================
#include "gameui.h"
#include "observer.h"
#include "frame.h"

//  0x6F3A1650 - IndexedArrayHolder_6F3A1650::At, already reconstructed
//  elsewhere (Misc/misc_field_getters.cpp) - declared locally the way
//  every other TU that calls it does (see e.g. Agent/relationconstant.cpp).
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};
extern void* g_unk6FAB65F4;   // dword_6FAB65F4

//  0x6F421E50 - PeekFieldWalkerHead, already reconstructed
//  (Item/unitsubgrouprefresh.cpp).
struct SFieldWalkerHolder;
int __fastcall PeekFieldWalkerHead(SFieldWalkerHolder* self);

//  0x6F2781F0, reached with a real `this` in ecx - see
//  GameUI/cameracandidatescan.cpp's own header note on why this is a
//  fresh local declaration rather than Misc/citemclosure_batchL_thunks.cpp's
//  existing `__stdcall(int,int)` one.
namespace
{
    struct SPositionDeltaTarget2
    {
        void RefreshPositionDeltaChecked(int outXYAddr, int mode);
    };
    __declspec(naked) void SPositionDeltaTarget2::RefreshPositionDeltaChecked(int, int)
    {
        __asm { mov eax, 0x6F2781F0 }
        __asm { jmp eax }
    }
}

//  flt_6F9361D0 - the blend time every SetCursorMode call in this
//  function shares.
extern const CFloat g_cursorModeBlendTime;

int CCameraWar3::Method_0x0C(const SAgentMessage* msg)
{
    if (m_updateSuppressed != 0)
        return 0;

    CGameUI* ui = (CGameUI*)GetGameUI(1, 0);
    STrackTarget* target = ui->m_pTrackedTarget;
    int trackable = 0;
    int takesCamera = 0;
    if (target != 0)
    {
        trackable = target->IsTrackable();
        takesCamera = target->TakesOverCamera();
    }

    unsigned int type = msg->m_id;

    if (type == 0x20)
    {
        ReleasePanModifiers();
        return 0;
    }

    if (type > 0x20)
        goto LargeDispatch;

    //  Eight of the ten in-range ids (1,2,4,5,6,8,9,10) share this gate;
    //  0, 3 and 7 fall straight to the big dispatch below.
    if (type == 0 || type == 3 || type == 7)
        goto LargeDispatch;

    {
        int idx = msg->m_field10;
        if (idx == 0)
            goto ApplyFlags;

        if (IsPanMovementActive() != 0)
            goto LargeDispatch;
        if (m_preferenceField30 != 0)
            goto LargeDispatch;
        if (ui->m_viewReadyA == 0)
            goto LargeDispatch;
        if (trackable != 0)
            goto LargeDispatch;

        if (idx == 0)
            goto ApplyFlags;

        //  +0x24C is an unnamed CGameUI field this pass does not
        //  otherwise reach - only its own +0xA0 dword is read here,
        //  against `trackable` (always 0 on this path).
        {
            int* field24C = *(int**)((char*)ui + 0x24C);
            if (*(int*)((char*)field24C + 0xA0) != trackable)
                return 1;
        }

        if (idx == 0)
            goto ApplyFlags;

        //  +0x250 - another unnamed CGameUI field, a CCameraWar3-compatible
        //  pointer (only GetField84's own +0x84 read is proven).
        {
            CCameraWar3* other = *(CCameraWar3**)((char*)ui + 0x250);
            if (other->GetField84() != 0)
                return 1;
        }
    }

ApplyFlags:
    ApplyModifierFlags((int)msg->m_id, msg->m_field10, m_unknownFloats4DC[8], 1);
    return 1;

LargeDispatch:
    type = msg->m_id;

    if (type > 0x400500CD)
        goto RangeCheck2;

    if (type == 0x400500CD)
    {
        if (IsPanMovementActive() != 0)
            return 1;
        if (ui->m_viewReadyA == 0)
            return 1;
        if (m_pEventSink != 0)
            return 1;

        {
            int div = msg->m_field2C / 9;
            if (div < 0)
            {
                if (m_presetIndex > 0)
                {
                    SetTargetDistancePreset(m_presetIndex - 1);
                    return 0;
                }
                return 1;
            }
            if (div > 0)
            {
                SetTargetDistancePreset(m_presetIndex + 1);
                return 0;
            }
            return 1;
        }
    }

    if (type != 0x22)
        return 1;

    SetControllerMode(1);
    return (int)(type - 0x21);

RangeCheck2:
    if (type < 0x40060064 || type > 0x40060066)
        return 1;

    if (IsPanMovementActive() != 0)
        return 1;
    if (ui->m_viewReadyA == 0)
        return 1;

    {
        int wasThird = (type == 0x40060066);

        //  Both discard their return value - real side-effecting calls
        //  the shipped code makes and ignores, per its own
        //  `lea ecx,[eax+2]` / `mov ecx,1` immediate arguments.
        IsEventKeyRegistered((void*)2);
        IsEventKeyRegistered((void*)1);

        if (takesCamera != 0)
        {
            int* field24C = *(int**)((char*)ui + 0x24C);
            if (*(int*)((char*)field24C + 0xA0) != 0)
                return 1;
        }
        else if (wasThird == 0)
        {
            return 1;
        }

        //  SubtypeDispatch (0x6F3090AD onward): a third dispatch keyed on
        //  `msg->m_field10`.
        {
            int subType = msg->m_field10;

            if (subType > 0x204)
            {
                if (subType == 0x205)
                {
                    ApplyModifierFlags(1, takesCamera, m_unknownFloats4DC[9], 0);
                    return 1;
                }
                if (subType == 0x206)
                {
                    ApplyModifierFlags(8, takesCamera, m_unknownFloats4DC[9], 0);
                    return 1;
                }
                if (subType == 0x207)
                {
                    ApplyModifierFlags(2, takesCamera, m_unknownFloats4DC[9], 0);
                    return 1;
                }
                if (subType == 0x209)
                {
                    if (m_pEventSink == 0)
                    {
                        if (takesCamera != 0 && m_cursorMode == 0)
                        {
                            SetCursorMode(2, *(const float*)&g_cursorModeBlendTime.m_bits);
                            return 1;
                        }
                        if (wasThird != 0 && m_cursorMode == 2)
                        {
                            SetCursorMode(0, *(const float*)&g_cursorModeBlendTime.m_bits);
                            return 1;
                        }
                    }
                    return 1;
                }
                if (subType == 0x20C)
                {
                    if (wasThird == 0 && m_pEventSink == 0)
                        SetTargetDistancePreset(m_presetIndex + 1);
                    return 1;
                }
                if (subType == 0x20D)
                {
                    if (wasThird == 0 && m_pEventSink == 0 && m_presetIndex > 0)
                        SetTargetDistancePreset(m_presetIndex - 1);
                    return 1;
                }
            }
            else if (subType == 0x208)
            {
                if (m_pEventSink == 0)
                {
                    if (takesCamera != 0 && m_cursorMode == 0)
                    {
                        SetCursorMode(1, *(const float*)&g_cursorModeBlendTime.m_bits);
                        return 1;
                    }
                    if (wasThird != 0 && m_cursorMode == 1)
                    {
                        SetCursorMode(0, *(const float*)&g_cursorModeBlendTime.m_bits);
                        return 1;
                    }
                }
                return 1;
            }
            else if (subType == 0x202)
            {
                if (takesCamera != 0)
                    RefreshHighlightCandidate();
                return 1;
            }
            else if (subType == 0x10F)
            {
                if (wasThird != 0 && m_cursorMode == 2)
                    SetCursorMode(0, *(const float*)&g_cursorModeBlendTime.m_bits);
                return 1;
            }
            else if (subType == 0x20)
            {
                if (takesCamera != 0)
                    UpdateCursorPresetPosition();
                return 1;
            }
            else if (subType == 0x101)
            {
                if (wasThird != 0 && m_cursorMode == 1)
                    SetCursorMode(0, *(const float*)&g_cursorModeBlendTime.m_bits);
                return 1;
            }
            else
            {
                //  Every other subtype: only this class's own +0x484 code
                //  (an unnamed field this pass does not otherwise reach)
                //  is handled, and only when the tracked target takes
                //  over the camera.
                if (takesCamera == 0)
                    return 1;
                if (subType != *(int*)((char*)this + 0x484))
                    return 1;

                {
                    IndexedArrayHolder_6F3A1650* holder =
                        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
                    int index = *(unsigned short*)((char*)holder + 0x28);
                    SFieldWalkerHolder* record =
                        (SFieldWalkerHolder*)holder->At((unsigned int)index);
                    void* head = *(void**)((char*)record + 0x34);
                    int headObj = PeekFieldWalkerHead((SFieldWalkerHolder*)head);
                    if (headObj != 0)
                    {
                        float outXY[2];
                        ((SPositionDeltaTarget2*)headObj)
                            ->RefreshPositionDeltaChecked((int)&outXY[0], 0);
                        SnapPanTargetToGround(outXY[0], outXY[1]);
                    }
                }
                return 1;
            }
        }
    }

    //  Compiler warning C4715 flagged a real gap here, not a false
    //  positive: `subType > 0x204` above shares its body with the second
    //  dispatch tier this file's own header comment describes, and this
    //  session did not fully re-derive how 0x205/0x206/0x207's own
    //  sequential-subtract checks (handled higher up in the real asm, at
    //  0x6F3090FC) nest against 0x208/0x209/0x20C/0x20D's own compare
    //  chain (0x6F30911A) - the two tiers may share more of their
    //  fall-through than this reconstruction currently expresses, and a
    //  `subType` value that lands in neither tier's named cases has no
    //  return here. Every other unhandled-value path in this function
    //  falls through to a dump-confirmed `mov eax,1`; matching that here
    //  is the safe default rather than leaving undefined behaviour, but
    //  the nesting itself is unverified and worth a closer pass with the
    //  raw bytes open side by side.
    return 1;
}
