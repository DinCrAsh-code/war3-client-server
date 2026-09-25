//============================================================================
//  CGameUI, the controllers `JASS_PauseGame` walks, and the CCameraWar3 the
//  JASS camera natives go through.
//
//  Two targets met in this file - JASS_PauseGame from CGameUI's own side and
//  JASS_PanCameraToTimedWithZ from the camera's - so it carries both, and
//  every offset below is one or the other's disassembly, not an inference.
//
//  The class name is ground truth and not a guess: the singleton accessor's
//  own Storm allocation tag names ".\\CGameUI.cpp" (see
//  Widget/selectable_attachcreate.cpp, which reached 0x6F300710 first).
//  Everything below is the slice of it these targets touch - offsets are
//  exact, gaps are `m_reservedNN` named for the hex offset they start at,
//  and no member the call trees do not read has been invented.
//
//  The camera has no allocation tag of its own, but its agile type id names
//  it: 0x6F301E10 returns 0x2B63616D, '+cam' read most significant byte
//  first, which is the tag SCheckedCameraSlot::Assign (cameratypedassign.cpp)
//  checks candidates against.
//
//  **The class name below used to be `CGameCamera`, spelled out from that
//  '+cam' tag - a guess, and its own comment said so.**  It was wrong.  The
//  constructor at 0x6F307B00 (asm/sub_6F307B00_0x6F307B00_calltree_asm.md,
//  now archived) stamps `mov dword ptr [esi], offset ??_7CCameraWar3@@6B@`
//  into `this` as its very first field write after the base-class call -
//  this session's own RTTI/vtable ground truth, and outranks an agile-type
//  inference the same way a `## User notes` section or a `user_knowledge.json`
//  fact would.  Every field offset that constructor touches lines up exactly
//  with what this file already had under the old name (the nine `SCameraProp`
//  properties from +0xCC, the `CPathRef m_target` at +0x1A4, the `FVec3
//  m_offset` at +0x50 PanTo already used), so the rename is a name change
//  only - nothing about the layout or the already-matching methods below
//  moved.  `docs/targets/vtables/CCameraWar3.txt` and
//  `user_knowledge.json`'s own `CCameraWar3` vtable entry (24 slots,
//  `0x6F9365AC`) are the same class; the base is `CAgent`
//  (`sub_6F001730`/`CAgent::CAgent()` is the first call the constructor
//  makes), consistent with the nine inherited-unchanged slots (`ReleaseSelf`,
//  `Dispatch`, the two `CObserver` message slots, `Method_0x18`,
//  `GetRecordVersion`, `Method_0x50`, `Method_0x54`, `Method_0x5C`) in that
//  vtable dump.
//
//  **The camera works in hardware floats, not in CFloat.**  Every value it
//  stores and computes goes through the x87 (`fld` / `fstp` / `fmul`), not
//  through the software float library the simulation uses - which makes
//  sense: nothing about where the local player's camera points has to be
//  bit-identical across machines.  The one place the two meet is the
//  FloatProp/Position layer it publishes through (SUnitTrackedRef,
//  unittrackedref.h; CPathRef, game.h), whose setters take a `const CFloat*`;
//  the shipped code hands them the address of a plain float local, which is
//  what AsCFloat() below spells.
//============================================================================
#ifndef GAMEUI_H
#define GAMEUI_H

#include "observer.h"
#include "agent.h"
#include "agenttimer.h"
#include "fvec3.h"
#include "unittrackedref.h"
#include "game.h"
#include "agenttypedslots.h"
#include "frame.h"
#include "modelframe.h"
#include "ghostimageslot.h"
#include "cinematicfilter.h"

//----------------------------------------------------------------------------
//  A hardware float local, seen as the encoded CFloat the FloatProp and
//  Position setters take.  The two have the same representation - CFloat.h
//  says so in its first paragraph - and the shipped code simply pushes the
//  address of the float.
//----------------------------------------------------------------------------
inline const CFloat* AsCFloat(const float& v) { return (const CFloat*)&v; }

//----------------------------------------------------------------------------
//  One camera property as the camera embeds it: SUnitTrackedRef's four
//  words (unittrackedref.h) followed by the counted reference to whatever
//  modifier is currently driving it.  0x18 bytes - which is not a guess,
//  it is the stride between the nine consecutive properties
//  CCameraWar3::ResetToDefaults writes at +0xCC through +0x18C, and it is
//  where CCameraWar3::PanTo parks the FloatModifier it makes for the
//  property at +0x1D0 (its reference lands at +0x1E0).
//----------------------------------------------------------------------------
struct SCameraProp
{
    SUnitTrackedRef m_value;       // +0x00
    //  +0x10 / +0x14 - two counted references to whatever is currently
    //  driving this property.  Only two of them are ever touched in this
    //  tree, and each under a different name: PanTo installs the
    //  FloatModifier it makes into the +0x10 of the property at +0x1D0,
    //  and CancelModifiers walks the +0x14 of the one at +0x4AC.
    TRefCnt*        m_pModifier;   // +0x10
    TRefCnt*        m_pModifier2;  // +0x14
};

//----------------------------------------------------------------------------
//  A counted reference held in one word, and the only two operations this
//  call tree performs on one.  Assign is 0x6F029580 (countedrefassign.cpp);
//  Release is 0x6F029560, which trivialaccessors_08.cpp already
//  reconstructs under its own name and which is not re-declared here.
//----------------------------------------------------------------------------
struct SCountedRef
{
    //  0x6F029580 - adopt `other`'s referent: addref the new one, release
    //  the old one, store, and hand `this` back.
    SCountedRef* Assign(const SCountedRef* other);

    TRefCnt* m_ref;   // +0x00
};

class CCameraWar3;
class CGameUIPortraitPanel;
class CPortraitButton;

//----------------------------------------------------------------------------
//  CGameUI +0x3FC: the observer host three of PostCameraEvents' four camera
//  events go out through instead of through CGameUI itself.  Declared with
//  exactly the two members that call site reaches and no layout at all,
//  because nothing in either tree establishes any.
//
//  Both are *members* rather than __fastcall free functions taking the
//  receiver: the shipped call sites leave only the receiver in ecx and push
//  every other argument, which is __thiscall, and a __fastcall would take
//  the first of them in edx and clean four bytes too few - the exact
//  `ret <n>` mismatch tools/thunk_abi_audit.py exists to catch.
//----------------------------------------------------------------------------
//  What this object *is* was settled by this session's 0x6F341xxx slice, and
//  the name above no longer describes it.  Every one of the seven methods on
//  it belongs to the game's **chat input bar**: the CGameUI constructor makes
//  it with FDF frame id 0x2422 (0x6F2FF6CA), refcounts it, sets its height
//  through its own CLayoutFrame base at +0xB4 (0x6F2FF701) - so it is a
//  CFrame subclass, not a bare record - and 0x6F3412F0 fills the label at
//  +0x1E4 with one of the six "COLON_MESSAGE_*" game strings ("all", "allies",
//  "observers", "referees", a player name, single-player).  +0x1E0 is the
//  editable line itself: 0x6F341410 pushes text into it through CTextFrame's
//  0x6F615B50, and 0x6F341460 hands it the keyboard focus.
//
//  The right name for it is CGameUIChatBar.  It keeps the old one because
//  another session wrote it and this session's brief says a disagreement is
//  reported, not silently rewritten - see the write-up.  Nothing below is
//  invented: every offset is a load or a store in one of those seven bodies.
//----------------------------------------------------------------------------
//  Storm's growable array of plain pointers, as CGameUI embeds it.  The
//  layout is the one GameUI/pointerarrayappend.cpp already established from
//  Append's own arithmetic; SetCount is written here and inline because the
//  shipped CGameUI::PopTrackedTarget carries the whole of it with no call
//  between the copies, which is what /Ob2 does to a body this size.
//----------------------------------------------------------------------------
struct SPointerArray
{
    //  0x6F2F79B0 - GameUI/pointerarrayappend.cpp.
    unsigned int Append(unsigned int count, void* const* values);
    //  0x6F2F56B0 / 0x6F2F5710 - below this slice; thunked.  Each `retn 4`.
    unsigned int ComputeChunk(unsigned int count);
    void SetAlloc(unsigned int alloc);

    //  Grow to hold `count` and take it as the new count.  The grow happens
    //  only when the array is short of it on *both* counts - more than the
    //  live count and more than the allocation - which is why shrinking
    //  never reallocates.
    __forceinline void SetCount(unsigned int count)
    {
        if (count > m_count && count > m_alloc)
        {
            unsigned int chunk = m_chunk;
            if (chunk == 0)
                chunk = ComputeChunk(count);

            unsigned int alloc = count;
            unsigned int over = count % chunk;
            if (over != 0)
                alloc = count - over + chunk;

            SetAlloc(alloc);
        }

        m_count = count;
    }

    unsigned int m_alloc;   // +0x00
    unsigned int m_count;   // +0x04
    void**       m_data;    // +0x08
    unsigned int m_chunk;   // +0x0C
};

//----------------------------------------------------------------------------
//  What CGameUI tracks at +0x1B4 and remembers in the array after it.  Five
//  vtable slots and no fields: nothing in this tree reads a member of one.
//
//  **Every one of these five dispatches is on a receiver that is not `this`,
//  so tools/vtable_dispatch_audit.py cannot judge any of them, and there is
//  no committed vtable for the class to check them against either.**  What
//  fixes each argument count is the shipped call site itself: all five push
//  nothing at all and set only ecx (0x6F2FB81B, 0x6F2FB85D, 0x6F2FB88B,
//  0x6F2FB8F7, 0x6F2FB84F), so all five are no-argument __thiscall calls and
//  a wrong count is not possible in the direction that shifts a frame.
//  Each is named for what its caller does with the answer.
//----------------------------------------------------------------------------
struct STrackTarget
{
    //  slot 0x18 - gates everything: an untrackable target hands the
    //  portrait back to the camera and is never honoured.
    int IsTrackable()
    {
        typedef int (__thiscall *Fn)(void*);
        return ((Fn)(*(void***)this)[0x18 / 4])(this);
    }
    //  slot 0x1C - may the outgoing target go on the history stack?
    int IsWorthRemembering()
    {
        typedef int (__thiscall *Fn)(void*);
        return ((Fn)(*(void***)this)[0x1C / 4])(this);
    }
    //  slot 0x20 - does taking this target take the camera with it?
    int TakesOverCamera()
    {
        typedef int (__thiscall *Fn)(void*);
        return ((Fn)(*(void***)this)[0x20 / 4])(this);
    }
    //  slot 0x24 - last word: a "no" undoes the whole switch and pops the
    //  previous target back instead.
    int AcceptsTracking()
    {
        typedef int (__thiscall *Fn)(void*);
        return ((Fn)(*(void***)this)[0x24 / 4])(this);
    }
    //  slot 0x28 - told to the outgoing target, unconditionally.
    void OnTrackLost()
    {
        typedef void (__thiscall *Fn)(void*);
        ((Fn)(*(void***)this)[0x28 / 4])(this);
    }
};

class CGameUI;

//  The base is CBackdropFrame and not CFrame: 0x6F367FF0 calls 0x6F620ED0,
//  which calls 0x6F621B70, which calls CFrame's own constructor - three links
//  and all three now reconstructed (src/Frame/modelframe.h).  CBackdropFrame
//  is 0x1DC bytes, which is exactly where this class's own fields start, so
//  nothing moved when the block of padding that used to stand for them was
//  replaced by the real classes.
//
//  **The shipped name of this class is `CChatEditBar`.**  Its constructor
//  stamps ??_7CChatEditBar@@6B@ at +0x00 and its `_0` twin at +0x0B4
//  (0x6F368058).  `SSelectionHost` is this repo's older name for it, from
//  the side that reached it as CGameUI +0x3FC, and it is kept here because
//  renaming it would rename seven funcmap keys another slice owns - but it
//  is wrong, and the evidence that it is wrong is one instruction.
struct SSelectionHost : public CBackdropFrame
{
    //  +0x1DC - is the bar accepting typing right now?  Written by
    //  SetEntryActive and read by nothing in this tree.
    int         m_entryActive;      // +0x1DC
    //  +0x1E0 - the editable line.  A CTextFrame: 0x6F615B50, which
    //  SetEntryText calls on it, works on CTextFrame's own +0x1E4 capacity
    //  and +0x1F0 style words.
    CTextFrame* m_pEntryBox;        // +0x1E0
    //  +0x1E4 - the prefix label to the left of it ("All:", "Allies:", ...).
    CTextFrame* m_pPrefixLabel;     // +0x1E4
    //  +0x1E8 - which of those prefixes is showing, in the same numbering
    //  GetLocalChatTarget returns.
    int         m_colonTarget;      // +0x1E8

    //  0x6F3415A0 - straight into this host's own observer slot 2.
    //  `retn 0Ch`.
    void DispatchRange(unsigned int low, unsigned int high, void* target);
    //  0x6F3415B0 - into CObserver::PostEvent on its +0x1E0 sub-object.
    //  `retn 8`.
    void Post(unsigned int id, void* target);
    //  0x6F341410 - replace the editable line's text.  `retn 4`.
    void SetEntryText(const char* text);
    //  0x6F341430 - bind the editable line's alternate listener for one
    //  event.  `retn 0Ch`.
    void SetEntryAltEventListener(int event, void* listener, int enable);
    //  0x6F3412F0 - pick and show the "COLON_MESSAGE_*" prefix for `target`.
    //  `retn 4`.
    void SetColonMessage(int target);
    //  0x6F341460 - open or close the bar for typing.  `retn 4`.
    void SetEntryActive(int active);
    //  0x6F6212D0 - hand the backdrop its two skin paths and its alpha.
    //  `retn 14h`: five pushed dwords with `this` alone in ecx.  Still a
    //  redirect.
    void SetBackdropArt(const char* background, int alpha, int a,
                        const char* border, int b);
    //  0x6F6121E0 / 0x6F341210 - build the prefix label and the entry box.
    //  Both `retn 4` and both genuine __fastcall members: the shipped call
    //  sites leave `this` in ecx *and* a second argument in edx and push only
    //  one dword.  Still redirects.
    CTextFrame* __fastcall CreatePrefixLabel(int a, int b);
    CTextFrame* __fastcall CreateEntryBox(int a, int b);
    //  0x6F367FF0 - the constructor: it is what loads the "ToolTipBorder",
    //  "ToolTipBackground", "ChatEditBar" and "ChatFont" skins, which is the
    //  last piece of evidence that this class is the chat bar.  `retn 0Ch`.
    //  GameUI/chateditbar.cpp - a real constructor now, which is what let the
    //  base chain above be spelled out.
    SSelectionHost(CGameUI* ui, void* owner, int frameId);
};

//  0x6F3412D0 - which chat target the local player's own console is set to,
//  in the numbering SetColonMessage switches on: 0 all, 1 allies, 2
//  observers/referees, 3 + slot for a private message to one player.
int GetLocalChatTarget();


class CWorldFrameWar3;
class CScreenFrame;

//----------------------------------------------------------------------------
//  The five controllers CGameUI enables and disables when the game pauses.
//  Every one of them takes the same single `enabled` argument (0 on pause,
//  1 on resume) and caches it, so they are named for what they hold rather
//  than for a slot number.
//----------------------------------------------------------------------------

//  +0x3C4 - a flat list of screen frames with an extra "already inactive"
//  flag; the list itself lives one indirection away at +0x148.
class CGameUIItemBar;
//  +0x3C8 - a 3 x 4 grid of frames reached through +0x154, i.e. the command
//  card.  Three rows of four is what the two nested loop bounds (0x30 by
//  0x10, then 0x10 by 4) say and nothing else fits them.
class CGameUICommandCard;
//  +0x3DC - seven frames off the same +0x154 table, each of which votes on
//  its own enabled state through the object at its +0x1C4.
class CGameUIHeroBar;
//  +0x3E0 - one frame at +0x134 and the cached flag at +0x150.
class CGameUIMiniMapPanel;
//  +0x420 - the portrait/tip panel: a checked selectable at +0x124 and the
//  frame it drives at +0x128.
class CGameUIPortraitPanel;
//  +0x228 - really a `CEscMenu*` (GameUI/escmenu.h, confirmed structurally -
//  see this class's own comment below), kept under this stand-in name for
//  the one slice of it (the caption rewrite) this target ever touched.
class CGameUIPauseButton;
//  +0x214 - the overlay that owns the current selection's visuals.
class CGameUISelectionOverlay;
//  The real class at CGameUIPauseButton's own +0x18 (see CEscMenuMainPanel
//  below) is a CEscMenu*, not reconstructed here - a sibling target.
//  The class at CEscMenuMainPanel's own +0x190 - see CEscMenuMainPanel's
//  own comment for the vtable evidence behind this identification.  Only
//  forward-declared: no constructor or field of it has been reconstructed
//  in this repo yet (a separate, sibling target).
class CGlueTextButtonWar3;

//  Derived from CObserver, which PostCameraEvents (0x6F2F5EE0) settles: it
//  reaches CObserver::Dispatch through this object's own vtable slot 2 and
//  calls CObserver::PostEvent by name on `this`, exactly the way
//  CWidget::PostAgentEvent does (widget_postagentevent.cpp).  The base is
//  0x0C bytes - vtable, refcount, resource - so the first reserved block
//  starts there rather than at zero.
//----------------------------------------------------------------------------
//  CGameUI +0x23C's target.  Two fields and one indirection, all of them
//  0x6F3412D0's own loads: the live record hangs off +0x0C, its +0x1A0 says
//  which kind of chat target is selected and its +0x1A4 carries the player
//  slot when that kind is "one player".
//----------------------------------------------------------------------------
struct SChatTarget
{
    char m_reserved000[0x1A0];
    int  m_kind;        // +0x1A0
    int  m_slot;        // +0x1A4
};

struct SChatConsole
{
    char         m_reserved00[0x0C];
    SChatTarget* m_pTarget;  // +0x0C
};

class CGameUI : public CObserver
{
public:
    //  The font cache's node type.  Its name is the shipped one: the cache's
    //  own constructor (0x6F2FE340) stamps
    //  `??_7?$TSHashTable@UCCachedFont@CGameUI@@VHASHKEY_STR@@@@6B@`, which
    //  spells `struct CGameUI::CCachedFont` as the table's element.  Nothing
    //  this tree reaches reads a field of one - the only body of the
    //  instantiation that is reconstructed is the list's UnlinkAll, which
    //  works entirely through the link offset - so no layout is invented for
    //  it here - the type is defined and empty rather than left incomplete
    //  only because an incomplete element type stops MSVC inlining
    //  TSLink<T>::Unlink into the list walk, which costs that walk fourteen
    //  of its twenty-eight instructions.
    struct CCachedFont { };

    char          m_reserved00C[0x1AC - 0x0C];
    //  Both have to be set for the world view to be up; nothing in this tree
    //  reads either for anything else.
    int           m_viewReadyA;          // +0x1AC
    int           m_viewReadyB;          // +0x1B0
    //  +0x1B4 - what the UI is tracking right now, and the stack of what it
    //  was tracking before.  Both are named by SetTrackedTarget and
    //  PopTrackedTarget, which are the only things in this tree that touch
    //  them.
    STrackTarget* m_pTrackedTarget;      // +0x1B4
    SPointerArray m_trackHistory;        // +0x1B8 .. +0x1C8
    char          m_reserved1C8[0x08];
    //  +0x1D0 / +0x1D4 - two option words CPreferencesWar3::ApplyPreference
    //  (Config/preferenceapply.cpp) stores preference ids 42 and 63 into.
    //  Named for their offsets, the `QueryField78` last resort: nothing in
    //  this closure ever *reads* either one, so the only thing established
    //  about them is that a preference sets them and that they are four
    //  bytes wide.
    int           m_option1D0;           // +0x1D0
    int           m_option1D4;           // +0x1D4
    //  +0x1D8 - preference id 43's target as well as the cursor lock.
    int           m_cursorLocked;        // +0x1D8
    char          m_reserved1DC[0x38];
    CGameUISelectionOverlay* m_pSelectionOverlay;   // +0x214
    char          m_reserved218[0x10];
    CGameUIPauseButton*  m_pPauseButton; // +0x228
    char          m_reserved22C[0x04];
    //  +0x230 / +0x234 - the two targets that do *not* close the chat bar
    //  when they are tracked.  Nothing in this tree says what either one is
    //  beyond that exemption, which is what the names record.
    void*         m_pChatExemptTargetA;  // +0x230
    void*         m_pChatExemptTargetB;  // +0x234
    char          m_reserved238[0x04];
    //  +0x23C - the chat console.  GetLocalChatTarget (0x6F3412D0) is the
    //  only reader in this tree: it takes the record at the console's +0x0C
    //  and reads the two words that say who the local player is typing to.
    struct SChatConsole* m_pChatConsole; // +0x23C
    char          m_reserved240[0x14];
    //  +0x254 - the camera.  CGameUI::GetCamera (gameuigetcamera.cpp,
    //  0x6F2F5ED0) is the only reader; the JASS camera natives are what
    //  named it - PanCameraToTimedWithZ reads this field off the singleton
    //  and hands the result to CCameraWar3::PanTo as its `this`.
    CCameraWar3*  m_pCamera;             // +0x254
    int           m_pauseRequested;      // +0x258
    int           m_pauseLatched;        // +0x25C
    int           m_pausedByPlayer;      // +0x260
    char          m_reserved264[0x38];
    int           m_field29C;            // +0x29C
    unsigned int  m_uiUp;                // +0x2A0
    unsigned int  m_circlesLit;          // +0x2A4
    char          m_reserved2A8[0x3B8 - 0x2A8];
    //  +0x3B8 - compared against a resolved unit's own +0x30 owner id
    //  by CUnit::NotifyAbilityHostChanged's own listener-toggle chain
    //  (0x6F300CB0, unit_gameuiabilitylistenernotify.cpp) - almost
    //  certainly "the ability-host object the game UI is currently
    //  showing" (an inventory/hero panel target), but nothing in this
    //  tree reads it beyond that one comparison.
    int           m_trackedAbilityHostId; // +0x3B8
    //  +0x3BC - the world frame itself (0x6F39A910 builds it).  Named for
    //  its class rather than `m_pWorldFrame`, the name a prior session gave
    //  +0x3F4 on the assumption that a second "world frame" pointer must be
    //  the 3D viewport too - it isn't (see m_pPortraitButton3F4 below).
    CWorldFrameWar3* m_pWorldFrameWar3;  // +0x3BC
    //  +0x3C0 - the minimap (0x6F36D9F0 builds it and stamps
    //  ??_7CMinimap@@6B@); SetMinimap hands it to the world frame.
    class CMinimap* m_pMinimapObject;    // +0x3C0
    CGameUIItemBar*      m_pItemBar;     // +0x3C4
    CGameUICommandCard*  m_pCommandCard; // +0x3C8
    char          m_reserved3CC[0x08];
    //  +0x3D4 - the bare CSimpleFrame the five minimap buttons are parented
    //  on.  CreateMiniMapButtons builds it first and hands it to every one
    //  of them.
    class CSimpleFrame*  m_pMiniMapButtonHost;  // +0x3D4
    char          m_reserved3D8[0x04];
    CGameUIHeroBar*      m_pHeroBar;     // +0x3DC
    CGameUIMiniMapPanel* m_pMiniMap;     // +0x3E0
    char          m_reserved3E4[0x08];
    void*         m_pMessageFrame;       // +0x3EC
    char          m_reserved3F0[0x04];
    //  +0x3F4 - a second, distinct CPortraitButton (0x6F36F8D0 constructs
    //  it, confirmed by walking CGameUI::CGameUI's own body back from the
    //  store at +0x3F4 to the `call sub_6F36F8D0` and `??_7CPortraitButton@
    //  @6B@` vtable stamp just before it - not the world/camera viewport a
    //  prior session's `m_pWorldFrame` name assumed.  Its role relative to
    //  m_pPortrait (+0x420, CGameUIPortraitPanel - a different class at a
    //  different offset) is not yet known; do not confuse the two.  See
    //  docs/notes/camera-movable-during-pause-feature.md for why the old
    //  name mattered and how it was found wrong.
    CPortraitButton* m_pPortraitButton3F4; // +0x3F4
    char          m_reserved3F8[0x04];
    //  +0x3FC - the selection host.  PostCameraEvents posts three of its
    //  four camera events through this one rather than through CGameUI.
    SSelectionHost* m_pSelectionHost;    // +0x3FC
    char          m_reserved400[0x08];
    //  +0x408 - the ping button, the only one of the five whose three skins
    //  CreateMiniMapButtons sets itself ("MiniMapSignalButton...").
    class CSimpleButton* m_pMiniMapSignalButton;    // +0x408
    //  +0x40C .. +0x418 - the four minimap toggle buttons, in the order the
    //  four skin-refresh bodies address them.  Each is named for the skin
    //  keys its own refresh resolves ("MiniMapTerrainButton...",
    //  "MiniMapCreepButton...", "MiniMapAllyButton...",
    //  "MiniMapFormation...Button..."), which is the only thing in this tree
    //  that says what any of them is.
    class CSimpleButton* m_pMiniMapTerrainButton;   // +0x40C
    class CSimpleButton* m_pMiniMapAllyButton;      // +0x410
    class CSimpleButton* m_pMiniMapCreepButton;     // +0x414
    class CSimpleButton* m_pMiniMapFormationButton; // +0x418
    char          m_reserved41C[0x04];
    CGameUIPortraitPanel* m_pPortrait;   // +0x420
    //  +0x424 - the frame every free-standing sprite frame is parented on.
    //  CHeroBarButton's constructor reads it straight out of the singleton
    //  and hands it to CSpriteFrame's constructor.
    void*         m_pSpriteHost;         // +0x424
    //  +0x428 - read as an `int` by the unit tip's own factory
    //  (gameuiunittipsingleton.cpp, which is where the name comes from) and
    //  as a *frame pointer* by CreateMiniMapButtons, which parents the
    //  button host on it and anchors all five buttons to its point 6.  The
    //  two readings cannot both be right; the name is left as the earlier
    //  slice wrote it and this one casts.
    int           m_unitTipStyle;        // +0x428

    //  0x6F2FE9F0 - the whole in-game UI, built in one 1973-instruction
    //  body: the font cache, the twelve panels, the world frame at +0x3BC,
    //  the minimap at +0x3C0 and the five Quick* command registrations.
    //  __thiscall, no arguments (`retn 0`).  Still a redirect -
    //  GameUI/gameui_thunks.cpp says why.
    CGameUI();

    void SetGamePaused(int paused, int actingSlot, int notify, int latch,
                       int record);
    //  0x6F2F2F60 / 0x6F2F30C0 / 0x6F2F3000 / 0x6F2F3160 - re-skin one of
    //  the four minimap toggle buttons for the state it is now in.  All four
    //  `retn 0`.  GameUI/minimapbuttonskins.cpp is where the two that read
    //  the minimap's own flags live; the two that read a stored preference
    //  are one file each, because the preference read is a real call.
    void RefreshMiniMapTerrainButtonSkin();
    void RefreshMiniMapCreepButtonSkin();
    void RefreshMiniMapAllyButtonSkin();
    void RefreshMiniMapFormationButtonSkin();
    //  0x6F2F6040 - build the five minimap buttons and their host frame.
    //  `retn 0`.  GameUI/minimapbuttons.cpp.
    void CreateMiniMapButtons();
    //  0x6F2F9CC0 - if `unit` belongs to the locally acting player, call
    //  through the minimap panel's own vtable slot 0x6C/4 (27) once.
    //  `retn 8`, thiscall, two stack args.  GameUI/gameuiminimapunitnotify.cpp.
    void NotifyMiniMapIfLocallyOwned(class CUnit* unit, int flag);
    //  0x6F300CB0 - the CGameUI end of CUnit::NotifyAbilityManagerAndGameUI
    //  (GameUI/gameuiabilitylistenernotify.cpp).  `retn 4`.
    void NotifyAbilityListenerOfOwner(void* owner);
    //  0x6F2F4820 / 0x6F2F4790 - two near-identical zero-argument,
    //  __except_handler4-shaped CGameUI diagnostics
    //  NotifyAbilityListenerOfOwner picks between on the "pause the
    //  command card while typing" preference.  Kept redirects - well
    //  outside this batch's own scope.  `retn 0`.
    void* QueryFollowUpTarget();
    void* QueryFollowUpTargetAlt();
    //  0x6F2F2990 - allocate and construct the chat bar (+0x3FC).  A
    //  __fastcall member: `this` in ecx and `owner` in edx, with the FDF
    //  frame id pushed (`retn 4`).  GameUI/chatbarcreate.cpp.
    SSelectionHost* __fastcall CreateChatBar(void* owner, int frameId);
    //  0x6F2F3F50 - cycle the minimap ally button's stored mode.  `retn 4`.
    //  GameUI/minimapallymode.cpp.
    int AdvanceMiniMapAllyMode(int step);
    //  0x6F2FB7E0 - take `target` as the tracked object, telling the old one
    //  and the new one, the portrait, the camera and the chat bar.
    //  `retn 8`.  GameUI/gameuitracktarget.cpp.
    void SetTrackedTarget(STrackTarget* target, int recordPrevious);
    //  0x6F2FB920 - go back to whatever was tracked before and drop it off
    //  the history stack.  `retn 0`.  GameUI/gameuitrackpop.cpp.
    void PopTrackedTarget();
    //  0x6F2F7170 - the top of that stack, or null when it is empty.
    //  `retn 0`; still a redirect.
    STrackTarget* GetLastTrackedTarget();
    //  0x6F2F5EE0 - tell everything that watches the camera that `camera`
    //  has arrived or gone.  `retn 8`.
    void PostCameraEvents(CCameraWar3* camera, int direct);
    //  0x6F2F5ED0 - the camera at +0x254.  `retn 0`.
    CCameraWar3* GetCamera() const;
};

//----------------------------------------------------------------------------
//  0x6F300710 - the lazily-constructed singleton, `create` in ecx and `reset`
//  in edx.  Spelled *exactly* as Widget/selectable_attachcreate.cpp already
//  defines it, `SGameUI*` return type included: MSVC puts the return type in
//  the mangled name, so a second spelling here would be a second symbol for
//  one shipped address - the defect
//  docs/notes/verifier-gate-link-failures.md catalogues - and would cost
//  every call site below its match, because the canonicaliser compares the
//  funcmap tag and only the original name carries one.  The cast to CGameUI*
//  at each call site costs no instruction.
//----------------------------------------------------------------------------
struct SGameUI;
SGameUI* __fastcall GetGameUI(int create, int reset);

//============================================================================
//  The rest of the classes this batch reconstructs, and the free functions
//  its translation units call across module boundaries.  Everything here is
//  a real body somewhere under src/GameUI - the redirects into the other
//  batches of this closure live in gameuithunks.h.
//============================================================================

//----------------------------------------------------------------------------
//  CSimpleFrame - the frame base this file used to model as ninety bytes of
//  padding, a `m_hidden` flag and one method.  The frame-library batch
//  reconstructed the class's own constructor (0x6F609C70, the one that stamps
//  ??_7CSimpleFrame@@6B@), so the full layout lives in Frame/frame.h now and
//  ::Hide (0x6F2F1010, gameuiframehide.cpp) is declared there with it.  The
//  +0x90 flag is unchanged; it is what that constructor leaves at 1.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  The world-object singleton, `dword_6FAB65F4`.  Two of its members are
//  this batch's; everything else about it (the per-player slot array at
//  IndexedArrayHolder_6F3A1650::At, the +0x28 acting slot) is already
//  reconstructed elsewhere and is reached by casting `this`.
//----------------------------------------------------------------------------
class CGameWorld
{
public:
    void SetTextAgentsPaused(int paused);                   // 0x6F3A2820
    int  TestRelationBit(unsigned int player, int bit);     // 0x6F3A3400

    //  0x6F144B50 (src/Game/gameworldrelationclear.cpp) - the exact
    //  complement of TestRelationBit above: same two arguments, same
    //  call, and `neg`/`sbb`/`add 1` where that one has
    //  `neg`/`sbb`/`neg`, i.e. `== 0` rather than `!= 0`.  Batch A
    //  reached it as a redirect and called it IsRelationSet, which is
    //  the wrong way round; the pause sweep asks it about *enemies*.
    int  TestRelationBitClear(unsigned int player, int bit); // 0x6F144B50

    //  0x6F3A3430 (src/Game/gameworldtestrelationagainstowner.cpp) - a
    //  thin wrapper one module over from TestRelationBit itself: resolves
    //  `widget`'s owning player through its own vtable slot 59 (+0xEC,
    //  CWidget::GetOwningPlayerIndex) and tests `player`'s relation to
    //  that.  Kept out of gameuiplayerenum.cpp (TestRelationBit's own
    //  file) the same way TestRelationBitClear is kept out of it - a call
    //  compiled in the same TU as TestRelationBit's definition gets
    //  inlined by /Ob2, which the shipped stream's own `call sub_6F3A3400`
    //  says did not happen there either.
    int  TestRelationBitAgainstOwner(unsigned int player,
                                      class CWidget* widget);  // 0x6F3A3430

    //  0x6F3A3530 (src/Game/gameworldvisiblerelation.cpp) - the exact same
    //  shape as TestRelationBit, just against `m_visible` (+0x98) rather
    //  than `m_player` (+0x38).  Own TU for the same
    //  compiled-next-to-TestRelationBit inlining reason as
    //  TestRelationBitAgainstOwner/TestRelationBitClear above.
    int  TestVisibleRelationBit(unsigned int player, int bit); // 0x6F3A3530

    //  0x6F3A35C0 (src/Game/gameworldvisiblerelation.cpp) - true if the
    //  player's "detected" mask (+0xA8) has the bit, or the "visible" mask
    //  (+0x98) does *and* the world's own +0x30 object has flag 0x10000 set
    //  at its own +0x38 (an unidentified class, read through a raw offset
    //  per CLAUDE.md's own rule - only the one touched field gets a name).
    int  TestDetectionRelationBit(unsigned int player, int bit); // 0x6F3A35C0

    char m_reserved00[0x30];
    //  +0x30 - an unidentified object pointer; only its own +0x38 flags
    //  word (bit 0x10000) is ever read, by TestDetectionRelationBit above.
    struct SGameWorldFlagsAt30* m_pField30; // +0x30
};

//----------------------------------------------------------------------------
//  The object CGameWorld's own +0x30 points at.  Only the one flags word
//  TestDetectionRelationBit reads gets a name; everything ahead of it is an
//  unrecovered gap.
//----------------------------------------------------------------------------
struct SGameWorldFlagsAt30
{
    char         m_reserved00[0x38];
    unsigned int m_flags38;   // +0x38, bit 0x10000 tested
};

//----------------------------------------------------------------------------
//  The world frame, at CGameUI +0x3BC: the frame the 3D world is drawn into,
//  and also what owns the mouse cursor, the cursor modes, the tracked
//  widgets and every in-world indicator.
//
//  **This class used to be called `CCursorFrame`, and that was wrong.**
//  Batch A inferred the name from the vtable symbol at 0x6F97063C because
//  this object is a frame that owns cursor modes and frees an array tagged
//  with the RTTI descriptor name of `enum CursorMode`.  `Frame/cursorframe.h`
//  then found that 0x6F629170 stamps `??_7CCursorFrame@@6B@` outright on a
//  *different*, 0x1B0-byte CSpriteFrame, recorded that one of the two names
//  had to go, and could not say what this one should be.
//
//  Its own constructor settles it.  0x6F39A910 is the body CGameUI parks at
//  +0x3BC (`call sub_6F39A910` / `mov [esi+3BCh], eax`, at 0x6F2FECE7); it
//  stamps `??_7CWorldFrameWar3@@6B@` at +0 and its `_0` twin at +0xB4, tags
//  every allocation it makes ".\\CWorldFrameWar3.cpp", constructs the
//  checked camera slot at +0x19C and clears +0x1A0, +0x1AC, +0x1B4 and
//  +0x620 - every field declared below.  So the class is CWorldFrameWar3 and
//  the cursor state is one of the things it carries.
//
//  Deriving it from CObserver rather than CFrame is batch A's reading and is
//  left standing: the two vtables at +0 and +0xB4 are CFrame's own pair, so
//  the record really is a CFrame, but nothing here depends on the base and
//  re-basing it would move every offset below.
//----------------------------------------------------------------------------
//  One rally-point indicator record.  0x18 bytes - the loop's own stride -
//  with the sprite at +0x08, the same place SWaypointIndicator keeps its own.
struct SRallyIndicator
{
    char        m_reserved00[0x08];
    class CSprite* m_pSprite;   // +0x08
    char        m_reserved0C[0x0C];
};

struct SWaypointIndicator
{
    char        m_reserved00[0x08];
    class CSprite* m_pSprite;   // +0x08
    char        m_reserved0C[0x10];
};

//  One of the two twelve-entry player word tables at CWorldFrameWar3 +0x1F8
//  and +0x214.  The count really is stored: the frame's constructor writes 12
//  into the first dword of both, and the fill helper reads it back
//  (`mov eax,[esi]` / `add eax,eax` / memset of that many bytes from +4).
struct SPlayerWordTable
{
    //  The count is the constructor's own: CWorldFrameWar3's mem-init list
    //  stores 12 into both tables' +0x00 before anything else touches them.
    SPlayerWordTable() : m_count(12) {}

    unsigned int   m_count;         // +0x00 - 12
    unsigned short m_entries[12];   // +0x04
};

//  CFrame, not CObserver.  This used to be modelled as `: public CObserver`
//  with 0x15C bytes of padding after it, because the camera slice only ever
//  reached three offsets inside that block.  The constructor (0x6F39A910)
//  settles it: its first act is `call CFrame::CFrame` with `this` unchanged
//  in ecx, and it then stamps ??_7CWorldFrameWar3@@6B@ at +0x00 and
//  ??_7CWorldFrameWar3@@6B@_0 at +0xB4 - which is CLayer's vtable slot and
//  CLayoutFrame's, i.e. exactly CFrame's two bases.  CObserver is still a
//  base, one level further down (CFrame -> CLayer -> CObserver -> TRefCnt),
//  which is why the constructor's four devirtualised `CObserver::Dispatch`
//  calls pass `this` with no adjustment.
//
//  Three offsets the old padding named are CFrame's own and keep working
//  under their CFrame names: +0x0C/+0x10 are CLayer's m_flags/m_flagMask,
//  and +0x140 - the live camera controller NotifyControllerChanged swaps by
//  count - is CFrame::m_140.
class CWorldFrameWar3 : public CFrame
{
public:
    //  +0x168 .. +0x174 - a flag and three SSpriteSlots, the last three of
    //  which the constructor allocates 0x1C bytes each for and initialises
    //  through SSpriteSlot::Init.
    int           m_field168;           // +0x168
    struct SSpriteSlot* m_pSlot16C;     // +0x16C
    struct SSpriteSlot* m_pSlot170;     // +0x170
    struct SSpriteSlot* m_pSlot174;     // +0x174
    //  +0x178 - the player-slot table singleton (g_unk6FAB65F4), cached by
    //  LatchLocalPlayer so that everything below it can be re-read without
    //  the global.
    void*         m_pPlayerTable;       // +0x178
    int           m_field17C;           // +0x17C
    int           m_field180;           // +0x180
    //  +0x184 - the local player's own record, i.e. the player table's
    //  array entry for m_localPlayer.
    int           m_localPlayerRecord;  // +0x184
    //  +0x188 - the player table's own +0x34, cached beside it.  Nothing in
    //  this call tree reads it back, so it is named for where it came from.
    void*         m_field188;           // +0x188
    float         m_float18C;           // +0x18C
    float         m_float190;           // +0x190
    float         m_float194;           // +0x194
    //  +0x198 - the local player's slot index, latched by 0x6F391960 out
    //  of the player record it caches at +0x178; every model this frame
    //  loads is team-coloured for it.
    unsigned int  m_localPlayer;        // +0x198
    //  +0x19C - this frame's own checked '+cam' slot.  0x6F38ECD0 is two
    //  instructions: `add ecx, 19Ch` and a jump into
    //  SCheckedCameraSlot::Assign.  The slot is one word wide, so the three
    //  that follow it stay reserved.
    SCheckedCameraSlot m_camera;        // +0x19C
    //  +0x1A0 - the minimap, adopted by SetMinimap
    //  (GameUI/worldframeminimap.cpp).
    class CMinimap* m_pMinimap;         // +0x1A0
    int           m_field1A4;           // +0x1A4 - starts 1
    int           m_field1A8;           // +0x1A8 - starts 1
    int           m_cursorMode;         // +0x1AC
    int           m_modeCount;          // +0x1B0
    int           m_modeCapacity;       // +0x1B4
    void*         m_pModes;             // +0x1B8
    int           m_field1BC;           // +0x1BC
    int           m_suppressed;         // +0x1C0
    int           m_pausedByGame;       // +0x1C4
    //  +0x1C8 - the cursor's texture path: an SPathField, the counted-string
    //  field Misc/pathfieldset.cpp reconstructs, built by the constructor at
    //  0x6F011300 and written only by SetCursorState's `case 9`.  Twelve
    //  bytes, not 0x78: the block used to run all the way to +0x240, and
    //  LatchLocalPlayer (0x6F391960) disproves that outright - it stores a
    //  *word* at +0x1F4 and takes the addresses of +0x1F8 and +0x214 as two
    //  separate objects, and the frame's own constructor (0x6F39A910) writes
    //  every field named below individually.
    SUnitCreateParamsSub m_texturePath; // +0x1C8
    //  +0x1D4 .. +0x1E0 - four floats the constructor zeroes as one `fldz`
    //  run.  Nothing in this tree reads them.  Four separate members rather
    //  than an array: C++03 has no way to name an array in a member
    //  initialiser list, and the constructor's whole prefix is one.
    float         m_cursorSpan0;        // +0x1D4
    float         m_cursorSpan1;        // +0x1D8
    float         m_cursorSpan2;        // +0x1DC
    float         m_cursorSpan3;        // +0x1E0
    int           m_field1E4;           // +0x1E4
    int           m_field1E8;           // +0x1E8
    int           m_field1EC;           // +0x1EC
    int           m_field1F0;           // +0x1F0
    //  +0x1F4 - 1 << m_localPlayer, kept as a *word*: LatchLocalPlayer
    //  stores it with `mov [esi+1F4h], dx` and reads it back with `movzx`.
    unsigned short m_localPlayerBit;    // +0x1F4
    unsigned short m_reserved1F6;
    //  +0x1F8 / +0x214 - two twelve-entry tables of one word per player
    //  slot, both filled by the same helper (BuildPlayerMaskTable) and both
    //  primed by LatchLocalPlayer.  The first is primed from the player
    //  table's own word at +0x2C with the helper's third argument 0, which
    //  gives every selected player its own bit; the second from
    //  m_localPlayerBit with the argument 1, which takes each entry from the
    //  player record's word at +0x2E0 instead.  A frame whose
    //  m_localPlayerObject is set gets 0xFFF - all twelve - in the local
    //  player's entry and nothing anywhere else.  What the two are *used*
    //  for is not in this call tree: nothing here reads either one back, so
    //  they are named for how they are keyed rather than for a meaning.
    SPlayerWordTable m_playerMaskBySlot;    // +0x1F8
    SPlayerWordTable m_playerMaskByRecord;  // +0x214
    //  +0x230 - the local player record's +0xF0 handle ref resolved through
    //  QueryField78, i.e. the object standing behind the local slot.  It is
    //  what decides which of BuildPlayerMaskTable's two arms runs.
    int           m_localPlayerObject;  // +0x230
    char          m_reserved234[0x04];
    float         m_float238;           // +0x238
    float         m_float23C;           // +0x23C
    void*         m_pTrackTarget;       // +0x240
    //  +0x244 - a CAgentPtr, constructed from null by the frame's own
    //  member-initialiser list (0x6F02F780).
    CAgentPtr     m_pTrackOwner;        // +0x244
    int           m_field248;           // +0x248
    char          m_reserved24C[0x04];
    int           m_field250;           // +0x250
    //  +0x254 - a CCinematicFilter (GameUI/cinematicfilter.cpp), 0xA4 bytes.
    //  Its storage rather than the type: that class is declared in its own
    //  translation unit and nothing here reads a field of it.
    CCinematicFilter m_filter;          // +0x254 .. +0x2F8
    int           m_field2F8;           // +0x2F8
    int           m_field2FC;           // +0x2FC
    int           m_field300;           // +0x300 - starts -1
    char          m_reserved304[0x08];
    int           m_field30C;           // +0x30C
    float         m_float310;           // +0x310
    float         m_float314;           // +0x314
    float         m_float318;           // +0x318
    int           m_pauseLatched;       // +0x31C
    int           m_pauseKind;          // +0x320
    int           m_field324;           // +0x324 - starts 1
    int           m_field328;           // +0x328 - starts 1
    //  +0x32C - the player table's own +0x3C0, cached by LatchLocalPlayer.
    void*         m_field32C;           // +0x32C
    int           m_field330;           // +0x330 - starts 1
    //  +0x334 - the fog object CreateFogObject (0x6F4EAF50) makes, held as
    //  the numbered-slot host every setter in ApplyTerrainFogConfig writes
    //  through.
    void*         m_pFog;               // +0x334
    //  +0x338 / +0x33C - the two day/night-cycle sprites, built on demand
    //  by SetDayNightModels from the two "Environment\\DNC\\..." model
    //  paths 0x6F39A910 hands it.
    class CSprite* m_pDayModel;         // +0x338
    class CSprite* m_pNightModel;       // +0x33C
    //  +0x340 - the world light: created, given its two controller-slot
    //  values and a white colour, handed to the frame and activated.
    void*         m_pWorldLight;        // +0x340
    char          m_reserved344[0x04];
    //  +0x348 - the same 0x0C-byte counted-string field the cursor's texture
    //  path at +0x1C8 is (0x6F011300 builds both).
    SUnitCreateParamsSub m_field348;    // +0x348
    int           m_field354;           // +0x354
    int           m_field358;           // +0x358 - starts 1
    int           m_field35C;           // +0x35C
    int           m_field360;           // +0x360
    int           m_field364;           // +0x364
    int           m_field368;           // +0x368
    int           m_field36C;           // +0x36C
    //  +0x370 - the eight world-frame slots, 0x18 bytes each, whose ids the
    //  constructor stamps -1.  Its storage: SWorldFrameSlotArray is declared
    //  in the translation units that resize it.
    //  Its three fields spelled out, for the member-initialiser reason the
    //  cursor span above gives; SWorldFrameSlotArray itself is declared in
    //  the translation units that resize it, and the SetCount call casts.
    unsigned int  m_slotAlloc;          // +0x370
    unsigned int  m_slotCount;          // +0x374
    void*         m_pSlots;             // +0x378
    int           m_field37C;           // +0x37C
    float         m_float380;           // +0x380
    float         m_float384;           // +0x384
    int           m_field388;           // +0x388 - starts 1
    int           m_field38C;           // +0x38C
    float         m_float390;           // +0x390
    float         m_float394;           // +0x394
    //  +0x398 .. +0x3A8 - [Misc] ScaledAnimTime and DayHours, then the three
    //  [SelectionCircle] colours, read once at construction.
    unsigned int  m_scaledAnimTime;     // +0x398
    unsigned int  m_dayHours;           // +0x39C
    unsigned int  m_colorFriend;        // +0x3A0
    unsigned int  m_colorNeutral;       // +0x3A4
    unsigned int  m_colorEnemy;         // +0x3A8
    int           m_field3AC;           // +0x3AC
    //  +0x3B0 - the sixteen rally-point destination indicators, 0x18 bytes
    //  each with the record's own sprite at +0x08.  The count and the
    //  stride are CreateRallyIndicators' own loop bounds.
    struct SRallyIndicatorArray
    {
        //  0x6F398F70 - `retn 4`; still a redirect.
        void SetCount(unsigned int count);
        unsigned int m_alloc;                   // +0x00
        unsigned int m_count;                   // +0x04
        struct SRallyIndicator* m_data;         // +0x08
    }             m_rallyIndicators;    // +0x3B0 .. +0x3BC
    char          m_reserved3BC[0x180];
    //  +0x53C - sixteen plain ints, filled 15, 14, ... 0 - a draw order for
    //  the indicators above, and the only thing in this tree that reads or
    //  writes it is the loop that fills it.
    struct SRallyOrderArray
    {
        //  0x6F38CFE0 - `retn 4`; still a redirect.
        void SetAlloc(unsigned int alloc);
        //  Inline: CreateRallyIndicators carries the whole of it, and it is
        //  the shorter of the two SetCount shapes in this binary - no chunk
        //  rounding, just "grow when short on both counts, then take the
        //  count".
        __forceinline void SetCount(unsigned int count)
        {
            if (count > m_count && count > m_alloc)
                SetAlloc(count);
            m_count = count;
        }
        unsigned int m_alloc;   // +0x00
        unsigned int m_count;   // +0x04
        int*         m_data;    // +0x08
    }             m_rallyOrder;         // +0x53C .. +0x548
    char          m_reserved548[0x40];
    //  +0x588 - the single rally-point *source* indicator, the twin of the
    //  sixteen destinations above.
    class CSprite* m_pRallySourceSprite; // +0x588
    //  +0x58C - the "TargetPointConfirm" indicator sprite.
    class CSprite* m_pTargetConfirmSprite;  // +0x58C
    unsigned int  m_waypointAlloc;      // +0x590
    unsigned int  m_waypointCount;      // +0x594
    //  +0x598 - 256 waypoint-indicator records of 0x1C bytes each, whose
    //  +0x08 is the record's own sprite.  The count and the stride are the
    //  loop bounds in CreateWaypointIndicators (0x1C00 / 0x1C).
    struct SWaypointIndicator* m_pWaypoints;   // +0x598
    char          m_reserved59C[0x04];
    //  +0x5A0 .. +0x660 - eight growable arrays, every one of them reserved
    //  once by the constructor and then emptied again.  Their element types
    //  are the shipped ones: each array's own ComputeChunk/SetAlloc pair is
    //  already in funcmap under the instantiation the Storm allocation tag
    //  names.
    TSGrowableArray<class CUnit*>          m_units;         // +0x5A0
    TSGrowableArray<class CDestructable*>  m_destructables; // +0x5B0
    TSGrowableArray<class CItem*>          m_items;         // +0x5C0
    int           m_field5D0;           // +0x5D0
    int           m_field5D4;           // +0x5D4
    int           m_field5D8;           // +0x5D8
    int           m_field5DC;           // +0x5DC
    TSGrowableArray<class CEffectImage*>   m_effects;       // +0x5E0
    TSGrowableArray<class CGhostImage*>    m_ghostImages;   // +0x5F0
    TSGrowableArray<SCheckedUnitSlot>      m_selectedUnits; // +0x600
    int           m_field610;           // +0x610
    int           m_field614;           // +0x614
    int           m_field618;           // +0x618
    int           m_field61C;           // +0x61C
    //  +0x620 - what GetTrackedWidgets hands back.
    TSGrowableArray<SCheckedUnitSlot>      m_trackedWidgets;// +0x620
    TSGrowableArray<SCheckedSelectableSlot> m_selectables;  // +0x630
    int           m_field640;           // +0x640
    int           m_field644;           // +0x644
    int           m_field648;           // +0x648
    int           m_field64C;           // +0x64C
    TSGrowableArray<SCheckedGhostImageSlot> m_ghostSlots;   // +0x650

    void  ResetTrack();                              // 0x6F38E050
    void* GetTrackedWidgets();                       // 0x6F38C690
    void  ApplyCursorColor();                        // 0x6F393040
    void  ReleaseCursorModes();                      // 0x6F395DC0
    void  SetCursorState(int state, void* texture, int flags);  // 0x6F395EB0
    //  0x6F395D60 - push the current (or an explicit) cursor mode onto the
    //  +0x1B0 stack, unless it is already the top and nothing forces a
    //  duplicate.  `retn 0Ch`.  See gameuicursorstate.cpp.
    void  PushCursorMode(int force, int useExplicit, int explicitMode);
    //  0x6F398F20 - pop it back off and re-enter whatever mode is now on
    //  top.  `retn 0`.  See gameuicursorstate.cpp.
    void  PopCursorMode();
    //  0x6F38ECD0 - park `camera` in the '+cam' slot at +0x19C.  `retn 4`.
    //  GameUI/worldframesetcamera.cpp.
    void  SetCamera(CCameraWar3* camera);
    //  0x6F38C770 - adopt the minimap and force it to repaint at once.
    //  GameUI/worldframeminimap.cpp.
    void  SetMinimap(class CMinimap* minimap);
    //  0x6F38BD80 - build the "TargetPointConfirm" indicator sprite.
    //  GameUI/worldframetargetsprite.cpp.
    void  CreateTargetConfirmSprite();
    //  0x6F38BE80 - push the whole [TerrainZFog] config section into the fog
    //  object and hand it to the frame.  GameUI/worldframeterrainfog.cpp.
    void  ApplyTerrainFogConfig();
    //  0x6F38DBE0 - build all 256 waypoint-indicator sprites.
    //  GameUI/worldframewaypoints.cpp.
    void  CreateWaypointIndicators();
    //  0x6F399740 - build the sixteen rally-point destination indicators and
    //  the one source indicator.  `retn 0`.
    //  GameUI/worldframerallyindicators.cpp.
    void  CreateRallyIndicators();
    //  0x6F390460 - load the day and night cycle models.
    //  GameUI/worldframedaynight.cpp.
    void  SetDayNightModels(unsigned int dayModel, unsigned int nightModel);
    //  0x6F39A910 - GameUI/worldframector.cpp.  `retn 4`.
    CWorldFrameWar3(void* parent);
    //  0x6F606850 - adopt `controller` at +0x140, releasing the old one.
    //  A CSimpleFrame method by address, reached only through this class in
    //  this tree.  `retn 4`.
    void  NotifyControllerChanged(void* controller);
    //  0x6F391960 - cache the player-slot table and everything derived from
    //  the local player's slot, then recolour every indicator for it.
    //  `retn 0`.  GameUI/worldframelocalplayer.cpp.
    void  LatchLocalPlayer();
    //  0x6F38DEA0 - fill one of the two player word tables above.  Twelve
    //  entries, one per player: `players` selects which of them get a
    //  non-zero word, and `fromRecord` picks where that word comes from.
    //  `retn 0Ch`; still a redirect.
    void  BuildPlayerMaskTable(SPlayerWordTable* table, unsigned int players,
                               int fromRecord);
    //  0x6F38DE10 - re-run ApplyTeamColorForPlayer(m_localPlayer) over the
    //  rally indicators, the rally source sprite and all 256 waypoint
    //  indicators.  `retn 0`; still a redirect.
    void  RecolorIndicatorsForLocalPlayer();
    //  0x6F39A1E0 - `retn 4`.  Add `unit` to `m_selectedUnits` (+0x600)
    //  unless it is already reachable through the special-selection query
    //  at 0x6F27A460 (CUnit's own "flag 0x80000 set and the query says
    //  yes" gate) - see GameUI/worldframeaddselectedunit.cpp.  Reached
    //  from SPlayerTableSubmitParts::RefreshUnitFog
    //  (Widget/playertableunitfogrefresh.cpp) through `GetGameUI(1,0)`'s
    //  own +0x3BC world frame.
    void  AddSelectedUnitIfNotAlready(class CUnit* unit);
};

//----------------------------------------------------------------------------
//  CEscMenu's main panel and CGameUI +0x228's own +0x18 (CGameUI +0x228
//  is a CEscMenu*, see CGameUIPauseButton below).
//
//  RENAMED from the earlier CGameUIPauseFrame stand-in this session
//  (2026-08-28), now on real structural evidence rather than only the live
//  pointer read that first flagged it: `??_7CEscMenuMainPanel@@6B@` -
//  0x6F93C8EC, 57 slots, RTTI-confirmed by `ida_query.py names
//  CEscMenuMainPanel` - see docs/targets/vtables/CEscMenuMainPanel.txt.
//  Slots 0/2/5 are address-exact matches to CObserver's own
//  ReleaseSelf/Dispatch/Method_0x14 (docs/targets/vtables/CObserver.txt),
//  and slots 1/3/4 sit exactly where CObserver's own DeleteSelf/
//  Method_0x0C/Method_0x10 sit but carry different (CEscMenuMainPanel's
//  own, unreconstructed) addresses - this class derives from CObserver,
//  overriding three of its six virtuals and adding 51 more of its own.
//  No constructor was reachable this session (the operator's ida_query
//  worker answered exactly the `names` and `vtable` calls above and
//  nothing after; every further query - a second vtable dump for this
//  class's own secondary/`_0` vtable at 0x6F93C8C0, an xrefs search for
//  the constructor, a fresh calltree_asm - came back exit 3, "nobody is
//  listening"), so the two known fields below are still the operator's
//  own live reads, carried forward under the corrected name rather than
//  independently re-derived: `m_paused` at +0x170, and `m_pCaptionFrame`
//  at +0x190, which a live check of *its own* vtable pointer read as
//  `CEscMenuMainPanel`'s sibling target, `CGlueTextButtonWar3`
//  (0x6F95A00C, ??_7CGlueTextButtonWar3@@6B@) - the Esc menu's main panel
//  and one of its text buttons, not a dedicated "pause caption" frame.
//  RefreshCaption's own body (gameuipausecaption.cpp) is independent,
//  weaker corroboration: it reaches a +0x1E4 CTextFrame* and a slot-0x10C
//  enable/disable virtual on whatever `m_pCaptionFrame` points at, which
//  is the shape a button-with-a-text-label class would have and is at
//  least consistent with `CGlueTextButtonWar3`, though it does not by
//  itself distinguish that from some other button class.
//
//  That reading has one loose end worth flagging for whoever reconstructs
//  CGlueTextButtonWar3 for real: a live check of m_pCaptionFrame's *own*
//  vtable pointer (the object CSimpleTop's mouse-capture field pointed at
//  while every click was being swallowed during a live pause) came back as
//  `CSimpleButton`'s own vtable (0x6F96DC8C) - not CGlueTextButtonWar3's
//  own (0x6F95A00C), which is a different address regardless of whether
//  CGlueTextButtonWar3 overrides anything (MSVC stamps a distinct vtable
//  per class for RTTI even with zero overrides). Whether the captured
//  object truly is this field, a different CSimpleButton entirely, or a
//  `this`-adjusted secondary-base view that happens to read as
//  CSimpleButton's own vtable is unresolved - see
//  docs/notes/pause-mouse-capture.md.
//----------------------------------------------------------------------------
class CEscMenuMainPanel
{
public:
    char  m_reserved000[0x170];
    int   m_paused;                     // +0x170 - the flag last applied
    char  m_reserved174[0x1C];
    CGlueTextButtonWar3* m_pCaptionFrame;              // +0x190

    void RefreshCaption(int paused);                 // 0x6F3505D0
};

//----------------------------------------------------------------------------
//  The UI's tooltip-carrying frame: whatever a button-shaped panel keeps at
//  its +0x134, and the tooltip host that frame keeps at its own +0x190.
//
//  Named for what its one reconstructed method does rather than from a
//  shipped tag, because there is none: no allocation in this tree is tagged
//  with the class and no constructor of it is reached.  CEscMenuMainPanel
//  above carries a frame at the same +0x190 and may well be the same shipped
//  class; nothing here proves it either way, so the two stay separate and
//  neither claims the other's fields.
//----------------------------------------------------------------------------
//  The host itself.  Only its one setter is reached; nothing in this tree
//  reads a field of one, so it has no layout here.
class CUIToolTipHost
{
public:
    //  0x6F337A00 - take the whole tooltip text.  `retn 14h` - five stack
    //  dwords, `this` alone in ecx, which is what makes it a __thiscall
    //  member and not a __fastcall free function taking the host: a
    //  __fastcall would take `title` in edx and clean four bytes too few.
    //  Still a redirect.
    void SetText(const char* title, const char* body, int a, int b, int c);
};

class CUIToolTipFrame
{
public:
    char  m_reserved000[0x190];
    //  +0x190 - the tooltip host the text is handed to.  A null one means
    //  the frame has no tooltip and SetToolTipText does nothing at all.
    CUIToolTipHost* m_pToolTipHost;     // +0x190

    //  0x6F34F5F0 - give this frame's tooltip its text.  `retn 14h`.
    //  GameUI/uitooltiptext.cpp.
    void SetToolTipText(const char* title, const char* body,
                        int a, int b, int c);
    //  0x6F34F300 - re-lay the tooltip out after its text changed.
    //  `retn 0`.  Still a redirect.
    void RefreshToolTipLayout();
};

//  MISNAMED, and now confirmed structurally rather than just by a live
//  vtable-pointer read: `CGameUI+0x228` is a real `CEscMenu*`
//  (GameUI/escmenu.h) - the CEscMenu target (2026-08-28,
//  docs/targets/CEscMenu.md) traced its constructor (0x6F34FA20) to the one
//  call site that builds this field (0x6F2F6530: `cmp dword ptr [esi+228h],
//  0` / `SMemAlloc(0x28, ...)` / `call sub_6F34FA20`), which both stamps
//  `??_7CEscMenu@@6B@` as its own vtable and confirms the field's real
//  object size (0x28 bytes) - independent confirmation of the same finding
//  the live read made, not just a repeat of it.
//
//  This stand-in class is kept under its old name anyway, because renaming
//  the *field* or its *type* here would break `gameuisetgamepaused.cpp`'s
//  and `gameuipausebutton.cpp`'s own `m_pPauseButton->SetPaused(paused)`
//  call site: `SetPaused` (0x6F35E670) is real, reconstructed, already
//  matching, and this narrow slice (a `char[0x18]` gap plus one method) is
//  the only part of the real 0x28-byte `CEscMenu` this file ever needed.
//  `escmenu.h`'s own `CEscMenu` now carries the real, full field layout
//  (five fields, not one) and the real constructor/destructor/vtable - that
//  is the class to extend from here on, not this one. `m_pFrame` below
//  really is `CEscMenu::m_pMainPanel` - a sibling session (2026-08-28,
//  same day) already renamed *that* stand-in for real, from
//  `CGameUIPauseFrame` to `CEscMenuMainPanel`, above, with its own vtable
//  evidence - so the field below is typed correctly now, just still under
//  this file's own field name (`m_pFrame`) rather than `m_pMainPanel`.
class CGameUIPauseButton
{
public:
    char m_reserved00[0x18];
    CEscMenuMainPanel* m_pFrame;                     // +0x18
    void SetPaused(int paused);                      // 0x6F35E670
};

//----------------------------------------------------------------------------
//  The five pause-time controllers.  Every one of them caches the flag it
//  was last given and does nothing when it has not changed.
//----------------------------------------------------------------------------
class CGameUIButtonList
{
public:
    char  m_reserved000[0x12C];
    int   m_count;                      // +0x12C
    void* m_pEntries;                   // +0x130
    char  m_reserved134[0x0C];
    int   m_enabled;                    // +0x140
    int   m_suppressed;                 // +0x144

    void SetEnabled(int enabled);                    // 0x6F3470E0
};

class CGameUIItemBar
{
public:
    char  m_reserved000[0x124];
    int   m_enabled;                    // +0x124
    char  m_reserved128[0x12C - 0x128];
    //  +0x12C..+0x144 - seven slots (index 0-6), read through
    //  GetSlot0x12C's own switch/jump table (index 7 and anything past it
    //  share the identical `xor eax,eax` jump-table entry, not a separate
    //  default block - see gameuiitembarslot.cpp).  What each slot points
    //  at is not established anywhere in this tree.
    void* m_slot0x12C[7];               // +0x12C
    CGameUIButtonList* m_pButtons;      // +0x148

    void SetEnabled(int enabled);                    // 0x6F352F00
    //  0x6F338830 (`retn 4`) - one of the seven +0x12C slots by index.
    //  gameuiitembarslot.cpp.
    void* GetSlot0x12C(unsigned int index);
};

class CGameUICommandCard
{
public:
    char  m_reserved000[0x154];
    void* m_pRows;                      // +0x154
    char  m_reserved158[0x58];
    int   m_enabled;                    // +0x1B0

    void SetEnabled(int enabled);                    // 0x6F342190
};

class CGameUIHeroBar
{
public:
    char  m_reserved000[0x154];
    void* m_pSlots;                     // +0x154
    char  m_reserved158[0x24];
    int   m_enabled;                    // +0x17C

    void SetEnabled(int enabled);                    // 0x6F3463A0
};

class CGameUIMiniMapPanel
{
public:
    char  m_reserved000[0x134];
    CSimpleButton* m_pFrame;            // +0x134
    char  m_reserved138[0x18];
    int   m_enabled;                    // +0x150

    void SetEnabled(int enabled);                    // 0x6F33ACC0
};

//  The camera target reached this class from the other side and confirmed
//  the two fields above from a second call site: 0x6F33BD80 stores the
//  camera at +0x128 and posts through it, and 0x6F33BDB0 tears the binding
//  down by building a `CControlTrackEvent` (its own shipped vtable symbol,
//  stamped into a stack record) around the +0x124 handle and handing it to
//  +0x128's vtable slot 3.  So the panel is what tracks a target *with* the
//  camera; the class name stays the one this file already had, which is
//  inferential either way - nothing in the image names it.
class CGameUIPortraitPanel
{
public:
    char  m_reserved000[0x124];
    void* m_pSelected;                  // +0x124 - a checked selectable
    //  +0x128 - the object the panel tracks its selection *with*.  The
    //  camera target is what says what goes in here: PostCameraEvents calls
    //  SetPortraitFrame with the CCameraWar3, and CancelControlTrack
    //  dispatches through this pointer's own vtable slot 3.  The method
    //  names are the ones this file already had; neither they nor the class
    //  name are recovered from the image, and nothing in either tree names
    //  this class.
    void* m_pFrame;                     // +0x128

    void SetPortraitFrame(void* frame);              // 0x6F33BD80
    void CancelControlTrack();                       // 0x6F33BDB0
};

//----------------------------------------------------------------------------
//  The selection overlay at CGameUI +0x214, and the per-selectable visual it
//  drives.  `SSelectionVisual` is the name Widget/selectionvisualrelease.cpp
//  already gave this receiver; only the members this batch needs are added.
//----------------------------------------------------------------------------
struct SOverlaySubject;

class CGameUISelectionOverlay
{
public:
    char  m_reserved00[0x14];
    SOverlaySubject* m_pSelected;                    // +0x14

    void ClearSelection();                           // 0x6F37A3B0
};

struct SSelectionVisual
{
    void __thiscall Detach(int mode);                // 0x6F379D10 - a
                                                     //   redirect elsewhere
    void __thiscall UpdatePlacement();               // 0x6F37BAB0
};

//----------------------------------------------------------------------------
//  The unit tip, named from its own `??_7CUnitTip@@6B@` store.
//----------------------------------------------------------------------------
class CUnitTip
{
public:
    void* m_vtable;                                  // +0x00
    char  m_reserved04[0x80];
    int   m_revision;                                // +0x84
    char  m_reserved88[0x08];
    int   m_hidden;                                  // +0x90
    int   m_shown;                                   // +0x94
    char  m_reserved98[0x8C];
    char  m_selected[4];                             // +0x124 - a checked
                                                     //   selectable slot
    void* m_pBackdrop;                               // +0x128
    void* m_pDetailLine;                             // +0x12C
    void* m_pDescLine;                               // +0x130
    void* m_pNameLine;                               // +0x134

    CUnitTip* Construct(int style);                  // 0x6F35B380
};

//----------------------------------------------------------------------------
//  Free functions this batch defines, called from more than one of its
//  translation units.
//----------------------------------------------------------------------------
//  0x6F2F8E90 - the colour a player's name is drawn in.  Four bytes returned
//  through a hidden buffer in ecx, which is what makes it a class and not an
//  `unsigned int`.
struct SPlayerColor
{
    unsigned int m_value;

    SPlayerColor(unsigned int value) : m_value(value) {}
    //  Written out rather than left implicit: a 4-byte class with no
    //  user-declared copy constructor comes back in eax, and the shipped
    //  function returns through a buffer in ecx.
    SPlayerColor(const SPlayerColor& other) : m_value(other.m_value) {}
};

SPlayerColor __fastcall GetPlayerUiColor(unsigned int player);      // 0x6F2F8E90
const char*  __fastcall GetPlayerDisplayName(unsigned int player,
                                             int ensure);           // 0x6F2F8F90
int          __fastcall IsLocalPlayerReferee();                     // 0x6F2F8FC0
int          __fastcall IsLocalPlayerFullPlayer();                  // 0x6F2F9020

int          __fastcall IsPlayerSlotOccupied(const void* slot);     // 0x6F313740
unsigned int __fastcall ResolvePlayerColorIndex(unsigned int player,
                                                unsigned int slot); // 0x6F314A70
const unsigned int* __fastcall GetPlayerColorValue(unsigned int player);
                                                                    // 0x6F314B60

void* __fastcall FindSkinSection(const char* name);                 // 0x6F31B4B0
void* __fastcall FindActiveSkinSection();                           // 0x6F31F4C0
const char* __fastcall ResolveSkinValue(const char* key,
                                        const char* skin);          // 0x6F31F530

int   __fastcall IsWorldViewReady();                                // 0x6F3335A0
void  RefreshAllSelectionVisuals();                                 // 0x6F333590
void  RefreshSelectedUnitVisuals();                                 // 0x6F379EA0

CUnitTip* AcquireUnitTip();                                         // 0x6F378C80

void  ClearAllPauseHighlights();                                    // 0x6F37D260
void  __fastcall UpdatePauseHighlights(int active, int checkRelation,
                                       int checkAlly);              // 0x6F381670

unsigned int GetTextAgentTypeId();                                  // 0x6F443C60

//----------------------------------------------------------------------------
//  The screen-projection helpers, 0x6F333CB0 / 0x6F333FB0 / 0x6F334180.
//----------------------------------------------------------------------------
//  The rectangle is two corners; the point the test takes is indexed and not
//  named, because its two components are consumed in the order (+0x00 -> the
//  rect's second axis, +0x04 -> its first) and nothing in this tree says
//  which of the two the caller thinks of as x.
struct SScreenRect
{
    float m_minX, m_minY, m_maxX, m_maxY;
    int __thiscall Contains(const float* point) const;              // 0x6F332540
};

//  Answers whether the projected point landed inside the clip rectangle;
//  `out` and `outDepth` are filled in either way.
int __fastcall ProjectWorldPointToScreen(float* out, float* outDepth,
                                         float x, float y, float z);
                                                                    // 0x6F333CB0
float* __fastcall GetAgentAnchorPosition(float* out, void* agent);   // 0x6F333FB0
int    __fastcall ProjectAgentToScreen(void* agent, float* out,
                                       float* outDepth);            // 0x6F334180

//----------------------------------------------------------------------------
//  CCameraWar3 : public CAgent - the base the constructor's own first call
//  (sub_6F001730/CAgent::CAgent(), asm/sub_6F307B00_0x6F307B00_calltree_asm.md)
//  and the vtable's own nine inherited-unchanged slots (ReleaseSelf,
//  CObserver::Dispatch, CObserver's two message slots, CAgent::Method_0x18,
//  GetRecordVersion, Method_0x50, Method_0x54, Method_0x5C - see
//  docs/targets/vtables/CCameraWar3.txt) both establish.  CAgent is exactly
//  0x20 bytes (vtable+refcount+resource+handle+typeTag+the embedded
//  CObserver at +0x14), so every offset below that predates this session
//  and was written relative to an un-based object is unchanged: CAgent's own
//  base subobject supplies [0x00, 0x20) and CCameraWar3's own first field
//  starts at +0x20, exactly where it already did.
//
//  Nine of CAgent's own virtual slots are overridden here with real,
//  distinct CCameraWar3 addresses (vtable slots 1, 3, 7, 8, 9, 13, 14, 15,
//  22 - user_knowledge.json's own CCameraWar3 vtable_members table); none of
//  them has a dump yet (out of this session's scope - see CLAUDE.md on not
//  hunting for a sibling session's slots), so each is declared here and
//  thunked in cameravtablethunks.cpp purely so this build's own
//  ??_7CCameraWar3@@6B@ has the right shape and length for
//  tools/check_vtables.py.  The other fifteen slots the vtable dump lists
//  either are genuinely inherited from CAgent unchanged (their address is
//  CAgent's own), or - six of them (slots 11, 12, 16, 17, 18, 19) - are
//  vtable_classes.json "missing" entries that are actually CAgent's own
//  shared nullsub addresses (0x6F471C30/0x6F471C20/0x6F001850/0x6F001860/
//  0x6F001870/0x6F001880) reused verbatim, not real overrides; neither kind
//  needs a declaration here.
//----------------------------------------------------------------------------
class CCameraWar3 : public CAgent
{
public:
    //  0x6F307B00 - see cameraconstructor.cpp.
    CCameraWar3();

    //  0x6F308340 - CameraWar3's own member teardown, in reverse declaration
    //  order: GameUI/cameradestructor.cpp.  347 instructions with a genuine
    //  __CxxFrameHandler3 frame (the nine SCameraProp modifier-pointer pairs
    //  and the two `TSGrowableArray` members all need unwind protection in
    //  the shipped build) this toolchain's fixed /GS- /EHs-c- cannot
    //  reproduce - see docs/msvc-vc8-idioms.md, "An `__except_handler4`-
    //  shaped frame this toolchain cannot reproduce" - so this stays
    //  DIFFERS even though the happy-path body is a full, offset-exact
    //  reconstruction.  +0x4C0 is m_localAngle's own m_pModifier2; +0xA8 and
    //  +0xBC are two `TSGrowableArray` members (m_pointListStorage /
    //  m_agentListStorage above) this destructor's own field inventory
    //  established.
    ~CCameraWar3();

    //  slot 1 / +0x04 (0x6F3085C0) - overrides CAgent::DeleteSelf.  Real
    //  reconstruction now: run the destructor above, then free `this` when
    //  the caller asked for it - the exact shape CAgent::DeleteSelf itself
    //  has (Agent/agent_deleteself.cpp).  GameUI/cameradeleteself.cpp.
    virtual CAgent* DeleteSelf(int flags);
    //  slot 3 / +0x0C (0x6F308E70) - overrides CObserver::Method_0x0C.
    //  The per-frame/per-event message dispatch root.
    //  GameUI/cameraupdate.cpp.
    virtual int Method_0x0C(const SAgentMessage* msg);
    //  slot 7 / +0x1C (0x6F301E40) - overrides CAgent::GetAgileTypeId.  No
    //  dump; thunked.
    virtual unsigned int GetAgileTypeId() const;
    //  slot 8 / +0x20 (0x6F301EE0) - overrides CAgent::Method_0x20.  No
    //  dump; thunked.
    virtual int Method_0x20(int context);
    //  slot 9 / +0x24 (0x6F3022B0) - overrides CAgent::Method_0x24.  No
    //  dump; thunked.
    virtual int Method_0x24(int context);
    //  slot 13 / +0x34 (0x6F308B60) - overrides CAgent::Deactivate.  No
    //  dump; thunked.
    virtual void Deactivate();
    //  slot 14 / +0x38 (0x6F303790) - overrides CAgent::Save.  Real
    //  reconstruction now - GameUI/camerasaveload.cpp.
    virtual void Save(class CDataStoreScratch* store);
    //  slot 15 / +0x3C (0x6F308810) - overrides CAgent::Load.  Real
    //  reconstruction now - GameUI/camerasaveload.cpp.
    virtual void Load(class CDataStore* store);
    //  slot 22 / +0x58 (0x6F301DF0) - overrides CAgent::GetClassName.  No
    //  dump; thunked.
    virtual const char* GetClassName();

    //  0x6F3026E0 - zero a three-float point and ask the current
    //  controller for its own value of slot 8 into it.  `retn 4`.
    float* QueryControllerVec3(float* out);
    //  0x6F303070 - project a ground point into the camera's own space and
    //  sample the terrain under it.  `retn 8`.
    float* ProjectGroundPoint(float* out, const float* point);
    //  0x6F305960 - drop the two modifiers PanTo installs.  `retn 0`.
    void ReleasePanModifiers();
    //  0x6F306750 - bind every property to the live controller.  `retn 0`.
    void BindControllerProps();
    //  0x6F306870 - unbind every one of them again.  `retn 0`.
    void UnbindControllerProps();
    //  0x6F306980 - switch controller mode.  `retn 4`.
    void SetControllerMode(int mode);
    //  0x6F3069F0 - cancel every modifier currently driving the camera.
    void CancelModifiers();
    //  0x6F306B00 - stop everything: modifiers, all ten property ramps and
    //  the pending timer.  `retn 4`.
    void StopAll(int cancelModifiers);
    //  0x6F307140 - the pan itself.  `retn 14h`.
    void PanTo(float x, float y, float duration, float height,
               int animateHeight);
    //  0x6F3085F0 - every property back to its configured default.
    void ResetToDefaults();
    //  0x6F309690 - leave whatever cursor mode is active.  `retn 0`.
    void ClearCursorMode();
    //  0x6F308CA0 - enter one, over `blend` seconds.  `retn 8`; the blend
    //  is a float stack argument.  Same clamp-and-blend shape as
    //  SetTargetDistancePreset (clamp to [0,2] against m_cursorMode instead
    //  of [0,5] against m_presetIndex, no SetLastCameraWar3Raw tail call).
    //  GameUI/cameracursormode.cpp.
    void SetCursorMode(int mode, float blend);
    //  0x6F306B80 - start the blend itself, out of this batch.  `retn 8`;
    //  called from SetCursorMode.  Declared here (not a local per-TU copy)
    //  because `this` is genuinely a CCameraWar3 already fully defined in
    //  every TU that would otherwise need one.
    void StartCursorModeTransition(float blend, int flag);
    //  0x6F304190 - stop one camera field where it is.  `retn 4`.
    //  GameUI/camerastopslot.cpp.
    void StopSlot(int slot);
    //  0x6F3027B0 - one camera property's current value, by the JASS
    //  `camerafield` id.  `retn 4`.  GameUI/cameragetproperty.cpp.
    float GetProperty(int field);
    //  0x6F3046D0 - set one camera property's target/span, by the same
    //  `camerafield` id GetProperty uses.  `retn 0Ch`.  A genuine
    //  `__except_handler4`-shaped SEH frame wraps the whole body (the
    //  ten-case switch's own SFloatModifierRef temporary needs unwind
    //  cleanup) - see docs/msvc-vc8-idioms.md.  GameUI/camerasetproperty.cpp.
    //  Stack order is `(field, span, value)`, not `(field, value, span)` -
    //  confirmed off the push order into MakeFloatModifier inside this
    //  function's own case bodies (the caller's arg_4 slot feeds the
    //  right-to-left-pushed *second* real parameter, `span`, and arg_8
    //  feeds the first, `value`) - see camerasetproperty.cpp.
    void SetProperty(int field, float span, float value);
    //  0x6F302920 - where the camera is looking, out of the live
    //  controller's slot 8.  `retn 4`.  GameUI/cameratargetposition.cpp.
    float* GetTargetPosition(float* out);
    //  0x6F302760 - how hard the camera's motion is smoothed, clamped into
    //  [0, g_cameraMaxSmoothingFactor].  `retn 4`.
    //  GameUI/camerasmoothing.cpp.
    void SetSmoothingFactor(float factor);
    //  0x6F3783E0 - plain getter, the update timer's own vtable pointer
    //  (m_updateTimer sits at this exact offset).  No caller in this
    //  batch's own reach says what it is really for; named neutrally, the
    //  way Misc/sweep_predicates.cpp's SField28Owner::GetField28 is.
    //  `retn 0`.  GameUI/cameramiscleaves.cpp.
    int GetField84() const;
    //  0x6F3056C0 - is the pan movement at +0x464 currently bound to a
    //  live handle?  `retn 0`.  GameUI/camerapanmovementactive.cpp.
    int IsPanMovementActive() const;

    //  0x6F308C60 - clamp `index` into [0,5] and, if it actually changed
    //  m_presetIndex, blend every property to it over one second.  `retn 4`.
    //  GameUI/cameracursorpreset.cpp.
    void SetTargetDistancePreset(int index);
    //  0x6F303290 - advance the cursor-preset table to its own next entry
    //  once the per-instance debounce timer allows it, and republish the
    //  result through m_target.  `retn 0`.  GameUI/cameracursorpreset.cpp.
    void UpdateCursorPresetPosition();
    //  0x6F3078B0 - drop the pan and every modifier, snap straight onto the
    //  ground under (x, y), and flag the world frame's own "camera moved"
    //  state if that snap actually went anywhere.  `retn 8`.
    //  GameUI/cameracursorpreset.cpp.
    void SnapPanTargetToGround(float x, float y);
    //  0x6F308CD0 - refresh the highlight candidate out of
    //  m_agentListStorage: a cheap incremental scan from the last index
    //  most frames, or - past a debounce interval - a full qsort re-rank
    //  (comparator 0x6F3056F0, GameUI/cameracandidatescan.cpp).  `retn 0`.
    void RefreshHighlightCandidate();

    //  0x6F305250 / 0x6F3053F0 - install (or clear) one of the two paired
    //  modifiers at m_pRampModifiers[index]/m_pOffsetModifiers[index],
    //  building the sin/cos offset from `value` over `duration`.  Both
    //  `retn 0Ch`, and both carry a genuine `__except_handler4`-shaped SEH
    //  frame this build's fixed `/GS- /EHs-c-` cannot reproduce (see
    //  docs/msvc-vc8-idioms.md) - thunked rather than guessed at.
    //  GameUI/gameui_thunks.cpp.
    void InstallRampModifier(int index, int clear, float duration);
    void InstallOffsetModifier(int index, int clear, float duration);
    //  0x6F305580 - the four-bit "which paired modifier" dispatcher in
    //  front of the two above, plus a ten-case cursor-suppression table for
    //  the world frame.  `retn 10h`.  GameUI/cameramodifierflags.cpp.
    void ApplyModifierFlags(int flags, int index, float duration,
                            int suppressCursor);
    //  0x6F306B80 - blend every one of the ten camera properties (plus the
    //  two listener config-string writers) to their current values over
    //  `blend` seconds; `cancelModifiers` forwards to StopAll.  `retn 8`.
    //  150 instructions across ten calls into the still-unreconstructed
    //  sub_6F305A60 (349 instructions of its own, a two-switch property
    //  blend/read-back leaf) - below this slice's size budget; thunked.
    //  GameUI/gameui_thunks.cpp.
    void ApplyPresetBlend(float blend, int cancelModifiers);
    //  0x6F305A60 - ApplyPresetBlend's own per-property leaf, called once
    //  per property.  `retn 10h`.  GameUI/camerapresetblendproperty.cpp.
    void ApplyPresetBlendProperty(int field, float value, float span, int flag);

    //  +0x20 - zeroed by the constructor (0x6F307B00); CAgent's own base
    //  subobject occupies [0x00, 0x20).  Read back by this batch's own
    //  Method_0x20 (cameraregisterproperties.cpp) as both a config-key
    //  index and an index into three small per-mode constant tables
    //  (cameraconstantlookup.cpp), so it is a preset/listener id rather
    //  than genuinely unnamed any more.
    int  m_presetIndex;
    //  +0x24 - which cursor mode is active; -1 for none.
    int  m_cursorMode;
    //  +0x28 - Save/Load carry it as a plain dword (CCameraWar3::Save/::Load,
    //  camerasaveload.cpp); nothing in this call tree says what it means.
    int  m_field0x28;
    //  +0x2C - Update's (0x6F308E70) own front gate: nonzero suppresses
    //  the whole dispatcher, returning 0 unconditionally.  Nothing in
    //  this pass's own closure writes it, so only that it gates Update is
    //  established.
    int  m_updateSuppressed;
    //  +0x30 - preference id 40's target (Config/preferenceapply.cpp).  Named
    //  for its offset, the `QueryField78` last resort: a preference writes it
    //  and nothing in either closure reads it, so its width is all that is
    //  established.
    int  m_preferenceField30;
    //  +0x34 - the controller the properties are currently bound to, which
    //  is m_pDefaultController whenever the mode is 0 or 2.
    void* m_pController;
    //  +0x38 - 0, 1 or 2; SetControllerMode is the only thing that writes it.
    int   m_controllerMode;
    //  +0x3C / +0x40 - the two controllers ResetToDefaults makes on first use.
    void* m_pAltController;
    void* m_pDefaultController;
    //  +0x44 - the job the last pan installed, released at the start of the
    //  next one.
    TRefCnt* m_pPanJob;
    char m_reserved48[0x50 - 0x48];
    //  +0x50..+0x58 - the pan offset, zeroed on every pan.  One value, not
    //  three fields: PanTo builds a zeroed FVec3 on its own stack and copies
    //  it over in three dword moves, which is a struct assignment and not
    //  three float stores.
    FVec3 m_offset;
    //  +0x5C - the second event sink UnbindControllerProps also clears.
    void* m_pEventSink;
    //  +0x60..+0x84 - a flat 9-dword (0x24-byte) block Save/Load carry
    //  through CDataStoreScratch::WriteDwordArray/CDataStore::ReadDwordArray
    //  with a literal count of 9 (camerasaveload.cpp) - the exact width of
    //  this gap, so this is a real, transmitted field and not padding.
    //  Nothing in either call tree reads any individual dword of it on its
    //  own, so it stays an opaque raw block rather than nine invented names.
    unsigned int m_rawBlock60[9];
    //  +0x84 - the pending-update timer StopAll cancels.  A CAgentTimer by
    //  value (agenttimer.h), which is how CItem embeds its own two.
    CAgentTimer m_updateTimer;
    //  +0x98..+0xB4 - the "camera cursor preset" cluster
    //  (GameUI/cameracursorpreset.cpp), converging evidence from three
    //  independent sessions' own dumps rather than one guess: Save/Load
    //  (GameUI/camerasaveload.cpp) carry +0xA0/+0xA4 as two plain raw
    //  dwords with no name of their own, and UpdateCursorPresetPosition's
    //  own asm (0x6F303290) reads the very same two offsets as the
    //  debounced table index and the table's own length - the same field,
    //  named for what it actually is now that both readings agree.
    int         m_cursorPresetIndex;      // +0x98 - last-applied table index
    float       m_cursorPresetNextTime;   // +0x9C - GetTickCountSeconds() debounce
    int         m_cursorPresetPending;    // +0xA0 - reused verbatim while the debounce holds
    int         m_cursorPresetCount;      // +0xA4 - table length; 0 disables the feature
    //  +0xA8 - the cinematic filter's own keyframe slot array, and (per
    //  UpdateCursorPresetPosition's own read of +0xB0, which is exactly
    //  this array's own `m_data`) the very "cursor preset table" the
    //  cluster above steps through - one object, not two. The constructor
    //  calls SetCount(8) on it at its own tail
    //  (GameUI/cameraconstructor.cpp); GameUI/camerafilterslotarray.cpp's
    //  own header note says "CCameraWar3::Load ... actually calls SetCount
    //  for real, on the +0xA8 filter-slot array Load/Save mirror each
    //  other over" - a real, already-reconstructed 12-byte
    //  SFilterSlotArray (cinematicfilter.h), just not previously split out
    //  of the reserved gap it used to sit inside. Independently confirmed
    //  from two more ends: ~CCameraWar3 (0x6F308340,
    //  GameUI/cameradestructor.cpp) frees its `m_data` word (+0x08, so
    //  +0xB0 here) through `Storm_403` tagged `.?AVC3Vector@NTempest@@` -
    //  exactly the "class NTempest::C3Vector" element tag
    //  camerafilterslotarray.cpp's own `SetAlloc` already documents for
    //  this same array - and `CCameraWar3::Save`/`::Load`
    //  (GameUI/camerasaveload.cpp) walk it three floats at a time. Four
    //  independent readings of the same field, not competing guesses.
    SFilterSlotArray m_filterSlots;    // +0xA8
    //  +0xB4/+0xB8 - the highlight-candidate rescan's own "resume here next
    //  frame" state (0x6F308CD0, GameUI/cameracandidatescan.cpp): the last
    //  winning index into m_agentListStorage below (-1 for none) and the
    //  GetTickCountSeconds() timestamp of the last full qsort rebuild.
    int   m_highlightScanIndex;    // +0xB4
    float m_highlightRebuildTime;  // +0xB8
    //  +0xBC - a `TSGrowableArray<CAgentPtr>` (storm.h/agent.h), established
    //  from ~CCameraWar3: it passes `this+0xBC` straight to `sub_6F286590`
    //  (`TSGrowableArray<CAgentPtr>::~TSGrowableArray`,
    //  Agent/agentptrarray.cpp - already reconstructed, reused directly),
    //  exactly the 0x10-byte span `TSGrowableArray<T>` occupies, ending
    //  precisely at +0xCC where the nine SCameraProp properties below
    //  begin. Left as raw bytes rather than a real
    //  `TSGrowableArray<CAgentPtr>` member for the reason
    //  `Player/playerwar3.h`'s own `m_deadHeroArrayStorage` gives: the
    //  destructor already tears it down by hand (inlined rather than
    //  through a real member-destructor call - see cameradestructor.cpp),
    //  and a typed member here would have the compiler destroy it a
    //  second time. Nothing in either call tree's own closure says what
    //  the array holds; the constructor does not touch this range at all
    //  (an open gap, not contradicting evidence).
    char m_agentListStorage[0xCC - 0xBC];

    //  +0xCC..+0x18C - nine properties, 0x18 apart.  Six of them are plain
    //  values and three are angles (the ones ResetToDefaults publishes
    //  through AssignWrappedValue rather than SetValue, which is what wraps
    //  them into one turn - see floatmodstep.cpp).
    //  +0xCC..+0x18C - nine properties, 0x18 apart.  Six are published
    //  with SUnitTrackedRef::SetValue and three with AssignWrappedValue,
    //  which wraps the value into one turn (floatmodstep.cpp) - so those
    //  three are angles, and their defaults say which: 5.3058 rad = 304
    //  degrees, 1.2217 = 70, 1.5708 = 90.  Every name below comes from the
    //  default ResetToDefaults writes into it, not from a guess: the
    //  first two from the config keys themselves ("Listener" /
    //  "ListenerDistance" and "ListenerAOA"), the rest from the numbers,
    //  which are Warcraft III's own published camera defaults.
    SCameraProp m_listenerDistance;  // +0x0CC - "Listener"/"ListenerDistance"
    SCameraProp m_listenerAngle;     // +0x0E4 - "ListenerAOA", degrees -> radians
    SCameraProp m_targetDistance;    // +0x0FC - 1650
    SCameraProp m_farZ;              // +0x114 - 5000
    SCameraProp m_nearZ;             // +0x12C - 100
    SCameraProp m_fieldOfView;       // +0x144 - 70 degrees
    SCameraProp m_rotation;          // +0x15C - 90 degrees
    SCameraProp m_angleOfAttack;     // +0x174 - 304 degrees
    SCameraProp m_roll;              // +0x18C - 0
    //  +0x1A4 - where the camera is looking, published through the same
    //  Position layer every widget's own placement goes through.
    CPathRef    m_target;
    //  +0x1B4 - and how high above the ground that point sits.
    SCameraProp m_targetZ;
    //  +0x1CC - set when a timed pan is running.
    int         m_panActive;
    //  +0x1D0 - the height ramp a timed pan drives, with the FloatModifier
    //  it installed at its own +0x10 (i.e. the camera's +0x1E0).
    SCameraProp m_panZ;
    char m_reserved1E8[0x454 - 0x1E8];
    //  +0x454..+0x460 - two arrays of two modifier references, interleaved:
    //  CancelModifiers walks them as A[0], B[0], A[1], B[1], which is one
    //  loop over a pair of arrays and not one over four slots.
    TRefCnt*    m_pRampModifiers[2];      // +0x454
    TRefCnt*    m_pOffsetModifiers[2];    // +0x45C
    //  +0x464 - the MovementModifier a timed pan installs on m_target.
    TRefCnt*    m_pPanMovement;
    char m_reserved468[0x4AC - 0x468];
    //  +0x4AC - a tenth property, reset with the other nine and by the same
    //  wrapping setter the three angles use, so it is a fourth angle; its
    //  default is zero, which does not narrow it further.  Its *second*
    //  modifier slot (+0x4C0) is the one CancelModifiers clears last.
    SCameraProp m_localAngle;
    //  +0x4C4 / +0x4C8 - Save/Load carry both as plain individual floats
    //  (WriteFloat/ReadFloat, camerasaveload.cpp); nothing else in this
    //  call tree touches either.
    float m_unknownFloats4C4[2];
    //  +0x4CC..+0x4D8 - the bounds ProjectGroundPoint clamps the
    //  camera-space point against, min pair then max pair, each pair with
    //  **y before x**.  Which component each is was open until the camera
    //  family arrived: the four JASS natives GetCameraBoundMinX/MinY/MaxX
    //  /MaxY read +0x4D0, +0x4CC, +0x4D8 and +0x4D4 in that order, which
    //  names all four.
    float       m_boundsMinY;   // +0x4CC
    float       m_boundsMinX;   // +0x4D0
    float       m_boundsMaxY;   // +0x4D4
    float       m_boundsMaxX;   // +0x4D8
    //  +0x4DC..+0x500 - ten more plain individual floats Save/Load carry
    //  the same way as +0x4C4/+0x4C8 above; still nothing else reads any
    //  one of them on its own.
    float m_unknownFloats4DC[10];
    //  +0x504..+0x528 - a second flat 9-dword raw block, the same
    //  WriteDwordArray/ReadDwordArray shape as +0x60's own (gameui.h above).
    unsigned int m_rawBlock504[9];
    //  +0x528 / +0x54C - two 3x3 matrices, row major.  The first takes a
    //  ground point into the space the bounds above are expressed in; the
    //  second takes the clamped result back out again.  Only the first two
    //  rows are ever read - the third component of the input is a literal
    //  zero at every use.
    float       m_toBoundsSpace[9];    // +0x528
    float       m_fromBoundsSpace[9];  // +0x54C
    //  +0x570..+0x590 - the nine weights of the terrain height sample
    //  ProjectGroundPoint takes around the projected point.
    float       m_sampleWeights[9];
    char m_reserved594[0x5BC - 0x594];
    //  +0x5BC - the smoothing factor the JASS native
    //  `CameraSetSmoothingFactor` writes; SetSmoothingFactor is the only
    //  writer in this family and clamps it into [0, kMax] first.
    float       m_smoothingFactor;   // +0x5BC
};

#endif
