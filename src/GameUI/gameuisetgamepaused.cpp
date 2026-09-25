//============================================================================
//  0x6F2FA7E0 - CGameUI::SetGamePaused.  The whole of `JASS_PauseGame`.
//
//  Five stack arguments (`retn 14h`), of which the native supplies all five:
//  the pause flag, the acting player's slot, and three flags that say
//  whether the request should be recorded, latched and announced.
//
//  The body is three passes:
//
//    1. decide whether this player may pause at all.  A real player has to
//       own the local slot and be participating; an observer (slot 0xFF) is
//       let through on a different test entirely, and the *acting* slot's
//       referee flag is remembered because the notification text depends on
//       it;
//    2. walk the five controllers and the world frame.  Pausing clears the
//       command card, the item bar, the hero bar, the minimap and the world
//       frame's own slot 0x10C, cancels the portrait panel's control track,
//       clears the selection overlay and re-places every selection visual;
//       resuming does the same walk with the flags inverted and *no*
//       overlay work;
//    3. announce it.  The pausing player's name is coloured with
//       "|c%2.2x%2.2x%2.2x%2.2x%s|r" - the four bytes are the colour dword
//       read most-significant first - and folded into one of three game
//       strings, which then goes to the message frame's slot 0x6C for ten
//       seconds.
//
//  Two locals are 0x9F-byte buffers, which is what puts a /GS cookie in the
//  shipped prologue.  This repo builds at a fixed /GS-, so the cookie load,
//  the `xor esp` and the check call are the only instructions of this
//  function that cannot come back - see docs/msvc-vc8-idioms.md.
//
//  Own translation unit: twenty-two real calls out of it.
//
//  ==========================================================================
//  PARKED EXPERIMENT (2026-08-27/28) - operator request "let me move the
//  camera while the game is paused" - REVERTED TO SHIPPED BEHAVIOUR after a
//  live test falsified the premise it was built on.  Read this before
//  touching either PAUSE_TRY_* macro below.
//
//  The original theory: the +0x3F4 frame's slot 0x10C is a generic
//  SetEnabled(0)-on-pause call (true - corroborated independently by the
//  identical pattern in gameuipausecaption.cpp and chatbaractivate.cpp on
//  unrelated frames), so it must be the "world/camera frame", and disabling
//  it must be what freezes manual camera input and dims the screen on
//  pause.  That identification was never checked - it was inferred purely
//  from the slot number matching the sibling UI panels' own disable calls.
//
//  It was wrong.  Walking CGameUI::CGameUI's own body (asm/sub_6F2FE9F0_...
//  .md) from the store at +0x3F4 back to its constructor call shows the
//  object there is a **CPortraitButton** (0x6F36F8D0, already reconstructed
//  in portraitbutton.cpp, RTTI-confirmed by its own `??_7CPortraitButton@@
//  6B@` vtable stamp) - a plain CButtonFrame-derived button, unrelated to
//  the 3D viewport or CWorldFrameWar3.  See gameui.h's m_pPortraitButton3F4
//  for the corrected field (was misnamed `m_pWorldFrame` by whichever prior
//  session read this comment at face value).  Disabling a portrait button
//  has no plausible mechanism for a screen-wide 50% dim or a camera-drag
//  freeze; the whole first candidate should have been treated as unproven,
//  not evidence-backed.
//
//  The operator tested both candidates together live (singleplayer "pause"
//  via the main menu, and a real LAN pause) and got a worse result than the
//  shipped baseline on both counts: the camera still did not move, AND the
//  game stopped actually pausing - units kept moving under both a
//  single-player and a LAN pause with this hook armed.  That second part is
//  new: the diagnostic scaffold below already established (8377086) that
//  hooking this function *fully unmodified* pauses the game correctly and
//  only breaks health bars, so the units-still-moving regression is
//  something these two skipped lines caused between them, not a pre-
//  existing defect.  Given the corrected identity above, the portrait-
//  button disable is very unlikely to be that cause - a UI button being
//  left enabled cannot plausibly stop the simulation from halting - which
//  leaves `m_pausedByGame` as the live suspect, but this has NOT been
//  isolated by a real test yet; treat it as an open question, not a
//  conclusion.
//
//  So: both PAUSE_TRY_* macros below default to 0 (shipped behaviour,
//  matching the original 148/281 IDENTICAL score) until a bisected live
//  test says otherwise.  Flip exactly ONE to 1 per test build - never both
//  at once, that is what already failed - to find out (a) whether either
//  one reproduces the units-still-moving regression alone, and (b) whether
//  either one does anything at all for camera movement (current evidence
//  says no for the combination, but neither has been tested alone).  The
//  real cause of the camera freeze is still unknown; it is very likely NOT
//  in this function at all, given the prior session's static trace already
//  found zero pause-coupling anywhere in the reconstructed camera subsystem
//  (docs/notes/camera-movable-during-pause-feature.md) - the next step is
//  probably finding the actual mouse-drag/edge-scroll input code and
//  checking *that* for a pause gate, rather than reasoning backward from
//  this function's side effects again.
//
//  Both macros are single, isolated writes/calls with no effect on this
//  function's own stack layout, argument count or `retn` size regardless of
//  their value - abi_audit.py and thunk_abi_audit.py are unaffected either
//  way.  With both at 0 (the default), the compiled output is byte-for-byte
//  what it was before this experiment started, and funcmap.py's BEHAVIOUR
//  entry is back to the original IDENTICAL text.
//
//  Still armed like any other hook - false in mix_config.json regardless of
//  these macros' values.  See CLAUDE.md, "mix_config.json lives here":
//  nothing an agent does may set that to true.
//  ==========================================================================
//============================================================================

//  Skip the +0x3F4 CPortraitButton's slot-0x10C disable on the pause edge.
//  Corrected understanding (above): this button is unrelated to the camera
//  or the world simulation, so this is now believed inert either way - but
//  it was tested only combined with PAUSE_TRY_SKIP_WORLDFRAME_PAUSED_FLAG
//  below, never alone.  0 = shipped.
#define PAUSE_TRY_SKIP_PORTRAIT_DISABLE   0
//  Skip `worldFrame->m_pausedByGame = 1`.  The live suspect for the
//  units-still-moving regression (above), but unconfirmed - it was tested
//  only combined with PAUSE_TRY_SKIP_PORTRAIT_DISABLE above, never alone.
//  0 = shipped.
#define PAUSE_TRY_SKIP_WORLDFRAME_PAUSED_FLAG 0

//  ==========================================================================
//  TEMPORARY DIAGNOSTIC SCAFFOLD - not a reconstruction, not a feature.
//  Confirmed live: hooking this function unmodified (CAMERA_MOVABLE_DURING_
//  PAUSE off, matching the shipped stream exactly) still detaches every
//  unit/building's health bar from its owner on pause, freezing it at its
//  last screen position. Confirmed NOT present with this function fully
//  unhooked. So the cause is somewhere in this function's own pause-branch
//  side effects, not in the camera experiment's two skipped lines above
//  (the bug reproduces with those both on and off) - flip exactly ONE
//  of the switches below to 0 per test build, leave the rest at 1, and
//  report which single one clears the health-bar bug. Revert every one of
//  these to 1 (or remove the scaffold entirely) once the culprit is found -
//  none of them should ship as a real change without a documented reason
//  the way the PAUSE_TRY_* macros above have one.
//
//  CORRECTION after the first "all six off, same bug" report: the first
//  round of flags did not actually cover everything the comments claimed.
//  BISECT_WORLDFRAME_TRACK's comment said it covered "ResetTrack() +
//  m_pauseKind/m_pauseLatched writes", but only the ResetTrack() call was
//  ever wrapped - the two direct field writes a few lines below it
//  (m_pWorldFrameWar3->m_pauseKind = 0; m_pWorldFrameWar3->m_pauseLatched =
//  1;) ran unconditionally in every build the operator tested, flag or no
//  flag. That is now fixed: both writes are under the same flag the
//  original comment promised. Two more pieces of real, always-run work in
//  the pause path had no flag at all - m_pPauseButton->SetPaused(paused)
//  and the coloured-player-name Storm_578 formatting block - and now do
//  (BISECT_PAUSE_BUTTON, BISECT_COLORED_NAME). So "all switches off" has
//  never actually been tested yet; the previous round's negative result
//  only rules out the six pieces that really were gated.
#define BISECT_TEXT_AGENTS_PAUSE   1   // SetTextAgentsPaused(1) - floating text
#define BISECT_PORTRAIT_CANCEL     1   // m_pPortrait->CancelControlTrack()
#define BISECT_CONTROLLER_DISABLE  1   // command card/item bar/hero bar/minimap SetEnabled(0)
#define BISECT_SELECTION_REFRESH   1   // ClearSelection() + RefreshAllSelectionVisuals()
#define BISECT_FIELD26C            1   // OrField284SetField26C(0)
#define BISECT_WORLDFRAME_TRACK    1   // ResetTrack() + m_pauseKind/m_pauseLatched writes
#define BISECT_NETDATA_REQUEST     1   // CNetData::RequestPause/RequestResume (0 = treat as always-accepted)
#define BISECT_MESSAGE_POST        1   // the announce block's PostMessageFn slot-0x6C dispatch
#define BISECT_PAUSE_BUTTON        1   // m_pPauseButton->SetPaused(paused)
#define BISECT_COLORED_NAME        1   // GetPlayerUiColor/GetPlayerDisplayName/Storm_578 name coloring
//  ==========================================================================

//  ==========================================================================
//  DELIBERATE FEATURE (2026-08-28) - "let the camera move while the game is
//  paused."  Not a reconstruction: shipped WC3 never does this.  See
//  docs/notes/pause-mouse-capture.md for the whole investigation this is the
//  payoff of.
//
//  The camera code itself has no pause-coupling anywhere (two full static
//  traces found none).  Live testing showed the real symptom is broader than
//  the camera: during a real pause NO click reaches anything at all - not
//  the minimap, not a unit, not the world.  Root cause, confirmed live with
//  a debugger: `CSimpleTop::m_pCapturedFrame` (src/Frame/frame.h) - "the
//  frame that currently holds the mouse capture" - is stuck non-null during
//  pause, so ordinary UI input routing sends every mouse event to that one
//  captured frame and nowhere else, regardless of where the cursor actually
//  is. Six rounds of reconstruction (CSimpleButton's own 29-slot vtable,
//  CSimpleTop's own 57-slot vtable, the real WndProc, CGxRawInputBridge, the
//  input-event ring's consumer chain) all confirmed the code that
//  *acquires* capture is real, generic Storm plumbing this pipeline has not
//  reconstructed (and may never fully - it is shared by 28+ unrelated
//  subsystems), but none of them needed to find it, because the fix does
//  not require knowing who set the field - only how to release it, which
//  is already fully reconstructed:
//
//      if (frame == m_pCapturedFrame) {
//          frame->NotifyCaptureLost();   // vtable slot 24
//          m_pCapturedFrame = 0;
//      }
//
//  is CSimpleTop::RemoveFrameFromLayer's own already-EXACT/IDENTICAL body
//  (src/Frame/simpletoplayers.cpp) - the real, shipped way this engine
//  releases a stuck capture.  This feature just calls that same release
//  unconditionally on the pause edge, instead of waiting for whatever
//  ordinarily triggers it (a mouse-up this session never found) to run.
//
//  Never tested live - the debugger session that found the mechanism ended
//  before this specific line could be armed and tried.  Off by default;
//  flip to 1 for a bench session, and if it works, say what capture would
//  otherwise be released onto (which button it was) in the write-up.
#define CAMERA_FEATURE_RELEASE_CAPTURE_ON_PAUSE   0
//  ==========================================================================

#include "gameui.h"
#include "gameuithunks.h"
#include "netdata.h"
#include "playercolor.h"        // g_unk6FAB65F4
#include "frame.h"              // g_pSimpleTop, CSimpleTop::m_pCapturedFrame

struct IndexedArrayHolder_6F3A1650
{
    char           m_reserved00[0x28];
    unsigned short m_actingSlot;    // +0x28
    int At(unsigned int index);
};

//  0x6F40B1C0 - Misc/trivialaccessors_04.cpp: the slot's referee flag.
int __fastcall GetField0x2A8_6F40B1C0(const void* self);
//  0x6F53E670 - Game/gamedatatag.cpp.
int IsGameDataTagSet_6F53E670();
//  0x6F2F3220 - Misc/misc_trivial_getters.cpp, reached on the CGameUI.
struct SField284Owner { void __thiscall OrField284SetField26C(unsigned int mask); };

//  Storm ordinal 578 - SStrPrintf.
int Storm_578(char* dest, unsigned int size, const char* format, ...);

//  slot 0x10C of the +0x3F4 CPortraitButton: enable or grey out (generic
//  CFrame dispatch - see the corrected identity note above this function).
typedef void (__thiscall *SetFrameEnabledFn)(void* self, int enabled);
//  slot 0x6C of the message frame: post one line for `seconds` seconds.
typedef void (__thiscall *PostMessageFn)(void* self, const char* text,
                                         const int* color, float seconds,
                                         int flags);

enum { kNoPlayerSlot = 0xFF, kMessageSeconds = 10 };

void CGameUI::SetGamePaused(int paused, int actingSlot, int notify, int latch,
                            int record)
{
    IndexedArrayHolder_6F3A1650* world =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;

    int actingIsReferee = 0;
    int allow;

    if (actingSlot == kNoPlayerSlot)
    {
        allow = (paused != 0 && notify != 0) ? 1 : 0;
    }
    else
    {
        actingIsReferee =
            GetField0x2A8_6F40B1C0((const void*)world->At(actingSlot));

        unsigned char localSlot = CNetData::GetPausingSlot(0);

        if (paused != 0)
        {
            //  A player who still owns a slot cannot pause on someone
            //  else's behalf, and one who is not participating cannot pause
            //  at all.
            if (localSlot != kNoPlayerSlot)
                return;
            if (CNetData::IsSlotParticipating((unsigned char)actingSlot, 0) == 0)
                return;

            allow = (notify != 0) ? 1 : 0;
        }
        else
        {
            if (localSlot == kNoPlayerSlot)
                return;

            if (GetField0x2A8_6F40B1C0((const void*)world->At(localSlot)) != 0
                && actingIsReferee == 0)
                return;

            allow = 0;
        }
    }

    m_pauseRequested = allow;

    if (record != 0)
        m_pausedByPlayer = paused;

    if (paused == 0 && m_pausedByPlayer != paused)
        return;

    CWorldFrameWar3* worldFrame = m_pWorldFrameWar3;

    if (worldFrame != 0)
    {
        if (paused != 0)
        {
#if BISECT_WORLDFRAME_TRACK
            worldFrame->ResetTrack();
#endif
#if !PAUSE_TRY_SKIP_WORLDFRAME_PAUSED_FLAG
            worldFrame->m_pausedByGame = 1;
#endif
        }
        else
        {
            worldFrame->ReleaseCursorModes();
            worldFrame->ResetTrack();
            worldFrame->m_suppressed = 0;
            worldFrame->SetCursorState(0, 0, 0);
        }
    }

    if (actingSlot != kNoPlayerSlot)
    {
        int accepted;

#if BISECT_NETDATA_REQUEST
        if (paused != 0)
            accepted = CNetData::RequestPause((unsigned char)actingSlot, 0,
                                          (actingIsReferee == 0) ? 1 : 0);
        else
            accepted = CNetData::RequestResume(0);
#else
        accepted = 1;
#endif

        if (accepted == 0)
            return;
    }

#if BISECT_PAUSE_BUTTON
    if (m_pPauseButton != 0)
        m_pPauseButton->SetPaused(paused);
#endif

    //  The coloured player name, empty when there is no acting player.
    char coloredName[0x1C];

    coloredName[0] = 0;

#if BISECT_COLORED_NAME
    if (actingSlot != kNoPlayerSlot)
    {
        unsigned int color = GetPlayerUiColor(actingSlot).m_value;
        const char* name = GetPlayerDisplayName(actingSlot, 1);
        const unsigned char* bytes = (const unsigned char*)&color;

        Storm_578(coloredName, sizeof(coloredName),
                  "|c%2.2x%2.2x%2.2x%2.2x%s|r",
                  bytes[3], bytes[2], bytes[1], bytes[0], name);
    }
#endif

    char message[0x9F];
    char format[0x9F];
    int  colorOverride;

    if (paused != 0)
    {
        if (notify != 0)
        {
#if BISECT_WORLDFRAME_TRACK
            m_pWorldFrameWar3->m_pauseKind = 0;
            m_pWorldFrameWar3->m_pauseLatched = 1;
#endif

#if BISECT_TEXT_AGENTS_PAUSE
            ((CGameWorld*)world)->SetTextAgentsPaused(1);
#endif

#if BISECT_PORTRAIT_CANCEL
            m_pPortrait->CancelControlTrack();
#endif
#if BISECT_CONTROLLER_DISABLE
            m_pCommandCard->SetEnabled(0);
            m_pItemBar->SetEnabled(0);
            m_pHeroBar->SetEnabled(0);
            m_pMiniMap->SetEnabled(0);
#endif

#if !PAUSE_TRY_SKIP_PORTRAIT_DISABLE
            ((SetFrameEnabledFn)(*(void***)m_pPortraitButton3F4)[0x10C / 4])(
                m_pPortraitButton3F4, 0);
#endif

#if BISECT_SELECTION_REFRESH
            m_pSelectionOverlay->ClearSelection();

            RefreshAllSelectionVisuals();
#endif

#if BISECT_FIELD26C
            ((SField284Owner*)this)->OrField284SetField26C(0);
#endif

#if CAMERA_FEATURE_RELEASE_CAPTURE_ON_PAUSE
            //  Not shipped behaviour - see the header comment above.  The
            //  exact release CSimpleTop::RemoveFrameFromLayer already does
            //  when it drops a captured frame, run here unconditionally so
            //  a stuck capture cannot survive into a pause.
            if (g_pSimpleTop != 0 && g_pSimpleTop->m_pCapturedFrame != 0)
            {
                g_pSimpleTop->m_pCapturedFrame->NotifyCaptureLost();
                g_pSimpleTop->m_pCapturedFrame = 0;
            }
#endif
        }

        if (IsGameDataTagSet_6F53E670() == 0)
            return;
        if (actingSlot == kNoPlayerSlot)
            return;
        if (m_pMessageFrame == 0)
            return;

        GetGameString(actingIsReferee != 0 ? "PAUSE_GAME_NOTIFY_NO_TIMEOUT"
                                           : "PAUSE_GAME_NOTIFY",
                      format, sizeof(format));

        Storm_578(message, sizeof(message), format, coloredName,
                  3 - CNetData::GetSlotPauseCount((unsigned char)actingSlot, 0));

        colorOverride = -1;

#if BISECT_MESSAGE_POST
        ((PostMessageFn)(*(void***)m_pMessageFrame)[0x6C / 4])(
            m_pMessageFrame, message, &colorOverride,
            (float)kMessageSeconds, 0);
#endif

        return;
    }

    if (notify != 0)
    {
        m_pauseLatched = 0;
        m_pausedByPlayer = 0;
        m_pWorldFrameWar3->m_pauseLatched = 0;
        m_pWorldFrameWar3->m_pauseKind = latch;

        ((CGameWorld*)world)->SetTextAgentsPaused(0);

        m_pPortrait->SetPortraitFrame(0);
        m_pCommandCard->SetEnabled(1);
        m_pItemBar->SetEnabled(1);
        m_pHeroBar->SetEnabled(1);
        m_pMiniMap->SetEnabled(1);

        ((SetFrameEnabledFn)(*(void***)m_pPortraitButton3F4)[0x10C / 4])(
            m_pPortraitButton3F4, 1);

        ((SField284Owner*)this)->OrField284SetField26C(0);
    }

    if (IsGameDataTagSet_6F53E670() == 0)
        return;
    if (actingSlot == kNoPlayerSlot)
        return;
    if (m_pMessageFrame == 0)
        return;

    GetGameString("RESUME_GAME_NOTIFY", message, sizeof(message));

    Storm_578(format, sizeof(format), message, coloredName);

    colorOverride = -1;

#if BISECT_MESSAGE_POST
    ((PostMessageFn)(*(void***)m_pMessageFrame)[0x6C / 4])(
        m_pMessageFrame, format, &colorOverride, (float)kMessageSeconds, 0);
#endif
}
