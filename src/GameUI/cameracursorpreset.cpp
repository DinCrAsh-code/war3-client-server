//============================================================================
//  Three of CCameraWar3::Update's (0x6F308E70) still-unreconstructed
//  leaves: the two "clamp an index, blend to it if it moved" setters, and
//  the cursor-preset table's own per-frame advance.
//
//  **SetTargetDistancePreset / SetCursorMode share one idiom.**  Both clamp
//  their int argument into a small range with the same branchless
//  "0 < x ? x : 0" (`xor eax,eax`/`cmp`/`sbb`/`and`) followed by a plain
//  `if (x > N) x = N;`, compare the result against a field, and - only on a
//  real change - store the new value and blend every property to it via
//  ApplyPresetBlend (0x6F306B80, still thunked - see gameui.h).  The
//  placeholder int this build pushes ahead of ApplyPresetBlend's float
//  argument (`push 0`/`push ecx` immediately overwritten by an `fstp`) is
//  dead by construction - MSVC reserves the stack slot with whatever is in
//  a register and then stores the real float over it - so it costs nothing
//  to leave out of the source.
//
//  **UpdateCursorPresetPosition's own field names converge with two other
//  sessions' independent reads of the same offsets** (gameui.h's own
//  header comment on +0x98..+0xB4 has the full cross-reference): +0xA0/
//  +0xA4 are the same two dwords CCameraWar3::Save/::Load carry, and +0xB0
//  is not its own field at all - it is `m_filterSlots.m_data`, the
//  cinematic filter's own already-established `SFilterSlotArray`, read
//  straight through rather than duplicated as a second pointer. What the
//  asm actually does, unambiguously:
//    1. GetGameUI(1, 0) unconditionally (its own +0x258, m_pauseRequested,
//       gates the whole function, but the call itself always happens first);
//    2. bail if the table is empty (+0xA4 == 0) or the game is paused;
//    3. `now = GetTickCountSeconds()`; if `now` has not yet reached the
//       cached +0x9C plus a small bias (dbl_6F875118 - g_cameraUpdateDebounceBias,
//       also read by 0x6F308CD0's own debounce, so it is a bias shared
//       across this module and not a per-field constant), step the index
//       back one slot (mod the table length); otherwise reuse whatever is
//       already sitting at +0xA0 unchanged;
//    4. stamp the new "now" into +0x9C, the chosen index into both +0x98
//       and (implicitly, since nothing ever reads +0xA0 back out again in
//       this closure) +0xA0's own slot is left stale until the next debounce
//       expiry;
//    5. republish `m_target` to the resulting table entry.
//  Which of +0x98/+0xA0 is "the" committed index and which is "the last one
//  applied while debounced" could not be settled further without a second
//  caller of the debounced (index != table-advance) path, which neither
//  dump's closure reaches from 0x6F308E70.
//============================================================================
#include "gameui.h"
#include "fvec3.h"
#include "pathmove.h"

extern const double g_cameraUpdateDebounceBias;   // dbl_6F875118
//  dword_6FAAE5B4 - already named g_unk6FAAE5B4 (funcmap.py DATA,
//  Unit/unit_dumpstate.cpp's own declaration) and read here as a float for
//  the hardware compare this call site actually does. Declared with the
//  same type as that file's own extern - two translation units declaring
//  the same funcmap.DATA global with different types is a real LNK2019 in
//  the operator's build (tools/link_check.py's own "SPLIT DATA GLOBAL"
//  check) - so the float reinterpretation happens at the use site instead.
extern const unsigned int g_unk6FAAE5B4;

//  0x6F303350 - Misc/citemclosure_batchL_thunks.cpp.  `retn 0x14`.
void __stdcall InstallVisualAttachmentPosition(int, int, int, int, int);

//  0x6F30A860 - GameUI/cameramiscleaves.cpp.  Stamps its fastcall receiver
//  (here, the raw preset index) into dword_6FAB542C; see that file's own
//  comment for why this call site is what settles what it is really for.
void __fastcall SetLastCameraWar3Raw(void* value);

//  0x6F6C4E80 - GameUI/cameramiscleaves.cpp.
float __fastcall GetTickCountSeconds();

//----------------------------------------------------------------------------
//  0x6F308C60 - CCameraWar3::SetTargetDistancePreset.  `retn 4`.
//----------------------------------------------------------------------------
void CCameraWar3::SetTargetDistancePreset(int index)
{
    unsigned int mask = (0u < (unsigned int)index) ? -1 : 0;
    unsigned int clamped = mask & (unsigned int)index;
    if (clamped >= 5)
        clamped = 5;

    if ((int)clamped != m_presetIndex)
    {
        m_presetIndex = clamped;
        ApplyPresetBlend(1.0f, 0);
        SetLastCameraWar3Raw((void*)(unsigned int)m_presetIndex);
    }
}

//  0x6F308CA0 - CCameraWar3::SetCursorMode has its own real definition in
//  GameUI/cameracursormode.cpp (a sibling batch's own reconstruction of the
//  same address, landed independently) - not duplicated here, even though
//  it shares the same clamp-and-blend idiom as SetTargetDistancePreset
//  above.

//----------------------------------------------------------------------------
//  0x6F303290 - CCameraWar3::UpdateCursorPresetPosition.  `retn 0`.
//----------------------------------------------------------------------------
void CCameraWar3::UpdateCursorPresetPosition()
{
    CGameUI* ui = (CGameUI*)GetGameUI(1, 0);

    if (m_cursorPresetCount == 0)
        return;
    if (ui->m_pauseRequested != 0)
        return;

    int index = m_cursorPresetPending;
    float now = GetTickCountSeconds();
    if (now < m_cursorPresetNextTime + g_cameraUpdateDebounceBias)
    {
        index = (unsigned int)(m_cursorPresetIndex + m_cursorPresetCount - 1) %
                (unsigned int)m_cursorPresetCount;
    }

    m_cursorPresetNextTime = now;
    m_cursorPresetIndex = index;

    const float* entry = (const float*)m_filterSlots.m_data + index * 3;
    CWorldVec3 world(CFloat(*(const int*)&entry[0]),
                     CFloat(*(const int*)&entry[1]),
                     CFloat(*(const int*)&entry[2]));
    m_target.SetWorldPosition(&world);
}

//----------------------------------------------------------------------------
//  0x6F3078B0 - CCameraWar3::SnapPanTargetToGround.  `retn 8`.
//
//  Drop the pan and every modifier, snap m_target straight onto the ground
//  under (x, y), and - only if that actually moved the target by more than
//  g_unk6FAAE5B4 (squared, so the compare never takes a
//  square root it does not need) - flag whatever CGameUI's own +0x3BC/+0x38C
//  chain is (neither of these fields has a caller anywhere else in this
//  target's closure, so they stay raw offsets rather than invented names).
//----------------------------------------------------------------------------
void CCameraWar3::SnapPanTargetToGround(float x, float y)
{
    ReleasePanModifiers();
    CancelModifiers();
    InstallVisualAttachmentPosition(0, 0, 0, 0, 0);

    FVec3 point;
    point.m_x = x;
    point.m_y = y;
    FVec3 ground;
    ProjectGroundPoint(&ground.m_x, &point.m_x);

    CWorldVec3 previous = m_target.SmartPtrToWorldVec3();

    CWorldVec3 world(*(const CFloat*)&ground.m_x,
                     *(const CFloat*)&ground.m_y,
                     *(const CFloat*)&ground.m_z);
    m_target.SetWorldPosition(&world);

    CWorldVec3 current = m_target.SmartPtrToWorldVec3();
    CFloat distSq(kCFloatNoInit);
    (previous - current).LengthSquared(&distSq);
    CFloat dist = CFloatSqrt(distSq);

    if (*(const float*)&dist > *(const float*)&g_unk6FAAE5B4)
    {
        CGameUI* ui = (CGameUI*)GetGameUI(0, 0);
        if (ui != 0)
            ui->m_pWorldFrameWar3->m_field38C = 1;
    }
}
