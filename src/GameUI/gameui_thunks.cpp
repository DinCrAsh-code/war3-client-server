//============================================================================
//  Pass-through definitions for the callees this target's slice declares and
//  calls but does not reconstruct.  Same rules as
//  src/Misc/unreconstructed_thunks.cpp: each body jumps to the shipped
//  function at its real address, so an unreconstructed callee simply runs
//  original code, and each one is deleted as its real body lands.
//
//  NOT reconstructions.  Never offer them as hookable.
//
//  Every `retn <n>` below was read off the shipped body with
//  `tools/dumpfn.py`, which is what fixes each declaration's argument count -
//  see tools/thunk_abi_audit.py for why that is not optional.
//============================================================================
#include "positionmodifier.h"
#include "floatpropmodifier.h"
#include "gameui.h"
#include "cameracontrollerbind.h"
#include "tokenizer.h"
#include "unittrackedref.h"
#include "fadetimer.h"
#include "CMiscCustom.h"
#include "selectionmanager.h"
#include "infopanel.h"
#include "commandbutton.h"
#include "herobar.h"
#include "buildmode.h"
#include "renderdevice.h"

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//----------------------------------------------------------------------------
//  The 0x6F4Axxx publishing layer under both modifier modules.  Each takes
//  the object LookupHandle just returned in ecx and everything else on the
//  stack.
//----------------------------------------------------------------------------
//  0x6F4A8450 - `retn 0Ch` (no dump of its own; the byte count is fixed by
//  MovementModifier::SetRate's own `retn 4`, which only balances if this
//  callee cleans the three dwords it is handed).
ADDR_THUNK(void SPositionSink::SetRate(const CFloat*, int, int), 0x6F4A8450)
//  0x6F4A7CB0 is reconstructed now - Widget/positionsinksettarget.cpp - and
//  the redirect that stood in for it here is gone with it.
//  0x6F4A73B0 is reconstructed now - CPathTrace::PublishVelocityDelta,
//  Pathfinding/pathtracepublishvelocity.cpp - and the redirect that stood in
//  for it here is gone with it.  The publisher the sink hands its delta to
//  is a CPathTrace and the delta is a velocity change; see that file.
//  0x6F4A9ED0 - `retn 0Ch`.
ADDR_THUNK(void SFloatSink::SetSpan(const CFloat*, int, int), 0x6F4A9ED0)
//  0x6F4A9B00 - `retn 4`.
ADDR_THUNK(void SFloatSink::SetValue(const CFloat*), 0x6F4A9B00)


//----------------------------------------------------------------------------
//  The controller side of the camera.  All three binders are genuine
//  __fastcall - the controller in ecx, the slot in edx and three stack
//  dwords the callee cleans (`retn 0Ch`).
//----------------------------------------------------------------------------
//  All three are reconstructed now - GameUI/controllerslotbind.cpp - and
//  their redirects are gone with them.
//  0x6F4E8260 is reconstructed now - GameUI/controllerslotquery.cpp - and
//  its redirect is gone with it.  It reads slot N's three floats out of
//  whatever object it is handed, which is a camera controller at one call
//  site and a sprite at the other; one shipped address, one C++ name.

//----------------------------------------------------------------------------
//  The nine callbacks the binder registers.  None is called from anywhere
//  in this slice - only had its address taken - and every one of them is
//  `retn 4`, which is exactly what the __stdcall declaration below puts
//  there.
//----------------------------------------------------------------------------
ADDR_THUNK(void __stdcall PublishSlotToProp(void*), 0x6F469410)
ADDR_THUNK(void __stdcall CameraOnTransformSlot(void*), 0x6F304AA0)
ADDR_THUNK(void __stdcall CameraOnTargetSlot(void*), 0x6F302570)
ADDR_THUNK(void __stdcall CameraOnRotationSlotLocked(void*), 0x6F302530)
ADDR_THUNK(void __stdcall CameraOnZoomSlot(void*), 0x6F3025D0)
ADDR_THUNK(void __stdcall CameraOnModeEnter(void*), 0x6F305030)
ADDR_THUNK(void __stdcall CameraOnModeLeave(void*), 0x6F3051B0)

//  The two slot callbacks CWorldFrameWar3::SetDayNightModels registers on
//  each day/night sprite - the ramp binder's on slot 0 and the direct
//  binder's on slot 1, both with the world frame as their context.  Neither
//  is called from anywhere in this tree and neither has a dump; both are
//  `retn 4`, read off the image with `ida_query bytes` at the last three
//  bytes of each range (0x6F390450 and 0x6F38DBBA), which is exactly what
//  the __stdcall declaration puts there.  Named for the binding site, since
//  nothing here says what either one computes.
ADDR_THUNK(void __stdcall WorldFrameOnDayNightRampSlot(void*), 0x6F3903F0)
ADDR_THUNK(void __stdcall WorldFrameOnDayNightDirectSlot(void*), 0x6F38DB90)

//----------------------------------------------------------------------------
//  Members, not free functions: every one of these call sites leaves only
//  the receiver in ecx and pushes the rest.
//----------------------------------------------------------------------------
//  0x6F308CA0 - CCameraWar3::SetCursorMode is a real body now, in
//  GameUI/cameracursormode.cpp (CItem vtable-closure batch 2). That file
//  reaches 0x6F306B80 through its own naked thunk, `StartCursorModeTransition`
//  - a second, independently-added name for the exact same address as
//  `ApplyPresetBlend` right below (two sibling batches each thunked
//  0x6F306B80 under a different name before landing; both compile to the
//  identical `mov eax,0x6F306B80/jmp eax` naked body, so this is redundant
//  but not a conflict - `CCameraWar3::SetTargetDistancePreset`
//  (GameUI/cameracursorpreset.cpp) calls it as `ApplyPresetBlend`, so both
//  names stay declared).
//  0x6F306B80 - CCameraWar3::ApplyPresetBlend: 150 instructions across ten
//  calls into the still-unreconstructed sub_6F305A60 (349 instructions of
//  its own) - below this slice's size budget; thunked.
ADDR_THUNK(void CCameraWar3::ApplyPresetBlend(float, int), 0x6F306B80)
//  0x6F305250 / 0x6F3053F0 - genuine __except_handler4-shaped SEH frames
//  (docs/msvc-vc8-idioms.md) around a sin/cos modifier install; thunked.
ADDR_THUNK(void CCameraWar3::InstallRampModifier(int, int, float), 0x6F305250)  // (index, clear, duration)
ADDR_THUNK(void CCameraWar3::InstallOffsetModifier(int, int, float), 0x6F3053F0)  // (index, clear, duration)
//  0x6F304190 - CCameraWar3::StopSlot is reconstructed now, in
//  GameUI/camerastopslot.cpp; the redirect that stood in for it here is gone.
//  0x6F38ECD0 - CWorldFrameWar3::SetCamera is reconstructed now, in
//  GameUI/worldframesetcamera.cpp, so the redirect that stood in for it here
//  is gone: one shipped address, one C++ name.
//  0x6F606850 - CWorldFrameWar3::NotifyControllerChanged is reconstructed in
//  Frame/framecursorcontroller.cpp; the redirect that stood in for it here is
//  gone.
//  0x6F3415A0 and 0x6F3415B0 are reconstructed now - GameUI/chatbarpost.cpp -
//  and the two redirects that stood in for them here are gone with them.

//  0x6F615B50 - CTextFrame::SetText, the editable line's own text setter.
//  `retn 8`.  Declared on CTextFrame in Frame/frame.h; its module has not
//  landed, so the address is still a redirect.
ADDR_THUNK(void CTextFrame::SetText(const char*, int), 0x6F615B50)

//  The two circle-geometry builders CSelectionManager::RebuildCircleGeometry
//  picks between.  Both `retn 0`.
ADDR_THUNK(void CSelectionManager::BuildScaledCircleGeometry(), 0x6F380B90)
ADDR_THUNK(void CSelectionManager::BuildDefaultCircleGeometry(), 0x6F380330)

//  0x6F337A00 - the tooltip host's five-argument text setter.  `retn 14h`,
//  i.e. all five arguments on the stack and only `this` in ecx.
ADDR_THUNK(void CUIToolTipHost::SetText(const char*, const char*,
                                        int, int, int), 0x6F337A00)
//  0x6F34F300 - re-lay a tooltip out.  `retn 0`.
ADDR_THUNK(void CUIToolTipFrame::RefreshToolTipLayout(), 0x6F34F300)

//  0x6F367FF0 is a real constructor now - GameUI/chateditbar.cpp - and the
//  redirect that stood in for it here is gone with it.

//  0x6F2F7170 - CGameUI::GetLastTrackedTarget is a real body now, in
//  GameUI/gameuilasttracked.cpp (CItem vtable-closure batch 2).

//  CSelectMode's two embedded sub-objects' constructors, re-declared here
//  against local copies of the two structs (GameUI/selectmodector.cpp owns
//  the real ones) - which mangles identically, because MSVC puts only the
//  class *name* in a member's mangled name.
struct SSelectModeRequestSlot { void Construct(int arg); };
struct SSelectModeEventSlot   { void Construct(); };
//  0x6F2C9670 - `retn 4`.
ADDR_THUNK(void SSelectModeRequestSlot::Construct(int), 0x6F2C9670)
//  0x6F62BBD0 - `retn 0`.
ADDR_THUNK(void SSelectModeEventSlot::Construct(), 0x6F62BBD0)

//  0x6F2FBE80 is reconstructed now - GameUI/agenttimerarrayalloc.cpp - and
//  the redirect that stood in for it here is gone with it.

//  The waypoint-indicator array's own two out-of-line helpers and its
//  element constructor.  Re-declared here against a local copy of the class
//  for the reason the others in this file give.
struct SWaypointIndicatorArray
{
    void Destruct();
    void SetAlloc(unsigned int alloc);
};
//  0x6F38EDB0 - `retn 0`.
ADDR_THUNK(void SWaypointIndicatorArray::Destruct(), 0x6F38EDB0)
//  0x6F396420 - `retn 4`.
ADDR_THUNK(void SWaypointIndicatorArray::SetAlloc(unsigned int), 0x6F396420)
//  0x6F38FA30 - `retn 0`.
ADDR_THUNK(void __fastcall ConstructWaypointIndicator(SWaypointIndicator*),
           0x6F38FA30)

//  The world frame's eight-slot array's own two out-of-line helpers.
struct SWorldFrameSlotArray
{
    void Destruct();
    void SetAlloc(unsigned int alloc);
};
//  0x6F396170 - `retn 0`.
ADDR_THUNK(void SWorldFrameSlotArray::Destruct(), 0x6F396170)
//  0x6F3962E0 - `retn 4`.
ADDR_THUNK(void SWorldFrameSlotArray::SetAlloc(unsigned int), 0x6F3962E0)

//  0x6F3659A0 - the ramp callback CTimeOfDayIndicator binds on slot 0 of its
//  own sprite.  Never called from this tree, only had its address taken;
//  `retn 4`.
ADDR_THUNK(void __stdcall OnTimeOfDayRampSlot(void*), 0x6F3659A0)

//  0x6F0074F0 - AcquireGameSingleton is reconstructed for real in
//  GameUI/acquiregamesingleton.cpp now (the checksum-provider-registry
//  investigation identified index 6's own object, CGameWar3).

//  CWorldFrameWar3's two player-mask helpers.  `retn 0Ch` and `retn 0`.
ADDR_THUNK(void CWorldFrameWar3::BuildPlayerMaskTable(SPlayerWordTable*,
                                                      unsigned int, int),
           0x6F38DEA0)
ADDR_THUNK(void CWorldFrameWar3::RecolorIndicatorsForLocalPlayer(),
           0x6F38DE10)

//  0x6F0E7410 - the 'abil' checked slot's constructor (its Assign is
//  0x6F0DEB90, whose required type comes from the 'abil' tag getter at
//  0x6F0211D0).  `retn 4`.
ADDR_THUNK(void SCheckedAbilitySlot::Construct(CAgent*), 0x6F0E7410)

//  0x6F34F850 - the command button's own 0x6D8-byte record.  `retn 0`, and
//  it hands `this` back.
ADDR_THUNK(CCommandButtonData* CCommandButtonData::Construct(), 0x6F34F850)

//  --- the chat edit bar's two children and its own backdrop setter --------
//  Every one of these is declared on GameUI/chateditbar.cpp's own copy of the
//  class; MSVC puts only the class *name* in a member's mangled name, so the
//  two declarations are the one symbol.
class CTextFrame;
class CChatPrefixLabel
{
public:
    void SetFont(const char* path, float height, int flags);    // 0x6F5FB960
};
class CChatEntryBox
{
public:
    void SetMaxLines(int lines);                                // 0x6F616250
};

ADDR_THUNK(void CChatPrefixLabel::SetFont(const char*, float, int), 0x6F5FB960)
//  0x6F611EB0 / 0x6F611F90 / 0x6F613CA0 / 0x6F613D60 / 0x6F613D90 are real
//  reconstructions now - GameUI/chattextlines.cpp - and the five redirects
//  that stood in for them here are gone with them.  0x6F616250
//  (CChatEntryBox::SetMaxLines) is still a thunk; see that file's own note
//  on why.
ADDR_THUNK(void CChatEntryBox::SetMaxLines(int), 0x6F616250)
ADDR_THUNK(void SSelectionHost::SetBackdropArt(const char*, int, int,
                                               const char*, int), 0x6F6212D0)
ADDR_THUNK(CTextFrame* __fastcall SSelectionHost::CreatePrefixLabel(int, int),
           0x6F6121E0)
ADDR_THUNK(CTextFrame* __fastcall SSelectionHost::CreateEntryBox(int, int),
           0x6F341210)

//  --- the cinematic filter's four array helpers ---------------------------
//  The four types come in with gameui.h now (GameUI/cinematicfilter.h): the
//  world frame embeds a CCinematicFilter at +0x254 and has to name it, so
//  they are no longer local to a translation unit and re-declaring them here
//  would be a redefinition.
ADDR_THUNK(void SFilterWordArray::SetAlloc(unsigned int), 0x6F34C1E0)
ADDR_THUNK(void SFilterPairArray::SetAlloc(unsigned int), 0x6F34C150)
//  0x6F311880 - SFilterIntArray::SetAlloc is a real body now, in
//  GameUI/filterintarraysetalloc.cpp (CItem vtable-closure batch 2).

//  0x6F337380 - the command button's own icon alpha.  `retn 4`.
ADDR_THUNK(void CCommandButton::SetIconAlpha(float), 0x6F337380)

//  0x6F33FB10 / 0x6F33FB70 - the inventory bar's slot array, both `retn 4`.
//  Declared on a local view of the class rather than in the header, the way
//  the rest of this file's array helpers are.
struct SInventorySlotArray
{
    unsigned int ComputeChunk(unsigned int count);
    void         SetAlloc(unsigned int alloc);
};
ADDR_THUNK(unsigned int SInventorySlotArray::ComputeChunk(unsigned int),
           0x6F33FB10)
ADDR_THUNK(void SInventorySlotArray::SetAlloc(unsigned int), 0x6F33FB70)

//  0x6F359CC0 - CStatBar's own constructor.  `retn 0Ch`, and it hands `this`
//  back.
ADDR_THUNK(CStatBar* CStatBar::Construct(void*, int, int), 0x6F359CC0)

//  0x6F369770 - CInfoPanelGroup's own cell builder.  `retn 0`.
ADDR_THUNK(void CInfoPanelGroup::PopulateGrid(), 0x6F369770)

//  The two rally-indicator arrays' own resize helpers, both `retn 4`.
ADDR_THUNK(void CWorldFrameWar3::SRallyIndicatorArray::SetCount(unsigned int),
           0x6F398F70)
ADDR_THUNK(void CWorldFrameWar3::SRallyOrderArray::SetAlloc(unsigned int),
           0x6F38CFE0)

//  The minimap marker array's two out-of-line helpers, both `retn 4`.
struct SMinimapMarkerArray
{
    unsigned int ComputeChunk(unsigned int count);
    void SetAlloc(unsigned int alloc);
};
ADDR_THUNK(unsigned int SMinimapMarkerArray::ComputeChunk(unsigned int),
           0x6F33FC80)
ADDR_THUNK(void SMinimapMarkerArray::SetAlloc(unsigned int), 0x6F35D150)

//  0x6F30B250 - RegisterAnimNameTokens is a real body now, in
//  GameUI/animnametokenizer.cpp (CItem vtable-closure batch 2).

//  0x6F47C490 (SUnitTrackedRef::SetWrappedValue) used to be redirected
//  here.  It is a real body now - src/Math/floatmodstepset.cpp.

//----------------------------------------------------------------------------
//  SPointerArray's own two out-of-line helpers (pointerarrayappend.cpp).
//  Re-declared here against a local copy of the struct, which mangles the
//  same way because MSVC puts only the class *name* in the mangled name.
//----------------------------------------------------------------------------
//  0x6F2F56B0 / 0x6F2F5710 - real bodies now, in GameUI/pointerarraygrow.cpp
//  (CItem vtable-closure batch 2).

//----------------------------------------------------------------------------
//  The skin side.  The table itself, its two Find methods and the value
//  lookup are reconstructed in gameuiskin*.cpp by the JASS_PauseGame target,
//  which reached the same four functions from the other direction; only the
//  art-path applier below is still a redirect.
//----------------------------------------------------------------------------
//  0x6F3208C0 - ApplyFrameArtPath is a real body now, in
//  GameUI/frameartpath.cpp (CItem vtable-closure batch 2).
//  0x6F002400 - CMiscCustom::GetString was thunked here while this class's
//  own body was unwritten.  It is written now (src/Config/CMiscCustom.cpp),
//  so the redirect is gone: two definitions of one symbol is an LNK2005 the
//  real build downgrades to LNK4006 under /FORCE:MULTIPLE, which lets the
//  first object on the link line win - and that is not necessarily the real
//  body.  See tools/link_check.py.

//  0x6F4E7C40 / 0x6F4E9F80 - the two controller factories, both `retn 0`.
//  Reconstructed for real now (src/GameUI/cameracontrollercreate.cpp and
//  cameraeventsinkcreate.cpp), so the redirects are gone: two definitions
//  of one symbol is an LNK2005 the real build downgrades to LNK4006 under
//  /FORCE:MULTIPLE, and the first object on the link line - not necessarily
//  the real body - would win.

//----------------------------------------------------------------------------
//  The two constructors this target's dump roots that are redirects rather
//  than reconstructions, and why.
//
//  Both are enormous single bodies wrapped in an __except_handler4 frame
//  with a double dozen unwind states - the frame this repo's fixed
//  /EHs-c- /GS- cannot emit at all - and both spend most of their length
//  initialising members of an object this repo has established no layout
//  for.  A partial constructor is not a partial match: it is a live defect,
//  because every field it fails to write is a field the real, unhooked
//  callers go on to read.  Redirecting hands the game its own constructor
//  and leaves the object exactly as the shipped build leaves it.
//
//  Deleting either redirect is what landing its real body looks like; the
//  first thing that needs is the class layout, which is the expensive part
//  and not this session's.
//----------------------------------------------------------------------------
//  0x6F2FE9F0 - CGameUI::CGameUI.  1973 instructions (445 of them the
//  appended unwind funclets), a 0x148-byte frame, two /GS cookies and
//  twenty-odd sub-objects.  `retn 0` - __thiscall with no arguments.
ADDR_THUNK(CGameUI::CGameUI(), 0x6F2FE9F0)
//  0x6F36D9F0 - CMinimap::CMinimap.  825 instructions (190 of them unwind
//  funclets), fourteen unwind states, two `eh vector constructor iterator`
//  member arrays and an object over 0x818 bytes wide.  `retn 4` - the parent
//  frame it hands straight to CFrame::CFrame.
//
//  CMinimap is re-declared here against nothing but its constructor, the
//  same local-declaration trick GameUI/minimaprefresh.cpp,
//  minimapbuttonskins.cpp and worldframeminimap.cpp each use for their own
//  slice of the class: MSVC puts only the class *name* in the mangled name,
//  so all four still meet at link time.  The class has no shared header
//  because no reconstruction in this repo has established its layout - see
//  the note above.
class CMinimap
{
public:
    CMinimap(void* parent);
};
ADDR_THUNK(CMinimap::CMinimap(void*), 0x6F36D9F0)

#undef ADDR_THUNK
