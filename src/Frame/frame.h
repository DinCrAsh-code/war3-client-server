//============================================================================
//  Storm's frame library - the UI widget framework `CGameUI` is built out of.
//
//  Every class name below is the shipped binary's own, not an invention:
//
//    * the Storm allocation tags name the modules - ".\\CFrame.cpp"
//      (0x6F6084C0), ".\\CFramePoint.cpp" (0x6F6292AD), ".\\CSimpleButton.cpp"
//      (0x6F603271), ".\\CSimpleMessageFrame.cpp" (0x6F603B23),
//      ".\\CSimpleRender.cpp" (0x6F60C9BC);
//    * the constructors stamp their own vtable symbols, which IDA prints
//      verbatim - ??_7CFrame@@6B@ / ??_7CFrame@@6B@_0 (0x6F608253),
//      ??_7CSimpleFrame@@6B@ (0x6F609CA1), ??_7CSimpleButton@@6B@
//      (0x6F603116), ??_7CSimpleMessageFrame@@6B@ (0x6F60482F),
//      ??_7CSimpleGrid@@6B@ (0x6F612CBF), ??_7CSimpleTop@@6B@ (0x6F60BE32),
//      ??_7CSimpleTexture@@6B@ (0x6F60D942), ??_7CSimpleFontString@@6B@
//      (0x6F60D59B), ??_7CScreenFrame@@6B@ (0x6F60A7E3), ??_7CSpriteFrame@@6B@
//      (0x6F60F071), ??_7CButtonFrame@@6B@ (0x6F622113),
//      ??_7CFramePointRelative@@6B@ (0x6F6294B1), ??_7CEventTimer@@6B@
//      (0x6F62BBD7) and ??_7FRAMENODE@CLayoutFrame@@6B@ (0x6F6066FD);
//    * the RTTI type-descriptor names the allocator is handed name the rest -
//      ".?AUFRAMENODE@CLayoutFrame@@", ".?AULIGHTNODE@CFrame@@" and
//      ".?AVCFramePointRelative@@".
//
//  **Two parallel hierarchies, not one.**  CFrame and CSimpleFrame both
//  derive from CLayoutFrame (the frame-point holder), and their own fields
//  both start at the byte CLayoutFrame ends on - CFrame at +0x11C because
//  CLayoutFrame is its *second* base at +0xB4, CSimpleFrame at +0x68 because
//  it is its first.  That is the whole reason the two classes' field
//  offsets overlap and are not a contradiction.
//
//  Sizes are the shipped allocations: sizeof(CFrame) is the 0x168 CFrame.cpp
//  hands SMemAlloc at 0x6F6084C5, sizeof(CSimpleFrame) is 0x124 because
//  CSimpleButton's own fields start there, and so on down the chain.
//============================================================================
#ifndef FRAME_H
#define FRAME_H

#include "storm.h"
#include "observer.h"
#include "dataallocator.h"
#include "conditionvariable.h"
#include "clayer.h"
//  SUnitCreateParamsSub - the 12-byte base 0x6F011300 constructs.
//  RCStaticString below derives from it, and Unit/unitcreateparams.h
//  is where that one shipped address keeps its one C++ name.
#include "unitcreateparams.h"

class CFramePoint;
class CLayoutFrame;

//----------------------------------------------------------------------------
//  One entry of a CLayoutFrame's per-key node list.  0x6F6066B0 allocates it
//  from the frame-node pool with the RTTI name ".?AUFRAMENODE@CLayoutFrame@@"
//  and links it at the head of the owner's list.
//----------------------------------------------------------------------------
struct FRAMENODE
{
    //  Slot 0, invoked by the pool's own release path as `vtbl[0](this, 0)`.
    //  It stays a `virtual` - the vptr it puts at +0 is part of this record's
    //  layout, and taking it away moves every field after it - and its body
    //  is in vtableboundary.cpp: a dispatch through whatever is really in
    //  slot 0, because the shipped objects carry the shipped vtable and
    //  nothing here ever stamps ours.
    virtual void Destroy(int deleteFlags);

    TSLink<FRAMENODE> m_link;      // +0x04
    void*             m_pKey;      // +0x0C  the frame this node speaks for
    int               m_flags;     // +0x10  or-ed with, never replaced
};

//----------------------------------------------------------------------------
//  A frame point: one anchor of a frame's layout.  The base carries only the
//  vtable; the relative flavour (0x6F629490) adds the two frames it ties
//  together and the offset between them.
//
//  Slot 3 is the flavour test: 0x6F629440 asks it and picks which of the two
//  pools to hand the block back to.
//----------------------------------------------------------------------------
class CFramePoint
{
public:
    //  Slot 0, invoked as `vtbl[0](this, 0)` by both release paths.  Body in
    //  vtableboundary.cpp, for the reason FRAMENODE::Destroy gives.
    virtual void Destroy(int deleteFlags);

    //  Slot 3 - the frame this point is anchored *to*, or null when it is an
    //  absolute point.  ClearPoints uses the answer as a receiver and
    //  Release only tests it, which is what says it is a pointer and not a
    //  flag.  A hand-written slot dispatch rather than a fourth `virtual`,
    //  because
    //  this class's real vtable is much longer than the one slot this batch
    //  can name and inventing the two in between would put them in the
    //  wrong places.
    CLayoutFrame* GetRelativeFrame()
    {
        typedef CLayoutFrame* (__thiscall *Fn)(void*);
        return ((Fn)(*(void***)this)[3])(this);
    }

    //  0x6F629440 - drop this point back into whichever pool it came from.
    void Release();
};

class CFramePointRelative : public CFramePoint
{
public:
    //  Inline on purpose: the shipped factory at 0x6F629490 has all five
    //  stores in its own body, behind the null test placement-new puts
    //  there, with no call of its own.
    CFramePointRelative(CLayoutFrame* owner, int otherPoint,
                        float x, float y)
    {
        m_pOwner = owner;
        m_otherPoint = otherPoint;
        m_offsetX = x;
        m_offsetY = y;
    }

    CLayoutFrame* m_pOwner;      // +0x04
    int           m_otherPoint;  // +0x08
    float         m_offsetX;     // +0x0C
    float         m_offsetY;     // +0x10
};

//----------------------------------------------------------------------------
//  The other flavour.  The class name is the shipped one: 0x6F629460 stamps
//  `??_7CFramePointAbsolute@@6B@` into every block it hands out, and the
//  RTTI descriptor it passes the pool as an allocation tag is
//  ".?AVCFramePointAbsolute@@".
//
//  It inherits rather than writing the vtable as a literal, because
//  CFramePoint already declares a `virtual` - so MSVC stamps
//  ??_7CFramePointAbsolute@@6B@ itself and the operand is a symbol on both
//  sides (docs/msvc-vc8-idioms.md, "... but a base class that already has
//  one costs nothing to inherit from").
//----------------------------------------------------------------------------
class CFramePointAbsolute : public CFramePoint
{
public:
    //  Inline for the same reason CFramePointRelative's is: the shipped
    //  factory at 0x6F629460 has both stores in its own body.
    CFramePointAbsolute(float x, float y)
    {
        m_x = x;
        m_y = y;
    }

    float m_x;                   // +0x04
    float m_y;                   // +0x08
};

//  0x6F629490 - framepoint.cpp.
CFramePoint* __fastcall MakeRelativeFramePoint(CLayoutFrame* frame,
                                               int otherPoint,
                                               float x, float y);
//  0x6F629460 - framepoint.cpp.  `__stdcall`, not the `__fastcall` its
//  relative sibling is: the shipped body takes both floats off the stack and
//  puts nothing in a register, because MSVC never register-allocates a float
//  argument and there is no other parameter to take ecx.
CFramePoint* __stdcall MakeAbsoluteFramePoint(float x, float y);
//  0x6F6292A0 / 0x6F6292C0 - framepointrelease.cpp.  Two shipped functions,
//  one per pool, that CFramePoint::Release tail-jumps into.
void __fastcall ReleaseAbsoluteFramePoint(CFramePoint* point);
void __fastcall ReleaseRelativeFramePoint(CFramePoint* point);

//----------------------------------------------------------------------------
//  A plain 4-float rect, the shape CLayoutFrame::m_rect and every function
//  below that reads or writes "a rect pointer" uses: x0, y0, x1, y1, the
//  same order the struct literal at m_rect's own declaration names.
//----------------------------------------------------------------------------
struct SFrameRect
{
    float x0, y0, x1, y1;
};

//----------------------------------------------------------------------------
//  CLayoutFrame - the frame-point holder, [0x00, 0x68).
//
//  The list at +0x38 is a TSList with the *link offset folded in as a
//  constant* (`lea ecx,[eax+4]` in 0x6F6066B0, never a load of the field),
//  which is what says it is TSList rather than TSExplicitList - see
//  Storm/storm.h.  Link(0) is the terminator at +0x3C, and that is why the
//  allocation-failed path in 0x6F6066B0 falls into the identical link
//  sequence with `lea ecx,[ebx+3Ch]`.
//----------------------------------------------------------------------------
class CLayoutFrame
{
public:
    CLayoutFrame();                              // 0x6F6065C0

    //  Slot 0 - the pool release path calls it as `vtbl[0](this, flags)`.
    //  Body in vtableboundary.cpp, for the reason FRAMENODE::Destroy gives.
    virtual void DestroyLayout(int deleteFlags);

    //  Slot 1 - "is this frame already on the re-layout queue?".  Hand-written
    //  for the same reason CFramePoint::IsRelative is.
    int IsLayoutQueued()
    {
        typedef int (__thiscall *Fn)(void*);
        return ((Fn)(*(void***)this)[1])(this);
    }

    //  0x6F605CC0 - queue this frame onto the global re-layout list.  A
    //  non-zero `checkQueued` asks slot 1 first and does nothing when it
    //  answers yes; every call site in this batch passes 0.
    void QueueForLayout(int checkQueued);
    //  0x6F605C70 - drop this frame's node for `key`, if it has one.
    void RemoveDependency(void* key);
    //  0x6F605D90 / 0x6F605DB0 - set one component of the frame's own size
    //  and queue it.  Two functions, not one with an index: the shipped
    //  build has both, each ending in the same tail jump.
    void SetWidth(float width);
    void SetHeight(float height);
    //  0x6F606160 - drop every frame point this frame holds.
    void ClearPoints();
    //  0x6F606210 - ClearPoints's selective sibling: drop only the anchor
    //  points relative to `frame`, then optionally queue for re-layout.
    void RemovePointsRelativeTo(CLayoutFrame* frame, int requeue);
    //  0x6F6061B0 - anchor point `index` at an absolute screen position.
    void SetPoint(int index, float x, float y, int relayout);
    //  0x6F606770 - anchor point `index` to `otherPoint` of `other`.
    void SetPointTo(int index, CLayoutFrame* other, int otherPoint,
                    float x, float y, int relayout);
    //  0x6F6067F0 - the two-point "fill this other frame" shorthand.
    void SetAllPoints(CLayoutFrame* other, int relayout);
    //  0x6F6066B0 - find or create this frame's node for `key` and or `flags`
    //  into it.
    void AddDependency(void* key, int flags);
    //  0x6F605D40 - take a new scale, and re-lay this frame out only when it
    //  really changed.  Frame/layoutscale.cpp.
    void SetScale(float scale);
    //  0x6F607930 - the same epsilon test, and then the new scale pushed down
    //  the owning CFrame's whole subtree through each child's slot 5.
    //  Frame/layoutscaletree.cpp.
    void SetScaleRecursive(float scale);
    //  0x6F605DD0 - slot 0's own first callee (Frame/simpleframevtable0.cpp):
    //  diff the old m_rect against `rect` on all four edges (each compared
    //  with the same |a-b|<epsilon test FloatNearlyEqual below implements,
    //  inlined here rather than called) into a 6-bit "what changed" mask,
    //  then - only when m_54 (this frame's own re-layout-queued flag) says
    //  a re-layout is already pending - walk the dependants list at
    //  dword_6FA9A990[this] and re-queue (QueueForLayout(0), already named)
    //  every dependant whose own +0x10 flags word intersects the mask.
    //  Left a thunk: the six-bit-mask arithmetic is real but nothing this
    //  batch reconstructs reads the mask itself, so getting every one of
    //  its six bit assignments right from the shape alone would be a guess
    //  dressed as a reconstruction.
    void RecomputeQueuedLayout(const SFrameRect* rect);
    //  0x6F605800 (`retn 4`) - slot 1's own callee (Frame/simpleframevtable1.cpp):
    //  compute this frame's own natural content rect (union of its own
    //  anchor points and, per its own call graph, its children's rects) into
    //  `*out`, answering whether it produced one at all.  Left a thunk - a
    //  big, self-contained CLayoutFrame-level walk this batch's one caller
    //  does not need reconstructed to get slot 1 itself right.
    int ComputeContentRect(SFrameRect* out);

    unsigned int m_pointCount;      // +0x04  always 9
    //  Nine, not twelve: the constructor's own memset is
    //  `m_pointCount * 4` bytes from +0x08 and the store into +0x2C is a
    //  separate instruction after it (0x6F60660B..0x6F60661D), so +0x2C is
    //  its own field and the anchor array stops at +0x2C.
    CFramePoint* m_points[9];       // +0x08 .. +0x2C
    int          m_2C;              // +0x2C
    //  dword_6FA9A990's queue link.  A member with a constructor, because the
    //  shipped constructor's two zero stores at +0x30/+0x34 come *before* the
    //  dependents list's own initialisation and after m_pointCount - which is
    //  member-initialiser order and not anything the body can spell.
    TSLink<CLayoutFrame> m_queueLink;   // +0x30 .. +0x38
    TSList<FRAMENODE, 4> m_nodes;   // +0x38 .. +0x44
    float        m_rect[4];         // +0x44  x0, y0, x1, y1
    int          m_54;              // +0x54
    float        m_width;           // +0x58
    float        m_height;          // +0x5C
    float        m_scale;           // +0x60  multiplies a font's point size
    int          m_64;              // +0x64
};

//  CLayoutFrame's slot 5 (+0x14) - "take this scale", dispatched on a child
//  by SetScaleRecursive and on `this` by CSimpleFrame::SetParent.
typedef void (__thiscall *LayoutSetScaleFn)(void* self, float scale);

//----------------------------------------------------------------------------
//  CFrame's first base, [0x00, 0xB4), is CLayer - clayer.h.  This batch used
//  to model it as a second class ("CEventFrame") whose constructor was a
//  redirect to 0x6F600720; that address is CLayer's own constructor, so the
//  two were one class under two names and the redirect shadowed a real
//  reconstruction.  Its event half - SetEventListener, SetAltEventListener,
//  AddPendingState and slot 61 - is declared on CLayer now.
//----------------------------------------------------------------------------

//  The state pair 0x6F601D60 works on lives well past CLayer's own
//  0xB4 bytes, in whichever derived frame the 0x6F36xxxx panels construct;
//  the method is the base's because the shipped module is.
struct SFrameStateOwner
{
    char m_reserved0000[0x1CC];
    int  m_pendingState;        // +0x1CC
    int  m_stateMask;           // +0x1D0
};

//----------------------------------------------------------------------------
//  CFrame - 0x168 bytes, per the SMemAlloc at 0x6F6084C5.
//----------------------------------------------------------------------------
struct LIGHTNODE;

class CFrame : public CLayer, public CLayoutFrame
{
public:
    CFrame(int a, int b, int c);                 // 0x6F608200

    //  0x6F608330 - out of scope for the session that reached it
    //  (CScreenFrame::~CScreenFrame, Frame/screenframedtor.cpp); declared
    //  here so that derived-class destructor calls it for real rather than
    //  inventing a second name for it.
    ~CFrame();

    //  0x6F607E80 - allocate a CFrame::LIGHTNODE, link it at the *tail* of
    //  the light list and fill in its two fields.  Lights are drawn in the
    //  order they were added, which is what the tail splice is for.
    void AddLight(void* light, int flags);

    //  0x6F607C30 - AddLight's twin for fog, plus a dedup step AddLight
    //  does not have: search m_144's own list for a FOGNODE whose flags
    //  already overlap the new ones and reuse it (dropping its held
    //  reference) instead of appending a second one when found.  A first
    //  attempt at a real body (citem-batch6) scored only 0.329 - the splice
    //  shape genuinely differs from a plain `LinkToTail`, not just in
    //  register allocation - so it stays a redirect (framethunks.cpp)
    //  rather than a wrong claim.
    void AddFog(void* fog, int flags);

    //  0x6F608490 - the module's own factory: allocate 0x168 bytes tagged
    //  ".\\CFrame.cpp" and construct a CFrame into them.  Static, and a
    //  genuine __fastcall - two of its three arguments arrive in ecx and edx.
    static CFrame* __fastcall Create(int a, int b, int c);

    //========================================================================
    //  CFrame's own vtable contribution - docs/targets/vtables/CSimpleTop.txt
    //  (0x6F96E2FC) cross-checked address-for-address against a fresh
    //  CWorldFrameWar3 pull (0x6F94157C, both 57 slots): the two share every
    //  slot whose address is identical between them, which is exactly
    //  CFrame's own contribution (nothing beyond it - neither leaf class
    //  adds a slot past 56, they only override some of these 51).  Slot 4 is
    //  CObserver::Method_0x10, *overridden* here (declared where an override
    //  belongs, not renumbered); slots 6-56 are 51 brand-new virtuals no
    //  CObserver-rooted class before this one declared.
    //
    //  Nine of the 51 (10, 11, 12, 14, 16, 27, 34, 35, 45) are where
    //  CSimpleTop and CWorldFrameWar3 disagree - each overrides with its own
    //  address, so CFrame's *own* unoverridden body at each is not evidenced
    //  anywhere in this batch's closure (no plain, non-further-derived CFrame
    //  is ever constructed) and is declared here the same way
    //  CSimpleFrame's own base-case placeholders are: dispatch through
    //  whatever the real, shipped vtable holds (Frame/framevtabledefaults2.cpp).
    //  CSimpleTop overrides all nine below with its own real bodies (three of
    //  them - 11, 12, 27 - kept naked thunks; see the override block for why).
    //  The other 42 are the *same* address on both leaf classes, so the body
    //  here is the real, shared one - not a guess.
    //========================================================================

    //  Slot 4 (+0x10) override of CObserver::Method_0x10 - stamps `this`
    //  into the message's own +0xC field (one dword past what SAgentMessage
    //  currently names) and tail-forwards to the base body.
    //  Frame/framequerystate.cpp.
    virtual int Method_0x10(SAgentMessage* msg);

    //  Slots 6/7/8 (+0x18/+0x1C/+0x20) - three parallel "single owner"
    //  handoffs, each its own global (dword_6FACE66C/674/678) and its own
    //  pair of notify slots on the object losing/gaining the token (0x74/
    //  0x70, 0x94/0x90, 0xA0/0x9C - all inside this same 0-56 range, so on a
    //  CFrame-descended receiver the "old" notify these three slots
    //  themselves dispatch is one of the trivial defaults below unless some
    //  further subclass overrides it).  `take` selects between "give the
    //  token to me" (the bool true) and "give up the token, unless somebody
    //  else already holds it" (false: pass 0 only when I am still the
    //  current holder).  Slot 6's own handoff also resets a shared elapsed-
    //  time float (flt_6FACE660 = g_layerElapsedTime, already named in
    //  clayer.h).  Frame/frameownertoggle.cpp.
    virtual CLayer* Method_0x18(int take);
    virtual CLayer* Method_0x1C(int take);
    virtual CLayer* Method_0x20(int take);

    //  Slot 9 (+0x24) - a real, shared CLayer/CFrame body (0x6F5FD650, 425
    //  lines through its own callees): per-frame world/screen coordinate
    //  propagation through this frame's own children and dependants,
    //  touching six more `flt_6FACE6xx` globals and two more indirect calls
    //  through the receiver's own vtable.  Left a naked thunk - this session
    //  budgeted its time toward CSimpleTop's own 11 slots and the shared
    //  slots tractable within it; the geometry here is real but its exact
    //  source shape is not yet worth guessing at.  Frame/framethunks.cpp.
    virtual void Method_0x24(void* arg);

    //  Slots 10-12 (+0x28/+0x2C/+0x30) - CSimpleTop/CWorldFrameWar3 disagree
    //  here; see the block comment above.  Frame/framevtabledefaults2.cpp.
    virtual int Method_0x28(int unused) const;
    virtual int  Method_0x2C(void* a, float b, float c);
    virtual void Method_0x30();

    //  Slot 13 (+0x34) - if this frame owns any lights or fog (m_144's own
    //  list or m_lights non-empty) and bit 2 of m_124 is *not* set, notify
    //  every matching fog/light node.  Frame/framelightfog.cpp.
    virtual void Method_0x34();
    //  0x6F606EC0 - "does m_124 have every bit of `mask` set".
    int HasAllFlagBits(unsigned int mask);
    //  0x6F607DA0 / 0x6F608020 - slot 13's own two node walks; naked
    //  thunks for the reason Frame/framelightfog.cpp's own header gives.
    void NotifyMatchingFogNodes(unsigned int mask, unsigned int expect);
    void NotifyMatchingLightNodes(unsigned int mask, unsigned int expect);

    //  Slot 14 (+0x38) - disagreement slot (see above).  Same tail-jump-into-
    //  slot-4 shape as slot 15's own family below, so the same signature.
    virtual int Method_0x38(SAgentMessage* msg);

    //  Slot 15 (+0x3C) - a real, shared trivial forwarder: tail-dispatch
    //  this object's own slot 4 (Method_0x10) unchanged.  One of eight
    //  bodily-identical forwarders in this vtable (15, 18-24); see
    //  Frame/framedispatchforward.cpp for why they are not one function.
    virtual int Method_0x3C(SAgentMessage* msg);

    //  Slot 16 (+0x40) - disagreement slot (see above); same shape as 14.
    virtual int Method_0x40(SAgentMessage* msg);

    //  Slot 17 (+0x44) - the one member of the "tail-forward to slot 4"
    //  family that redirects through a global first: if dword_6FACE684 is
    //  set, forward *that* object's own slot 4 instead of this one's.
    //  Frame/framedispatchforward.cpp.
    virtual int Method_0x44(SAgentMessage* msg);

    //  Slots 18-24 (+0x48..+0x60) - the rest of the trivial forwarder
    //  family.  Frame/framedispatchforward.cpp.
    virtual int Method_0x48(SAgentMessage* msg);
    virtual int Method_0x4C(SAgentMessage* msg);
    virtual int Method_0x50(SAgentMessage* msg);
    virtual int Method_0x54(SAgentMessage* msg);
    virtual int Method_0x58(SAgentMessage* msg);
    virtual int Method_0x5C(SAgentMessage* msg);
    virtual int Method_0x60(SAgentMessage* msg);

    //  Slots 25/26 (+0x64/+0x68) - a real, shared "arm/disarm this layer's
    //  own shortcut table" pair, mirroring CSimpleTop's own
    //  RegisterShortcuts/UnregisterShortcuts at the per-CLayer level: 26
    //  releases the keyboard first if this layer currently holds it
    //  (SetKeyboardFocus(0,0,0), already named on CLayer).
    //  Frame/frameshortcutsflag.cpp.
    virtual void Method_0x64();
    virtual void Method_0x68();
    //  0x6F6012F0 / 0x6F601420 - slot 25/26's own two Storm-registration
    //  helpers; naked thunks for the reason Frame/frameshortcutsflag.cpp's
    //  own header gives.
    void ArmShortcutTable(void* list74, void* shortcuts);
    void DisarmShortcutTable(void* list74, void* shortcuts);

    //  Slot 27 (+0x6C) - disagreement slot (see above).
    virtual void Method_0x6C();

    //  Slots 28-30 (+0x70/+0x74/+0x78) - real, shared true no-ops (`retn`,
    //  IDA's own nullsub_1326/1327/1308).  Frame/framevtabledefaults2.cpp.
    virtual void Method_0x70();
    virtual void Method_0x74();
    virtual void Method_0x78();

    //  Slots 31-33 (+0x7C/+0x80/+0x84) - a real, shared "broadcast to every
    //  child layer" family: walk m_children (CLayer's own LAYERNODE list)
    //  and call each child's own slot 31/32/33 in turn, gated respectively
    //  on m_field30 (31), m_float34 (32, non-zero test) and m_field2C (33).
    //  33 also lazily ensures this frame's own screen-space render object
    //  (CScreenFrame::EnsureRenderObject, already named) before it recurses.
    //  Frame/framechildbroadcast.cpp.
    virtual void Method_0x7C(void* arg);
    virtual void Method_0x80(float arg);
    virtual void Method_0x84(void* arg);
    //  0x6F5FADC0 - slot 33's own "ensure the host's screen sprite" helper;
    //  a real reconstruction (Frame/framechildbroadcast.cpp), also reused
    //  as-is by CSimpleTop's own slot 34 (Frame/simpletoprenderobjects.cpp)
    //  since both dispatch through the identical shipped address.
    void EnsureAndConfigureScreenSprite(void* arg);

    //  Slots 34/35 (+0x88/+0x8C) - disagreement slots (see above).  No stack
    //  arguments - both take only `this`.
    virtual void Method_0x88();
    virtual void Method_0x8C();

    //  Slots 36-42 (+0x90..+0xA8) - real, shared true no-ops
    //  (nullsub_1309..1315).  Frame/framevtabledefaults2.cpp.
    virtual void Method_0x90();
    virtual void Method_0x94();
    virtual void Method_0x98();
    virtual void Method_0x9C();
    virtual void Method_0xA0();
    virtual void Method_0xA4();
    virtual void Method_0xA8();

    //  Slot 43 (+0xAC) - real, shared: `return 1`, two stack arguments,
    //  neither read.  Frame/framevtabledefaults2.cpp.
    virtual int Method_0xAC(int a, int b);

    //  Slot 44 (+0xB0) - real, shared true no-op (nullsub_1316).
    virtual void Method_0xB0();

    //  Slot 45 (+0xB4) - disagreement slot; CSimpleTop's own override
    //  (below) is unambiguous, so the name is taken from it even though
    //  CFrame's own un-overridden body is not independently confirmed -
    //  the same inference CSimpleButton::NotifyCaptureGained/Lost already
    //  rests on.  Weak corroborating evidence: CWorldFrameWar3's own
    //  override here is nullsub_1317, a true no-op - consistent with this
    //  slot being a rarely-meaningful "something changed" hook most
    //  CFrame-descended classes leave empty.
    virtual void MarkAllDirty();

    //  Slot 46/47 (+0xB8/+0xBC) - real, shared rect/anchor geometry
    //  (0x6F606BD0/0x6F606CF0) this session left naked thunks for the same
    //  time-budget reason slot 9 above did.  Frame/framethunks.cpp.
    virtual void Method_0xB8(void* a, void* b);
    virtual void Method_0xBC(void* a, void* b, void* c);

    //  Slots 48-50 (+0xC0/+0xC4/+0xC8) - real, shared `return 0` family
    //  (one stack arg / none / none).  Frame/framevtabledefaults2.cpp.
    virtual int Method_0xC0(int a);
    virtual int Method_0xC4();
    virtual int Method_0xC8();

    //  Slot 51 (+0xCC) - real, shared: copies three globals
    //  (dword_6FACE690/694/698) into a 12-byte out-parameter.  `this` is
    //  never read.  Frame/framequerystate.cpp.
    virtual void Method_0xCC(void* out);

    //  Slot 52/53 (+0xD0/+0xD4) - a real, shared "set/clear bit 0 or 1 of
    //  the CLayer state word and cascade to children" pair (0x6F5FE6F0/
    //  0x6F5FE690), each ending in a tail-call into its own slot 26 - left
    //  naked thunks, same reason as 9/46/47.  Frame/framethunks.cpp.
    virtual void Method_0xD0();
    virtual void Method_0xD4();

    //  Slot 54 (+0xD8) - real, shared: detach this frame's own subtree,
    //  release whatever m_160/m_164 name and free m_160 if set.
    //  Frame/frameteardown.cpp.
    virtual void Method_0xD8(void* arg);
    //  0x6F601520 - slot 54's own subtree-detach helper; naked thunk, see
    //  Frame/frameteardown.cpp's own header comment.
    void DetachSubtree(void* arg);

    //  Slot 55 (+0xDC) - real, shared: `return 1`, two stack arguments,
    //  neither read (same shape as slot 43).  Frame/framevtabledefaults2.cpp.
    virtual int Method_0xDC(int a, int b);

    //  Slot 56 (+0xE0) - real, shared screen-offset accumulation
    //  (0x6F6071A0) - naked thunk, same reason as 9/46/47/52/53.
    //  Frame/framethunks.cpp.
    virtual void Method_0xE0(float* out);

    int   m_11C;                    // +0x11C
    int   m_120;                    // +0x120
    int   m_124;                    // +0x124
    int   m_128;                    // +0x128
    float m_12C;                    // +0x12C
    float m_130;                    // +0x130
    float m_134;                    // +0x134
    float m_138;                    // +0x138
    float m_13C;                    // +0x13C
    int   m_140;                    // +0x140
    TSList<void, 0>      m_144;     // +0x144 .. +0x150
    TSList<LIGHTNODE, 0> m_lights;  // +0x150 .. +0x15C
    int   m_lightCount;             // +0x15C
    int   m_160;                    // +0x160
    int   m_164;                    // +0x164
};

//  One entry of CFrame's light list - ".?AULIGHTNODE@CFrame@@", 16 bytes,
//  allocated straight from Storm rather than from a pool.
struct LIGHTNODE
{
    //  Inline: the shipped AddLight has all four stores in its own body,
    //  behind the null test placement-new puts there.
    LIGHTNODE() { m_pLight = 0; m_flags = 0; }

    TSLink<LIGHTNODE> m_link;   // +0x00
    void*             m_pLight; // +0x08
    int               m_flags;  // +0x0C
};

//----------------------------------------------------------------------------
//  The node types the frame lists hold.  Both are the RTTI descriptor names
//  the pool allocations are tagged with - ".?AUREGIONNODE@@" and
//  ".?AUSIMPLEFRAMENODE@@" - and neither carries a vtable, so its link is its
//  first member and every list of them folds a link offset of zero.
//----------------------------------------------------------------------------
struct REGIONNODE
{
    TSLink<REGIONNODE>  m_link;     // +0x00
    void*               m_pRegion;  // +0x08
};

struct SIMPLEFRAMENODE
{
    TSLink<SIMPLEFRAMENODE> m_link;     // +0x00
    void*                   m_pFrame;   // +0x08
};

//  0x6F60C0A0 - CSimpleTop::Method_0x2C's own drain: unlink and free every
//  node still on `*list`, one at a time, into g_simpleFrameNodeAllocator
//  (the same pool AddFrame/RemoveChildNode already allocate
//  SIMPLEFRAMENODEs from).  A free `__fastcall` function taking the list by
//  pointer, not a CSimpleTop member: the shipped call site loads `ecx` as
//  `&m_pendingHide` directly (`lea ecx, [this+0x180]`), which only a
//  free/static function with the list as its own first parameter
//  reproduces - a member's own `this` would put CSimpleTop* in ecx
//  instead.  Frame/simpletopflush.cpp.
void __fastcall DrainPendingHideList(TSList<SIMPLEFRAMENODE, 0>* list);

//  A list of REGIONNODEs plus the one operation that is not the template's:
//  draining a node hands it back to *this* list's own pool, so the helper is
//  per-node-type rather than a member of TSList.
class CRegionNodeList : public TSList<REGIONNODE, 0>
{
public:
    //  0x6F609730 - unlink `node`, hand it back to the pool and answer the
    //  predecessor the caller's walk should continue from.
    REGIONNODE* RemoveNode(REGIONNODE* node);
};

//----------------------------------------------------------------------------
//  The five 12-byte records CSimpleFrame embeds at +0xDC and the ten
//  CSimpleTop embeds at +0x18C are built by the vector-constructor iterator
//  over sub_6F6091F0/sub_6F609250 and destroyed by sub_6F609210/sub_6F609270.
//  Neither pair is in this batch, so the element's own constructor and
//  destructor run the shipped bodies - but the five at +0xDC are region node
//  lists all the same (CSimpleFrame::AddLayerRegionNode links into them with
//  a folded link offset of zero), which is why this one derives.
//----------------------------------------------------------------------------
struct SFrameSlot : public CRegionNodeList
{
    SFrameSlot();                   // 0x6F6091F0
    ~SFrameSlot();                  // 0x6F609210
};

//  Same shape as SFrameSlot above (a CRegionNodeList plus its own real
//  ctor/dtor, nothing else) - confirmed, not just assumed, by
//  CSimpleTop::Method_0x2C/Method_0x30 (simpletopflush.cpp,
//  simpletoprebuilddirtybatches.cpp): both walk `this + 0x18C +
//  layer*0xC` and read *that address's own +8* directly as a raw
//  REGIONNODE-list tail (`m_terminator.m_prevlink`, the layout
//  TSList<REGIONNODE,0>/CRegionNodeList gives for free at the same
//  offset), then dereference each node's own +8 as a REGIONNODE::m_pRegion.
//  Both call sites read the tail *raw* rather than through Tail()'s own
//  branchless clamp, so they walk the list by hand rather than through the
//  inherited accessors - see those two files for why.
struct SFrameLayer : public CRegionNodeList
{
    SFrameLayer();                  // 0x6F609250
    ~SFrameLayer();                 // 0x6F609270
};

//----------------------------------------------------------------------------
//  ".PAVFRAMEPRIORITY@@" - one frame's membership of one of CSimpleTop's draw
//  layers.  The array element is a *pointer* to this record, which is what
//  makes the RTTI descriptor the Storm calls carry begin `.PAV` and not
//  `.?AV`.
//----------------------------------------------------------------------------
class CSimpleFrame;

struct FRAMEPRIORITY
{
    CSimpleFrame*   m_pFrame;       // +0x00
    unsigned int    m_priority;     // +0x04
};

//----------------------------------------------------------------------------
//  One of CSimpleTop's four per-layer draw lists, 24 bytes: a Storm growable
//  array of FRAMEPRIORITY* kept sorted by descending priority, plus this
//  class's own used/reserved pair.  The stride is the shipped
//  `lea eax,[eax+eax*2]` / `lea esi,[ebp+eax*8+874h]` in 0x6F60BCE0, and the
//  element count is the `push 4` its constructor's vector iterator takes.
//----------------------------------------------------------------------------
class CFramePriorityArray
{
public:
    CFramePriorityArray();          // 0x6F60BB70
    ~CFramePriorityArray();         // 0x6F60BB90

    //  0x6F60ADD0 - the highest power of two not greater than `count`,
    //  capped at 64, and the cap is the only case that is cached.
    unsigned int ComputeChunk(unsigned int count);
    //  0x6F60B020 - resize the storage, in place when Storm can.
    void SetAlloc(unsigned int alloc);
    //  0x6F60BAF0 - grow to hold `count` elements, rounded up to a chunk.
    void SetCount(unsigned int count);
    //  0x6F60BBC0 - insert in descending-priority order.
    void Insert(FRAMEPRIORITY* entry);
    //  0x6F60BC50 - erase the element at `index` and shrink by a chunk when
    //  the slack passes eight.
    void EraseAt(unsigned int index);

    unsigned int    m_alloc;        // +0x00
    unsigned int    m_count;        // +0x04
    FRAMEPRIORITY** m_data;         // +0x08
    unsigned int    m_chunk;        // +0x0C
    unsigned int    m_used;         // +0x10
    unsigned int    m_reserved;     // +0x14
};

//----------------------------------------------------------------------------
//  CSimpleFrame - 0x124 bytes, because CSimpleButton's own fields start
//  there.
//----------------------------------------------------------------------------
class CSimpleTop;

class CSimpleFrame : public CLayoutFrame
{
public:
    CSimpleFrame(void* parent);                  // 0x6F609C70
    //  0x6F609DD0 - the base destructor.  Not reconstructed by this batch
    //  (out of its own 35 addresses; an `__except_handler4`-framed, ~190-
    //  instruction body tearing down every list and array this class owns).
    //  Declared as a real destructor (rather than a same-signature helper
    //  class called explicitly) so that CSimpleGrid::~CSimpleGrid's own
    //  *implicit* base-destructor call - which the compiler emits
    //  regardless, once any base up this chain has a non-trivial destructor -
    //  has this real target to tail-jump into instead of duplicating it.
    ~CSimpleFrame();

    //  0x6F6088F0 - raise flag bit `bit`; the first time it goes up, tell the
    //  owning CSimpleTop.
    void RaiseFlagBit(int bit, int arg);
    //  0x6F608930 - RaiseFlagBit's opposite number.
    //  Frame/simpleframeclearflag.cpp.
    void ClearFlagBit(int bit);
    //  0x6F6088C0 - set my own (depth, layer) cell of CSimpleTop's dirty
    //  grid.  Frame/simpleframedirty.cpp.
    void MarkLayerDirty(int layer);
    //  0x6F608C50 - drop one layer's priority back to zero and mark that
    //  layer dirty.  Frame/simpleframepriority.cpp.  It is *not*
    //  MarkLayerDirty: the shipped body writes +0xB8[layer] and only then
    //  tail-jumps into 0x6F6088C0, which is the function that owns that name.
    void ResetLayerPriority(int layer);
    //  0x6F608C30 - ResetLayerPriority's own "raise to 1" counterpart;
    //  CSimpleButton's own NotifyCaptureGained calls it on layer 4 (the
    //  highlight/overlay draw layer) as its own press-visual half of the
    //  pair NotifyCaptureLost's ResetLayerPriority(4) undoes.
    void RaiseLayerPriority(int layer);
    //  0x6F609980 - re-parent, taking the new parent's depth and scale as it
    //  goes.  Frame/simpleframeparent.cpp.
    void SetParent(void* parent);
    //  0x6F608550 - adopt a frame this one owns.
    //  Frame/simpleframeowned.cpp.
    void SetOwnedFrame(void* frame);
    //  0x6F2F1010 - raise the shown flag and tail-jump into slot 0x68.
    //  Reconstructed by the JASS_PauseGame slice in
    //  GameUI/gameuiframehide.cpp; declared here now that the class is.
    void Hide();

    //  0x6F6094F0 / 0x6F609810 - the unlayered region list at +0xD0.
    //  Frame/simpleframeregions.cpp.
    void AddRegionNode(void* region);
    void RemoveRegionNode(void* region);
    //  0x6F609580 / 0x6F609850 - the five per-layer region lists at +0xDC.
    //  Frame/simpleframelayerregions.cpp.
    void AddLayerRegionNode(void* region, int layer);
    void RemoveLayerRegionNode(void* region, int layer);
    //  0x6F609630 / 0x6F6098B0 - the child list at +0x118.
    //  Frame/simpleframechildren.cpp.
    void AddChildNode(void* child);
    SIMPLEFRAMENODE* RemoveChildNode(void* child);
    //  0x6F6085E0 - slot 0's own second callee: fold `rect` and this
    //  frame's own m_insetX0/Y0/X1/Y1 into the cached screen rect
    //  (m_screenX0..m_screenY1).  See both fields' own comments above.
    void RecomputeScreenRect(const SFrameRect* rect);
    //  0x6F604FC0 - see framethunks.h's own comment on this declaration.
    void RebuildBackdropRect(int forceRebuild);

    //========================================================================
    //  CSimpleButton's 29-slot vtable (0x6F96DC8C, docs/targets/vtables/
    //  CSimpleButton.txt) turned out to be CLayoutFrame's one inherited
    //  DestroyLayout override (slot 0) plus 28 further slots this session
    //  found nothing already covering by address anywhere in src/.  Nine
    //  more of them (1, 3-7, 10, 14, 25, 26) read only CLayoutFrame/
    //  CSimpleFrame's own fields and touch nothing CSimpleButton-specific,
    //  so they are declared here as CSimpleFrame's own virtuals, in the
    //  exact order the real vtable has them - MSVC assigns a class's own
    //  new virtual slots in declaration order, so this block's order *is*
    //  the vtable's order and must not be reordered.  Ten more (8, 9, 11-13,
    //  16-19, 22) are the shared "default: do nothing / return a constant"
    //  family every CLayoutFrame-derived class not overriding them shares
    //  (Frame/simpleframevtabledefaults.cpp).  The remaining seven (2, 20,
    //  21, 23, 24, 27, 28) sit at addresses inside the CSimpleButton.cpp
    //  module and touch CSimpleButton's own fields (m_enabled, m_124,
    //  m_140...) - CSimpleFrame declares them too (a class between
    //  CLayoutFrame and CSimpleButton needs *some* body at every slot it
    //  passes through), but only ever as the same "dispatch through
    //  whatever the real, shipped vtable holds" placeholder
    //  vtableboundary.cpp already uses for CLayoutFrame's own slot 0 - this
    //  batch has no dump for what a *plain* CSimpleFrame (not a button)
    //  does at any of the seven, and CSimpleButton overrides every one of
    //  them with the real body below.
    //========================================================================

    //  Slot 0 (+0x00, override of CLayoutFrame::DestroyLayout) - despite the
    //  inherited name and its `int` parameter, the real body
    //  (Frame/simpleframevtable0.cpp) never treats it as a delete-flags
    //  word: it reads it as a `const SFrameRect*` (the shipped caller is
    //  CLayoutFrame::SetRect, slot 3 below, which passes its own rect
    //  argument straight through), diffs the frame's own scaled-rect cache
    //  (m_98..m_A4) against it, and if either axis changed, cascades the
    //  same rect into m_pOwnedFrame.  It cannot be renamed - overriding a
    //  base `virtual` requires the identical name and signature - so the
    //  parameter keeps the name the base gave it and the comment carries
    //  the correction instead.
    virtual void DestroyLayout(int deleteFlags);

    //  Slot 1 (+0x04) - recompute this frame's own natural content rect
    //  (0x6F605800, a big CLayoutFrame-level "union of my own points and my
    //  children's rects" walk this batch thunks rather than reconstructs)
    //  and, if it produced one, apply it through slot 3 (SetRect) below.
    //  No arguments; Frame/simpleframevtable1.cpp.
    virtual void UpdateContentRect();

    //  Slot 2 (+0x08) - CSimpleButton's own deleting destructor; every
    //  other CLayoutFrame-derived class not yet reconstructed still needs
    //  *some* body here, so this default dispatches through the real
    //  vtable (Frame/simpleframevtabledefaults.cpp) the same way slot 0's
    //  base case would if CSimpleFrame did not override it either.
    virtual void* Destroy2(int deleteFlags);

    //  Slot 3 (+0x0C) - CLayoutFrame::SetRect: dispatch slot 0 above with
    //  the *old* rect still in m_rect (so it can diff against the new one),
    //  then store the four floats, then - only for a frame with a backdrop
    //  generator (m_64 != 0, CBackdropGenerator - SetOwnedFrame is what
    //  sets it) - rebuild the backdrop through 0x6F604FC0 (thunked; the
    //  nine-piece border geometry is CBackdropGenerator's own module, not
    //  this one's).  Frame/simpleframevtable3.cpp.
    //  Always returns 1 (m_54, just set) - the real body reloads it from
    //  memory rather than keeping the constant in a register, so the
    //  reconstruction does the same rather than declaring `void`.
    virtual int SetRect(const SFrameRect* rect);
    //  Slot 4 (+0x10) - CLayoutFrame::GetRect: 1 and a copy of m_rect once
    //  SetRect has run at least once (m_54 != 0), 0 otherwise.
    //  Frame/simpleframevtable4.cpp.
    virtual int GetRect(SFrameRect* out) const;
    //  Slot 5 (+0x14) - not CLayoutFrame::SetScaleRecursive (0x6F607930,
    //  already named, a different address): this one calls
    //  CLayoutFrame::SetScale directly and then pushes the same scale into
    //  two of CSimpleFrame's own lists (+0xD8, +0x120) via each entry's own
    //  slot 5, rather than the generic subtree walk SetScaleRecursive does.
    //  Frame/simpleframevtable5.cpp.
    virtual void PropagateScaleToChildren(float scale);
    //  Slot 6 / 7 (+0x18 / +0x1C) - m_scale * m_width, m_scale * m_height.
    //  No arguments, no callees; Frame/simpleframevtable6.cpp.
    virtual float GetScaledWidth() const;
    virtual float GetScaledHeight() const;

    //  Slots 8, 9, 11-13, 16-19, 22 - the ten-function "default: do
    //  nothing, or return a fixed constant" family every one of this
    //  session's addresses in that range turned out to be (confirmed by
    //  `bytes`, not guessed): three true no-ops (11, 12 are `retn`, 13 is
    //  `retn 4`), one `return 0` with no arguments (8), one `return 1` with
    //  two stack arguments (9), and five `return 0` with one stack argument
    //  (16-19, 22).  Nothing in this class's own closure calls any of them,
    //  so the names are placeholders the same way CObserver::Method_0x0C is
    //  one - Frame/simpleframevtabledefaults.cpp.
    virtual int Method_0x20();
    virtual int Method_0x24(int a, int b);
    //  Slot 10 (+0x28) - CLayoutFrame::PointInRect: is point `p` (a 2-float
    //  {x,y}) inside the frame's own scaled screen rect (m_98..m_A4), once
    //  that rect has been computed (m_54 != 0)?  Frame/simpleframevtable10.cpp.
    virtual int PointInRect(const float* p) const;
    virtual void Method_0x2C();
    virtual void Method_0x30();
    virtual void Method_0x34(int a);
    //  Slot 14 (+0x38) - pull the two floats a message-shaped struct keeps
    //  at +0x24/+0x28 (the same offsets slots 20/21 below read a mouse
    //  message's x/y from) and forward them to slot 10 above.
    //  Frame/simpleframevtable14.cpp.
    virtual int HitTestMessage(const void* msg) const;
    //  Slot 15 (+0x3C) - dispatch slot 12 (Method_0x30, a no-op on this
    //  class) on every child of one of a per-layer list at
    //  `this + layer*0xC + 0xE0` whose own rect is known; read as a generic
    //  "notify every child of a layer" broadcast rather than anything
    //  CSimpleButton gives real meaning to.  Kept a naked thunk
    //  (framethunks.cpp) rather than a hand-typed reconstruction: the
    //  +0xE0-based per-layer array it walks does not match
    //  m_layerRegions/SFrameSlot's own already-reconstructed shape closely
    //  enough for this session to be confident of its real field layout,
    //  and a virtual can be a thunk exactly the same way a non-virtual one
    //  (CSimpleButton::AttachSlotTexture) already is in this file.
    virtual int Method_0x3C(int layer, void* msg);
    virtual int Method_0x40(int a);
    virtual int Method_0x44(int a);
    virtual int Method_0x48(int a);
    virtual int Method_0x4C(int a);

    //  Slots 20, 21 - CSimpleButton's own mouse-down/mouse-up slots;
    //  declared here only as a dispatch-through-the-real-vtable placeholder
    //  for a CLayoutFrame-derived class that is not CSimpleButton, exactly
    //  as slot 0's own base case works.  Frame/simpleframevtabledefaults.cpp.
    virtual int Method_0x50(const void* msg);
    virtual int Method_0x54(const void* msg);
    //  Slot 22 (+0x58) - the last of the ten-function default family above;
    //  split out from that block only because slots 20/21 (CSimpleButton-
    //  specific) sit between it and slot 19 in the real vtable.  Same
    //  shape as 16-19: one stack argument, `return 0`.
    //  Frame/simpleframevtabledefaults.cpp.
    virtual int Method_0x58(int a);
    //  Slot 23 (+0x5C) - the acquire-side counterpart to NotifyCaptureLost
    //  below: same shape, opposite polarity (CSimpleButton's own override
    //  sets the pressed-layer priority and notifies m_70's slot 26 instead
    //  of undoing them), so it is very likely dispatched by whatever sets
    //  CSimpleTop::m_pCapturedFrame nonzero the same way
    //  CSimpleTop::RemoveFrameFromLayer dispatches slot 24 the moment it
    //  clears that field - see docs/notes/pause-mouse-capture.md.  No
    //  confirmed default body of its own (CSimpleButton's is the only one
    //  this session read); this is the same base-case placeholder.
    virtual void NotifyCaptureGained();
    //  Slot 24 (+0x60) - "you are no longer the captured frame".  Was a
    //  hand-dispatch method; now a real virtual so CSimpleButton can
    //  actually override it instead of only ever reaching the shipped
    //  vtable through the raw index.
    virtual void NotifyCaptureLost();
    //  Slot 25 (+0x64) - the hide half of the pair slot 26 shows: pull this
    //  frame out of every draw layer and region list it is in
    //  (CSimpleTop::RemoveFrameFromLayer, already named, plus
    //  RemoveFromLayerRegions below), dispatch slot 12 (Method_0x30, a
    //  no-op) on self, clear m_attached, and cascade to the +0x120 list.
    //  Frame/simpleframevtable25.cpp.
    virtual void NotifyHidden();
    //  Slot 26 (+0x68) - the "I am fully built" notice the constructor ends
    //  on: the mirror of slot 25 (AddFrameToLayer, AddToLayerRegions, slot
    //  11/Method_0x2C, m_attached = 1, cascade).  Frame/simpleframevtable26.cpp.
    virtual void NotifyConstructed();
    //  Slot 27/28 - CSimpleButton's click-fire and visual-state slots;
    //  base-case placeholder, exactly as the others in this run of seven.
    virtual void Method_0x6Cb(int id);
    virtual void NotifyEnabledChanged(int a, int b);

    CSimpleTop*   m_pTop;           // +0x68  dword_6FACE758, the singleton
    CSimpleFrame* m_pParent;        // +0x6C
    //  Read by CSimpleButton's own slot-23/24 overrides as a CSimpleFrame*
    //  (a secondary child frame - a highlight/caption overlay is the best
    //  guess - whose own slot 26/25 gets told when this button gains/loses
    //  capture) and by nothing else this batch reconstructs, so the type
    //  is this session's inference from that one read, not a confirmed
    //  fact.
    CSimpleFrame* m_70;             // +0x70
    int         m_74;               // +0x74
    int         m_78;               // +0x78
    int         m_7C;               // +0x7C
    int         m_80;               // +0x80
    int         m_depth;            // +0x84  the dirty grid's row index
    unsigned char m_88;             // +0x88  starts 0xFF
    char        m_reserved89[0x03]; // +0x89
    int         m_flags;            // +0x8C
    //  +0x90 is the *shown* flag and not a hidden one: SetParent stores 0
    //  into it before dispatching slot 25 and 1 before slot 26, and the
    //  constructor leaves it 1.
    int         m_shown;            // +0x90
    int         m_attached;         // +0x94
    //  m_98/m_9C and m_A0/m_A4 are the cached *screen-space* rect slot 10's
    //  PointInRect tests against: DestroyLayout's real body (slot 0, above)
    //  computes m_98 = m_A8 + newRect->x0, m_9C = m_AC + newRect->y0,
    //  m_A0 = newRect->x1 - m_B0, m_A4 = newRect->y1 - m_B4 - i.e. m_A8/m_AC
    //  and m_B0/m_B4 are this frame's own near/far anchor insets from
    //  whatever rect its layout parent hands it, and m_98..m_A4 is the
    //  result already added in, cached for the hit test to read without
    //  recomputing it every call.
    float       m_screenX0;         // +0x98
    float       m_screenY0;         // +0x9C
    float       m_screenX1;         // +0xA0
    float       m_screenY1;         // +0xA4
    float       m_insetX0;          // +0xA8
    float       m_insetY0;          // +0xAC
    float       m_insetX1;          // +0xB0
    float       m_insetY1;          // +0xB4
    unsigned int m_layerPriority[5];// +0xB8 .. +0xCC
    void*       m_pOwnedFrame;      // +0xCC
    CRegionNodeList m_regions;      // +0xD0 .. +0xDC
    SFrameSlot  m_layerRegions[5];  // +0xDC .. +0x118
    TSList<SIMPLEFRAMENODE, 0> m_children;   // +0x118 .. +0x124
};


//----------------------------------------------------------------------------
//  CSimpleButton - CSimpleButton.cpp, 0x6F6030E0.  Its own fields start at
//  +0x124 and run to +0x168.
//----------------------------------------------------------------------------
class CSimpleTexture;

class CSimpleButton : public CSimpleFrame
{
public:
    CSimpleButton(void* parent);                 // 0x6F6030E0

    //  0x6F602FE0 - the enable/disable this module owns.  It is a
    //  CSimpleButton method and not a CSimpleFrame one because +0x138, the
    //  field it tests, is the button's own.
    void SetEnabled(int enabled);
    //  0x6F603220 - load the art for one of the button's texture slots.
    int SetSlotTexture(int slot, const char* path);
    //  0x6F602F80 - park a loaded texture in one of the button's own slots,
    //  dropping whatever was there and re-parenting the new one onto this
    //  button's own layer.  Frame/simplebuttonslottexture.cpp.
    void AttachSlotTexture(int slot, CSimpleTexture* texture);

    //====================================================================
    //  CSimpleButton's own overrides of the seven CSimpleFrame
    //  base-case-placeholder slots above (2, 20, 21, 23, 24, 27, 28) - see
    //  the vtable block comment on CSimpleFrame for why those seven, and
    //  not the other 22, are declared again here.  Declaration order does
    //  not matter for these: they are *overrides*, so MSVC finds each
    //  one's slot by matching signature against the base, not by where the
    //  override sits in this class.
    //====================================================================
    //  Slot 2 - the deleting destructor: call the real destructor
    //  (0x6F603800, thunked - the big SEH-framed body is another pass's)
    //  and free the block when the caller asked for it (bit 0 of
    //  `deleteFlags`).  Frame/simplebuttonvtable2.cpp.
    virtual void* Destroy2(int deleteFlags);
    //  0x6F603800 - the real (non-deleting) CSimpleButton destructor body;
    //  a whole further module (texture-slot teardown, the observer at
    //  m_124, the base CSimpleFrame destructor) this session did not need
    //  to open to get the deleting-destructor *shape* at slot 2 right.
    void RealDestructorBody();
    //  0x6F6034C0 - reposition/re-show whichever of m_slots[state] the new
    //  visual state should display.  Called by all three of slots 23, 24
    //  and 28 with the state they just set; a big texture/backdrop-geometry
    //  walk (SetSlotTexture's own module) this session left a thunk.
    void UpdateVisualForState(int state);
    //  Slot 20/21 (+0x50/+0x54) - mouse-down / mouse-up: is `msg`'s button
    //  bit one m_140 listens for, and does its point land inside this
    //  button (slot 10, PointInRect)?  Down fires slot 27 unconditionally
    //  on a hit and marks the pressed visual state (slot 28, state 2);
    //  up only runs at all while m_enabled == 2 (i.e. while this button IS
    //  the pressed one) and fires slot 27 only on the matching "up" bit
    //  (msg's raw id shifted left 4), then restores state 1.
    //  Frame/simplebuttonvtable20.cpp / vtable21.cpp.
    virtual int Method_0x50(const void* msg);
    virtual int Method_0x54(const void* msg);
    //  Slot 23 - see NotifyCaptureGained's own declaration on CSimpleFrame:
    //  the acquire-side mirror of NotifyCaptureLost below.  Marks the
    //  pressed-layer priority, tells m_70's slot 26 (NotifyConstructed) and
    //  fires the m_12C observer (with m_130 as its user data) if one is
    //  registered.  Frame/simplebuttonvtable23.cpp.
    virtual void NotifyCaptureGained();
    //  Slot 24 - "you are no longer the captured frame": the exact mirror
    //  of slot 23 (drops the pressed-layer priority back to 0, tells m_70's
    //  slot 25/NotifyHidden instead, fires the m_12C observer with m_134
    //  instead of m_130).  Frame/simplebuttonvtable24.cpp.
    virtual void NotifyCaptureLost();
    //  Slot 27 (+0x6C) - fire a CSimpleButtonClickEvent through the
    //  observer at m_124, if one is registered: `id` (the raw message bit
    //  slot 20/21 read), m_128 and `this` go into the event, dispatched
    //  through the observer's own slot 3 (CObserver::Method_0x0C's shape -
    //  Agent/observer.h).  Frame/simplebuttonvtable27.cpp.
    virtual void Method_0x6Cb(int id);
    //  Slot 28 (+0x70) - the real state setter every other slot here calls
    //  "notify enabled changed" on: `state` (0 disabled, 1 normal, 2
    //  pressed) is stored into m_enabled itself and picks which of the
    //  three m_slots textures is shown, hiding whichever was up before.
    //  Kept under its existing name (buttonenable.cpp already calls it
    //  that way and this session did not want to touch a scored, working
    //  call site to relabel a parameter) - Frame/simplebuttonvtable28.cpp.
    virtual void NotifyEnabledChanged(int a, int b);

    //  m_124/m_128: the click-event observer CSimpleButtonClickEvent (slot
    //  27's own body, Frame/simplebuttonclickevent.h) fires through, and
    //  the extra word it carries alongside `this` and the raw message id.
    //  Not `CObserver*` outright - nothing in this batch confirms
    //  CObserver is the base rather than some sibling with the same slot-3
    //  shape - but it is dispatched exactly the way CObserver::Method_0x0C
    //  (Agent/observer.h) is.
    void* m_124;                    // +0x124
    int   m_128;                    // +0x128
    int   m_12C;                    // +0x12C
    //  m_130/m_134: NotifyCaptureGained/Lost's own per-direction user data
    //  for the *same* m_12C observer - see both slots' own comments.
    int   m_130;                    // +0x130
    int   m_134;                    // +0x134
    //  m_138 is not a bool: slot 21 tests it `== 2` ("currently pressed",
    //  set by slot 28's own body) and slot 20/21's own trailing notify
    //  calls set it to 2 then 1.  0 = disabled, 1 = normal, 2 = pressed.
    int   m_enabled;                // +0x138
    int   m_13C;                    // +0x13C
    int   m_140;                    // +0x140  starts 0x10
    int   m_144;                    // +0x144
    int   m_148;                    // +0x148
    int   m_14C;                    // +0x14C
    float m_150;                    // +0x150
    float m_154;                    // +0x154
    CSimpleTexture* m_slots[3];     // +0x158 .. +0x164

    //  The slot slot 28 (NotifyEnabledChanged) last showed - copied
    //  straight from whichever m_slots[state] entry it picked, so it holds
    //  the same CSimpleTexture* type.
    CSimpleTexture* m_164;          // +0x164
};

//----------------------------------------------------------------------------
//  ??_7CSimpleButtonClickEvent@@6B@ - the class name is the shipped one
//  (`build/*.asm` prints it verbatim as a symbol slot 27's body stamps into
//  a stack-allocated instance).  Nothing in this batch reaches its own
//  constructor or any of its methods - it is built as a bare object
//  literal (four stores: vtable, sourceButton, extraData, buttonId) and
//  handed to the observer by pointer, so only the *shape* slot 27 writes
//  is known, not any real member function.
//----------------------------------------------------------------------------
//: 0x6F603440 stamps this vftable symbol verbatim into every event it
//  builds; a build/*.asm literal for the same reason RCStaticString's own
//  kRCStaticStringVftable is (docs/msvc-vc8-idioms.md, "Two vtable
//  pointers a build must write as literals") - nothing in this closure
//  reaches CSimpleButtonClickEvent's own constructor, so there is no
//  `virtual` to declare that would make MSVC stamp it for us.
const unsigned int kCSimpleButtonClickEventVftable = 0x6F96DC80u;

struct CSimpleButtonClickEvent
{
    void*          m_vtable;      //  +0x00  ??_7CSimpleButtonClickEvent@@6B@
    int            m_eventId;     //  +0x04  always 0 in this batch's one call site
    int            m_extraData;   //  +0x08  CSimpleButton::m_128
    CSimpleButton* m_pButton;     //  +0x0C
    int            m_buttonId;    //  +0x10  the raw message bit slot 20/21 read
};

//----------------------------------------------------------------------------
//  Three more Storm-owned globals, all three written by CSimpleButton's own
//  slot 23/24 (NotifyCaptureGained/Lost) right before dispatching through
//  the observer at m_124 - a fixed "current event context" pair (extra
//  data, source button) plus a shared, address-taken global the two pass as
//  the message itself.  Nothing in this batch's closure reaches whatever
//  reads any of the three back, and unlike slot 27's own
//  CSimpleButtonClickEvent (built fresh on the stack, every field stored),
//  slots 23/24 never write a single field of the third one before taking
//  its address - so its real shape is unconfirmed and it stays untyped.
//----------------------------------------------------------------------------
extern int  g_buttonEventContext;      // dword_6FA9A8C0
extern void* g_buttonEventSource;      // dword_6FA9A8C4
extern int  g_buttonEventTemplate;     // off_6FA9A8B8

//----------------------------------------------------------------------------
//  CSimpleMessageFrame - CSimpleMessageFrame.cpp, 0x6F604820.  A scrolling
//  list of message lines; +0x124 is the live line count and +0x18C the line
//  array, whose stride is 12 bytes and whose +0x08 holds the line's own
//  CSimpleFontString minus 8 (the shipped code adds the 8 back at every use).
//----------------------------------------------------------------------------
class CSimpleMessageFrame : public CSimpleFrame
{
public:
    CSimpleMessageFrame(void* parent);           // 0x6F604820

    //  0x6F603B10 - set the font every line renders with.
    void SetFont(const char* font, float height, int flags);
    //  0x6F603BB0 - set the text capacity every line reserves.
    void SetLineLength(int length);
    //  0x6F603C00 - set every line's justification.
    void SetJustify(int justify);
    //  0x6F603C50 - set every line's shadow colour and offset.
    void SetShadow(int color, const float* offset);

    unsigned int m_lineCount;       // +0x124
    int   m_128;                    // +0x128
    const char* m_pFont;            // +0x12C
    float m_fontHeight;             // +0x130
    int   m_fontFlags;              // +0x134
    int   m_lineLength;             // +0x138  bytes of text each line holds
    int   m_justify;                // +0x13C
    int   m_shadowColor;            // +0x140
    float m_shadowOffset[2];        // +0x144 .. +0x14C
    float m_14C;                    // +0x14C
    float m_150;                    // +0x150
    float m_154;                    // +0x154
    float m_158;                    // +0x158
    float m_15C;                    // +0x15C
    float m_160;                    // +0x160
    float m_164;                    // +0x164
    float m_168;                    // +0x168
    float m_16C;                    // +0x16C
    int   m_170;                    // +0x170  starts 1
    int   m_174;                    // +0x174
    int   m_178;                    // +0x178
    int   m_17C;                    // +0x17C
    int   m_180;                    // +0x180
    int   m_184;                    // +0x184
    int   m_188;                    // +0x188
    char* m_pLines;                 // +0x18C
    int   m_190;                    // +0x190
};

//----------------------------------------------------------------------------
//  CSimpleGrid - 0x6F612CB0.  A two-dimensional bucket array: +0x124 columns
//  by +0x128 rows, the row records at +0x154 with a 16-byte stride.
//----------------------------------------------------------------------------
//  One row of a CSimpleGrid: 16 bytes, the last dword of which is the row's
//  own chunk size.  0x6F612E50 rounds the requested cell count up to a
//  multiple of that before growing the row.
struct SGridRow
{
    //  0x6F612830 - grow the cell array.  Another module's; thunked.
    void SetAlloc(unsigned int alloc);

    unsigned int   m_alloc;         // +0x00
    unsigned int   m_count;         // +0x04
    CSimpleFrame** m_cells;         // +0x08
    unsigned int   m_chunk;         // +0x0C
};

//  The grid's own array of those rows - the three-word Storm shape again,
//  grown out of line.
struct SGridRowArray
{
    //  0x6F612AA0 - free every row's own cell array, then the row array
    //  itself.  Not a real C++ destructor (nothing calls it implicitly) -
    //  CSimpleGrid::~CSimpleGrid calls it explicitly, twice, matching the
    //  shipped stream; see Frame/simplegrid.cpp.
    void Destruct();
    //  0x6F612D70 - set the row count, growing/shrinking the backing array
    //  of rows (via the still-unreconstructed 0x6F612300/0x6F612B20 pair -
    //  this array's own ComputeChunk/SetAlloc, another module's) and
    //  default-constructing/destructing whichever rows the change adds or
    //  drops.
    void SetCount(unsigned int count);

    unsigned int m_alloc;           // +0x00
    unsigned int m_count;           // +0x04
    SGridRow*    m_rows;            // +0x08
    //  +0x0C - the same growth-granularity latch TSGrowableArray's own
    //  m_chunk is (Containers/tsarray.inl).  Read by SetCount above;
    //  CSimpleGrid used to carry this same memory as an unrelated-looking
    //  `m_158` before this batch noticed SGridRowArray is four words, not
    //  three, and that its own +0xC lands exactly on what CSimpleGrid's own
    //  field table had been calling +0x158.
    unsigned int m_chunk;           // +0x0C
};

class CSimpleGrid : public CSimpleFrame
{
public:
    CSimpleGrid(void* parent);                   // 0x6F612CB0
    ~CSimpleGrid();                              // 0x6F612D30

    //  0x6F612E50 - resize to `columns` x `rows`.
    void SetSize(unsigned int rows, unsigned int columns);
    //  0x6F612400 - put `frame` into cell (column, row).
    void SetCellFrame(unsigned int column, unsigned int row,
                      CSimpleFrame* frame, int release);

    unsigned int m_rowCount;        // +0x124
    unsigned int m_columnCount;     // +0x128
    float m_12C;                    // +0x12C
    float m_130;                    // +0x130
    float m_134;                    // +0x134
    float m_138;                    // +0x138
    float m_13C;                    // +0x13C
    float m_140;                    // +0x140
    float m_144;                    // +0x144
    float m_148;                    // +0x148
    SGridRowArray m_rowArray;       // +0x14C .. +0x15C (m_rowArray.m_chunk is
                                    //  what used to be a separate `m_158`)
};

//----------------------------------------------------------------------------
//  CSimpleRegion - the base CSimpleFontString and CSimpleTexture share,
//  [0x68, 0x84).  Its module is ".\\CSimpleRender.cpp", which is what this
//  batch first named it after; the class's own name is the one its
//  constructor stamps at 0x6F60E8F7, `??_7CSimpleRegion@@6B@`, and that is
//  what it is called here.
//----------------------------------------------------------------------------
class CSimpleRegion : public CLayoutFrame
{
public:
    //  0x6F60E8C0 - Frame/simpleregionctor.cpp.
    CSimpleRegion(CSimpleFrame* parent, int layer, int attach);

    //  0x6F60E7C0 / 0x6F60E7E0 - join or leave the parent frame's list for my
    //  layer, once each way.  Frame/simpleregionattach.cpp.
    void AttachToLayer();
    void DetachFromLayer();
    //  0x6F60E800 - move to another parent and/or another layer.
    //  Frame/simpleregionparent.cpp.
    void SetParentLayer(CSimpleFrame* parent, int layer, int attach);
    //  0x6F60E8A0 - mark my parent's cell of the dirty grid, if I am linked
    //  into its layer at all.  Frame/simpleregiondirty.cpp.
    void MarkLayerDirty();

    //  Slot 9 (+0x24) - "the colour changed", no arguments.
    void NotifyColorChanged()
    {
        typedef void (__thiscall *Fn)(void*);
        ((Fn)(*(void***)this)[0x24 / 4])(this);
    }
    //  Slot 4 (+0x10) - "give me the four floats the corners want", and
    //  whether there are any.
    int QueryCornerRect(float* out)
    {
        typedef int (__thiscall *Fn)(void*, float*);
        return ((Fn)(*(void***)this)[0x10 / 4])(this, out);
    }

    //  Five bytes of 0xFF, written by the constructor as one byte store at
    //  +0x68 and one dword store at +0x69 - which is what says they are one
    //  array and not a dword colour plus a separate byte.  SetColor writes
    //  the four at +0x68 in the order a, b, g, r out of a packed ARGB dword.
    unsigned char m_color[5];       // +0x68
    char          m_reserved6D[3];  // +0x6D
    int           m_70;             // +0x70
    CSimpleFrame* m_pParent;        // +0x74
    int           m_layer;          // +0x78
    unsigned int  m_regionFlags;    // +0x7C  bit 0: linked into the layer
    float         m_80;             // +0x80
};

//----------------------------------------------------------------------------
//  CSimpleFontString - 0x6F60D580.  Its own fields start at +0x84.
//----------------------------------------------------------------------------
class CSimpleFontString : public CSimpleRegion
{
public:
    CSimpleFontString(void* parent, int layer, int style);   // 0x6F60D580

    //  0x6F60C9A0 - reserve `length` bytes of text buffer.
    void SetTextLength(int length);
    //  0x6F60CA10 - replace the string.
    void SetText(const char* text);
    //  0x6F60DA20 - pick the font; returns 0 when the font could not be made.
    int  SetFont(const char* font, float height, int flags);
    //  0x6F60DAF0 - horizontal justification, one bit of three.
    void SetJustify(int justify);
    //  0x6F60DC70 - shadow colour and offset in one call.
    //  Both arguments are read as *dwords*, not as floats: the shipped code
    //  moves them through integer registers, which is what says they arrive
    //  as one colour and one packed pair rather than as three floats.
    void SetShadow(const unsigned int* color, const unsigned int* offset);
    //  0x6F60E740 - the text colour, byte-swapped into +0x68.
    void SetColor(const unsigned int* color);
    //  0x6F60D630 - lay the current text out into a new text block, tinting
    //  it afterwards when the style asks.  `rect` null means "use the layout
    //  frame's own +0x44 rect".  Frame/fontstringrebuild.cpp.
    void Rebuild(const float* rect);

    int    m_84;                    // +0x84
    int    m_88;                    // +0x88
    void*  m_pFont;                 // +0x8C  refcounted
    float  m_fontHeight;            // +0x90
    int    m_textLength;            // +0x94
    int    m_textCapacity;          // +0x98
    char*  m_pText;                 // +0x9C
    void*  m_pRendered;             // +0xA0  refcounted
    float  m_A4;                    // +0xA4
    float  m_A8;                    // +0xA8
    unsigned int m_shadowColor;     // +0xAC  starts 0xFF000000
    float  m_shadowX;               // +0xB0
    float  m_shadowY;               // +0xB4
    float  m_B8;                    // +0xB8
    float  m_BC;                    // +0xBC
    int    m_style;                 // +0xC0  starts 0x212
};

//----------------------------------------------------------------------------
//  CSimpleTexture - 0x6F60D900.  Its own fields start at +0x84 too, and its
//  +0x8C/+0x90 pair is a texture and a two-valued mode rather than the font
//  string's font and point size.
//----------------------------------------------------------------------------
class CSimpleTexture : public CSimpleRegion
{
public:
    CSimpleTexture(void* parent, int layer, int style);   // 0x6F60D900

    //  0x6F60D420 - set the four corners' texture coordinates from one
    //  four-float rectangle, rotating them a quarter turn as it goes.
    void SetTexCoords(const float* rect);
    //  0x6F60D4E0 - the same tail, taking all eight coordinate floats at
    //  once instead of a rect.  The copies are integer `mov`s.
    void SetTexCoordArray(const float* coords);
    //  0x6F60E090 - load the texture named by `path`, trying .blp and then
    //  .tga, and fall back to a solid magenta 1x1.
    int  SetTexturePath(const char* path, int flags);
    //  0x6F60CCD0 / 0x6F60CD50 - the two halves of the corner refresh both
    //  coordinate setters end on: map a rect through the current texture
    //  coordinates, then build the quad's four (x, y, z) vertices from it.
    //  Frame/texturemaprect.cpp and Frame/texturevertices.cpp.
    void MapRect(float* rect);
    void SetVertices(const float* rect);

    int    m_84;                    // +0x84
    int    m_88;                    // +0x88
    void*  m_pTexture;              // +0x8C  refcounted
    int    m_textureMode;           // +0x90  2 when the load succeeded
    float  m_vertices[12];          // +0x94 .. +0xC4  four (x, y, z)
    float  m_texCoord[8];           // +0xC4 .. +0xE4  four (u, v)
    int    m_E4;                    // +0xE4
};

//----------------------------------------------------------------------------
//  CScreenFrame - 0x6F60A7B0.  A CFrame with four counted references and the
//  two "screen" flags, constructed with (0, 0, 0) and immediately told to
//  take mode 2.
//----------------------------------------------------------------------------
struct SFrameRef
{
    SFrameRef();                    // 0x6F60A3E0 (the shipped element ctor)
    ~SFrameRef();                   // 0x6F60A430
    void* m_pRef;                   // +0x00
};

class CScreenFrame : public CFrame
{
public:
    CScreenFrame();                              // 0x6F60A7B0

    //  0x6F60A540 - Frame/screenframedtor.cpp.  Releases m_pRenderObject
    //  and m_168 (the same test-decrement-call shape SAgentPtrSlot::Release,
    //  Misc/misc_agentslot_release.cpp, is), then falls through the
    //  compiler-generated array destructor for m_refs[4] (SFrameRef already
    //  has a real destructor, framethunks.cpp) and the base CFrame
    //  destructor.
    ~CScreenFrame();

    //  0x6F60A450 - release all four references in one backwards sweep.
    void ReleaseRefs();
    //  0x6F60A6E0 - hand back the screen-space sprite frame this frame draws
    //  through, creating it the first time `create` says so.
    //  Frame/screenframerender.cpp.
    void* EnsureRenderObject(int create);

    void*     m_168;                // +0x168
    void*     m_pRenderObject;      // +0x16C
    SFrameRef m_refs[4];            // +0x170 .. +0x180
};

//----------------------------------------------------------------------------
//  CSimpleTop - 0x6F60BDF0, the singleton every CSimpleFrame parks in its own
//  +0x68 (dword_6FACE758).  0x8FC bytes - confirmed by the allocation site
//  (0x6F2FF15A, `push 8FCh` immediately ahead of the constructor call), not
//  by the constructor's own body (which never allocates itself).  The
//  earlier "0x8F8" here was one dword short: CSimpleTop::Method_0x30 forms
//  `this+0x8F8` as a real address and passes it on, which only fits with
//  one more field past m_8F4 - see m_8F8 below.
//----------------------------------------------------------------------------
struct SRenderBatch
{
    SRenderBatch();                 // 0x6F60DED0
    ~SRenderBatch();                // 0x6F60DFF0

    //  0x6F60DF80 - drop this batch's own resource reference (if any, via
    //  the thunked ReleaseBatchResource) and clear both count fields.
    //  simpletoprebuilddirtybatches.cpp.
    void Reset();
    //  0x6F60D0E0 - qsort the +0x14 sub-array by whatever the thunked
    //  CompareRenderBatchEntries compares, once it holds more than one
    //  entry.  simpletoprebuilddirtybatches.cpp.
    void SortIfNeeded();

    char         m_reserved00[4];   // +0x00
    //  Method_0x30's own "does this batch have pending work" test
    //  (`[batch+4] != 0`).  Nothing in either closure this session covers
    //  ever writes it - the write is almost certainly inside whatever a
    //  region's own Method_0x3C override does to the batch pointer it is
    //  handed, which this session did not chase further.
    unsigned int m_04;              // +0x04
    //  A releasable resource: Reset() frees it through the thunked
    //  ReleaseBatchResource (0x6F7BA980, a generic pooled-object release -
    //  unlink from one list, push onto the free list at off_6FAACAC4 -
    //  unrelated to this module and not reconstructed here) and zeroes it.
    void*        m_08;              // +0x08
    char         m_reserved0C[4];   // +0x0C
    //  SortIfNeeded's own qsort NumOfElements.
    unsigned int m_10;              // +0x10
    //  SortIfNeeded's own qsort Base - 0x1C-byte entries, the same stride
    //  as this struct itself; whether that is meaningful or coincidental is
    //  unconfirmed.
    void*        m_14;              // +0x14
    char         m_reserved18[4];   // +0x18
};

//----------------------------------------------------------------------------
//  CSimpleTop::m_dirtyBatches (+0x844) - a plain {alloc,count,data} growable
//  array of SRenderBatch* (4-byte elements, confirmed by the `lea eax,
//  ds:0[edi*4]` size computation inside the thunked EnsureCapacity), rebuilt
//  from scratch every call to Method_0x30.  Not storm.h's own
//  TSGrowableArray<T>: that template carries a fourth field (m_chunk) this
//  one does not have room for (Method_0x30's own field arithmetic puts
//  m_shortcutsOn at +0x850, twelve bytes past m_dirtyBatches's own start,
//  leaving no thirteenth-and-fourteenth bytes for a chunk word) - so
//  EnsureCapacity computes its growth chunk inline every call instead of
//  caching one, and takes an inline small-buffer-of-8 fast path
//  (`this + 0x0C`) storm.h's own template does not have either.  Deep,
//  generic container internals; not reconstructed - naked thunk,
//  framethunks.h/.cpp.
//----------------------------------------------------------------------------
struct SBatchPtrArray
{
    void EnsureCapacity(unsigned int count);    // 0x6F60AF30, naked thunk

    unsigned int    m_alloc;    // +0x00
    unsigned int    m_count;    // +0x04
    SRenderBatch**  m_data;     // +0x08
};

//----------------------------------------------------------------------------
//  CSimpleTop::m_8F8 - the receiver Method_0x30's own final loop passes to
//  the thunked ProcessDirtyRenderBatch (`mov ecx, ebx; ...; call
//  sub_6F60D1B0` with `ebx = this + 0x8F8`).  That function's own body
//  never reads through `this` at all (a `__thiscall` with an ignored
//  receiver, not a spelling error here) - modelled as a struct purely so
//  the call can be a real, byte-exact `__thiscall` member call rather than
//  a free function C++ has no calling-convention keyword for; the one
//  `m_unknown` dword is CSimpleTop's own real, confirmed-live (0x6F2FF15A,
//  `push 8FCh`) final field, and nothing in either closure this session
//  covers ever reads its value.
//----------------------------------------------------------------------------
struct SDirtyBatchContext
{
    int  m_unknown;
    void ProcessDirtyRenderBatch(SRenderBatch* batch);   // 0x6F60D1B0, naked thunk
};

class CSimpleTop : public CFrame
{
public:
    CSimpleTop(int a, int b, int c);             // 0x6F60BDF0

    //  0x6F60C2A0 - remember one frame in the all-frames list at +0x174,
    //  once.  Frame/simpletopframes.cpp.
    void AddFrame(CSimpleFrame* frame);
    //  0x6F60BCE0 / 0x6F60BD50 - one frame's membership of one draw layer.
    //  A priority of -1 means "use the frame's own depth".
    //  Frame/simpletoplayers.cpp.
    void AddFrameToLayer(CSimpleFrame* frame, int layer, int priority);
    void RemoveFrameFromLayer(CSimpleFrame* frame, int layer);
    //  0x6F60C3B0 / 0x6F60C760 - `frame`'s own membership of the ten
    //  (layer, region-list) pairs at m_frameLayers - the walk CSimpleFrame's
    //  own NotifyHidden/NotifyConstructed (slots 25/26) run before ever
    //  touching AddFrameToLayer/RemoveFrameFromLayer above.  The remove
    //  half also clears m_pCapturedFrame (and tells `frame`'s own slot 24,
    //  NotifyCaptureLost) when `frame` was the captured one - a *third*
    //  confirmed capture-release path alongside RemoveFrameFromLayer's own.
    //  Both are naked thunks: `this`'s own +0x18C array is
    //  m_frameLayers[10] (confirmed - the stride and the trailing five-dword
    //  clear this pair's own code does line up with SFrameLayer's 0xC-byte
    //  size and m_dirty[50] immediately after it), but SFrameLayer's own
    //  internal shape - which dword is the entry count, which is the list
    //  head - is still opaque, and getting that wrong here would be a
    //  reconstruction in name only.  Frame/simpletopframes.cpp is where
    //  they would land once someone pins that shape down.
    void RemoveFromLayerRegions(CSimpleFrame* frame);
    void AddToLayerRegions(CSimpleFrame* frame, int depth);
    //  0x6F60B800 / 0x6F60B930 - bind or drop the keyboard shortcuts a draw
    //  layer's frames answer to.  Frame/simpletopshortcuts.cpp.
    void RegisterShortcuts(int layer);
    void UnregisterShortcuts(int layer);

    //========================================================================
    //  CSimpleTop's own 11-slot contribution to CFrame's 57-slot vtable
    //  (0x6F96E2FC, docs/targets/vtables/CSimpleTop.txt) - the slots where a
    //  fresh CWorldFrameWar3 pull (0x6F94157C) disagrees with this class,
    //  confirming these are genuinely CSimpleTop's own overrides and not
    //  shared CFrame behaviour.  The other 46 slots are declared on CFrame
    //  itself (frame.h's own CFrame block) because both leaf classes carry
    //  the identical address at each.
    //========================================================================

    //  Slot 1 (+0x04) override of CObserver::DeleteSelf - the deleting
    //  destructor: call the real (non-deleting) destructor, then free the
    //  block when the caller asked for it (bit 0 of `flags`) - the same
    //  "call the real dtor, conditionally free" shape CSimpleButton::Destroy2
    //  already has in the sibling CSimpleFrame hierarchy.  The real
    //  destructor body (0x6F60C0F0, SEH-framed, tears down every list and
    //  array this class owns) is a naked thunk: this session's time went to
    //  the vtable's own 57 slots, not to a whole-class destructor this
    //  batch's own closure never calls.  Frame/simpletopvtable.cpp.
    virtual CObserver* DeleteSelf(int flags);
    //  0x6F60C0F0 - the real destructor body DeleteSelf calls.  Naked thunk.
    void RealDestructorBody();

    //  Slot 3 (+0x0C) override of CObserver::Method_0x0C - forward the
    //  message to my own owner's copy of this same slot, or answer 0 if I
    //  have none.  Frame/simpletopvtable.cpp.
    virtual int Method_0x0C(const SAgentMessage* msg);

    //  Slot 10 (+0x28) - "is anything currently captured": `m_pCapturedFrame
    //  != 0`, ignoring the one (unread) stack argument every sibling slot in
    //  this family also carries.  The single strongest structural lead this
    //  session found toward docs/notes/pause-mouse-capture.md's own
    //  question - see that file's own dated section for the full context -
    //  but it is a *query*, not the write site: nothing in this class's own
    //  57 slots (searched to calltree depth 3) ever stores a non-null value
    //  into m_pCapturedFrame.  Frame/simpletopvtable.cpp.
    virtual int Method_0x28(int unused) const;

    //  Slot 11 (+0x2C) - reconstructed.  The prior session's own guessed
    //  signature (`void* a, float b, float c`) was wrong on two of three:
    //  `c` is dereferenced (`[ecx+0Ch]` and friends inside the thunked
    //  sub_6F4E7010), so it is a pointer, and `a` reaches a per-region
    //  dispatch through `fld`/`fstp`, so it is a float too - the real
    //  signature is `(float a, float b, const SFrameRect* c)`.  Two
    //  operations, each real:
    //    1. drain m_pendingHide (the +0x180 list this class queues frames
    //       onto for teardown): for each queued CSimpleFrame, clear its own
    //       m_shown, dispatch NotifyHidden then Destroy2(1), then free the
    //       whole list back to g_simpleFrameNodeAllocator through the real
    //       DrainPendingHide helper below (0x6F60C0A0);
    //    2. call the thunked SubmitRegionTransform (0x6F4E7010) with `b`,
    //       this frame's own on-screen rect corner (CLayoutFrame::m_rect[0]/
    //       [1], at this+0xF8/+0xFC - *not* a cursor position, contrary to
    //       the prior session's cold-read guess) and `c`; then broadcast
    //       `a` to every region in each of the ten m_frameLayers lists
    //       through that region's own vtable slot 0x34 (CSimpleFrame's own
    //       numbering, but dispatched raw - see the .cpp for why) with
    //       `region->Method_0x34ish(a)`, and finally call the thunked
    //       ProcessLayoutQueue (0x6F605FE0/0x6F605F50 - the same
    //       "dependants queue" machinery slot 27's own comment already
    //       flags as genuinely deep, below-scope container/tree-walk
    //       plumbing) once, unconditionally.
    //  Always returns 1 - the shipped body forms it as `edi+1` off a loop
    //  counter that always reaches 0, not a real condition.
    //  **Checked for docs/notes/pause-mouse-capture.md's own open question,
    //  and it is a clean miss**: neither this function nor anything in its
    //  full depth-3 closure (asm/sub_6F60C4B0_0x6F60C4B0_calltree_asm.md)
    //  references offset +0x16C at all.
    //  Frame/simpletopflush.cpp.
    virtual int Method_0x2C(float a, float b, const SFrameRect* c);


    //  Slot 12 (+0x30) - reconstructed.  Two passes over the 50-entry
    //  m_dirty/m_batches pair, gated by whether the first pass found
    //  anything dirty at all:
    //    1. for every dirty cell i, group = i/5 selects one of the ten
    //       m_frameLayers (five cells per layer - the dirty grid's own
    //       "row" the CSimpleFrame::m_depth comment already names), and
    //       every REGIONNODE in that layer's own list gets
    //       `region->Method_0x3C(i - group*5, &m_batches[i])` (the real
    //       named virtual this time - both arguments are plain `push`es in
    //       the shipped stream, not `fstp`s, so the types genuinely are
    //       `(int, void*)`); m_dirty[i] is left holding whether the *last*
    //       region answered 0 (no accumulation - a literal transcription of
    //       what the shipped stream does, not a bug this build introduces),
    //       and m_batches[i].SortIfNeeded() runs regardless of the answer;
    //    2. only if any cell was dirty: free and zero the m_dirtyBatches
    //       index array, refill it with a pointer to every m_batches[i]
    //       whose own m_04 is non-zero (growing it through the thunked
    //       SBatchPtrArray::EnsureCapacity as needed), then hand each
    //       collected pointer to the thunked ProcessDirtyRenderBatch
    //       (0x6F60D1B0, 888 lines through texture/font Render-module
    //       leaves - qsort, Storm_401/403/405, the render device's own
    //       vtable - genuinely out of this session's scope) together with
    //       `&m_8F8` (a receiver that function's own body never reads).
    //  **Checked the same way slot 11 was, same result**: no reference to
    //  +0x16C anywhere in this function or its full depth-3 closure
    //  (asm/sub_6F60C580_0x6F60C580_calltree_asm.md).
    //  Frame/simpletoprebuilddirtybatches.cpp.
    virtual void Method_0x30();

    //  Slots 14/16 (+0x38/+0x40) - CSimpleTop's own copy of the trivial
    //  "tail-jump my own slot 4" forwarder (frame.h's CFrame block, slot 15
    //  below) - bodily identical to the shared family, just compiled at a
    //  distinct address inside this module.  Frame/simpletopvtable.cpp.
    virtual int Method_0x38(SAgentMessage* msg);
    virtual int Method_0x40(SAgentMessage* msg);

    //  Slot 27 (+0x6C) - releases m_120 (a refcounted reference CFrame owns,
    //  standard AddRef/Release shape), zeroes it, calls a real but
    //  unreconstructed CLayoutFrame-level teardown+requeue helper
    //  (0x6F606290) against this frame's own +0xB4 sub-object, then sets
    //  m_11C = 1.  Naked thunk - 0x6F606290 shares its own tail with
    //  CLayoutFrame::RecomputeQueuedLayout's already-thunked dependants walk
    //  (dword_6FA9A990), the same "real but not worth guessing at" call this
    //  batch made for that method.  Frame/simpletopvtable.cpp.
    virtual void Method_0x6C();

    //  Slots 34/35 (+0x88/+0x8C) - a real "ensure render resources, then
    //  broadcast" pair.  Both start by asking the "system message host"
    //  singleton (g_unk6FACE754/ReadUnk6FACE754, already named) to lazily
    //  build itself a screen-space render object
    //  (CScreenFrame::EnsureRenderObject, already named - the `create` flag
    //  is 1 for slot 34's own call, 0 for slot 35's); 34 additionally
    //  ensures a second, per-mode-indexed resource at that host's own +0x170
    //  array before either one tail-dispatches through *this* frame's own
    //  CLayer::m_field38 reference at vtable slot 0xD4 (34) or 0xD0 (35) -
    //  the two are NOT CSimpleTop::m_pCapturedFrame despite the coincidental
    //  shared +0x16C offset; see the block comment on slot 10 above and
    //  docs/notes/pause-mouse-capture.md's own dated section.
    //  Frame/simpletoprenderobjects.cpp.
    virtual void Method_0x88();
    virtual void Method_0x8C();
    //  Slot 34/35's own remaining internal helpers; naked thunks, see
    //  Frame/simpletoprenderobjects.cpp's own header comment.  The third
    //  (0x6F5FADC0) is real now - CFrame::EnsureAndConfigureScreenSprite.
    int  FindOwnerWithPriority();
    void EnsureHostModeResource(void* arg);
    void EnsureHostModeResourceAlt(void* arg);

    //  Slot 45 (+0xB4) - mark every one of the 50 dirty-grid cells
    //  (m_dirty[50]) dirty in one `rep stosd`.  Frame/simpletopvtable.cpp.
    virtual void MarkAllDirty();

    int   m_168;                    // +0x168
    //  +0x16C is the frame that currently holds the mouse capture:
    //  RemoveFrameFromLayer compares the frame it is dropping against it and
    //  tells slot 24 when they are the same.
    CSimpleFrame* m_pCapturedFrame; // +0x16C
    int   m_170;                    // +0x170
    TSList<SIMPLEFRAMENODE, 0> m_frames;    // +0x174 .. +0x180
    //  The queue Method_0x2C (slot 11) drains every call: every
    //  CSimpleFrame some other closure has queued for teardown, hidden and
    //  Destroy2(1)'d there and then freed here.  Same node type and same
    //  link-offset-0 shape as m_frames above (confirmed by the exact same
    //  Tail()/Prev() clamp idiom at both call sites) - a second, separate
    //  list of the identical node type, not a re-use of m_frames itself.
    TSList<SIMPLEFRAMENODE, 0> m_pendingHide;   // +0x180 .. +0x18C
    SFrameLayer  m_frameLayers[10]; // +0x18C .. +0x204
    int   m_dirty[50];              // +0x204 .. +0x2CC
    SRenderBatch m_batches[50];     // +0x2CC .. +0x844
    //  Method_0x30's own scratch index: every m_batches[i] with pending
    //  work (m_04 != 0), collected fresh each call.  simpletoprebuilddirtybatches.cpp.
    SBatchPtrArray m_dirtyBatches;  // +0x844 .. +0x850
    char  m_reserved850[0x20];      // +0x850
    int   m_shortcutsOn;            // +0x870  starts 1
    //  Four, not five: the constructor's vector iterator takes `push 4`
    //  (0x6F60BED8) and layer 4 is the "all of them" case RegisterShortcuts
    //  tests for ahead of its own jump table, never an array index.
    CFramePriorityArray m_layers[4];    // +0x874 .. +0x8D4
    int   m_8D4;                    // +0x8D4
    int   m_8D8;                    // +0x8D8
    int   m_8DC;                    // +0x8DC
    int   m_8E0;                    // +0x8E0
    int   m_8E4;                    // +0x8E4
    int   m_8E8;                    // +0x8E8
    int   m_8EC;                    // +0x8EC
    int   m_8F0;                    // +0x8F0
    int   m_8F4;                    // +0x8F4
    //  Method_0x30's own last field: only its *address* is ever taken, as
    //  the (never-dereferenced) receiver for the thunked
    //  ProcessDirtyRenderBatch - see SDirtyBatchContext's own comment
    //  above.  This is the one dword the "0x8F8 bytes" note above was
    //  short by.
    SDirtyBatchContext m_8F8;        // +0x8F8
};

//----------------------------------------------------------------------------
//  CSpriteFrame - 0x6F60F030.  A CFrame that owns one sprite and the
//  animation state that drives it.
//----------------------------------------------------------------------------
struct SSpriteSlot
{
    //  0x6F60FC50 - a plain `retn 4` member the CSpriteFrame constructor
    //  calls after its base is up.  It hands `this` back: the shipped body
    //  copies ecx into eax before it writes anything and addresses every
    //  field off eax, which is what MSVC does for a member whose value is
    //  its own receiver.
    SSpriteSlot* Init(int owner);

    int          m_owner;           // +0x00
    int          m_04;              // +0x04
    void*        m_08;              // +0x08
    int          m_0C;              // +0x0C
    unsigned int m_10;              // +0x10  starts 0x40
    int          m_14;              // +0x14
    int          m_18;              // +0x18
};

class CSpriteFrame : public CFrame
{
public:
    CSpriteFrame(int a, int b, int c);           // 0x6F60F030

    //  0x6F60F360 - build the sprite named by `path` and hand it and the two
    //  trailing arguments to the two halves of the set-up.
    void SetSpritePath(const char* path, int a, int b);
    //  0x6F60EAA0 / 0x6F60F1A0 - those two halves.  Another module's; both
    //  thunked.
    void SetSprite(void* sprite, float scale);
    void ConfigureSprite(void* sprite, int a, int b);

    float m_168;                    // +0x168
    float m_16C;                    // +0x16C
    void* m_pSprite;                // +0x170
    int   m_174;                    // +0x174
    int   m_178;                    // +0x178
    int   m_17C;                    // +0x17C
    SSpriteSlot m_slot;             // +0x180 .. +0x19C
    float m_scale;                  // +0x19C
    int   m_1A0;                    // +0x1A0
    int   m_1A4;                    // +0x1A4
    float m_1A8;                    // +0x1A8
    int   m_1AC;                    // +0x1AC  starts 1
};

//  CCursorFrame (0x6F629170) is declared in Frame/cursorframe.h rather than
//  here, and that file says why.

//----------------------------------------------------------------------------
//  RCStaticString - a counted string handle, 12 bytes, whose base is the
//  same 0x6F011300 sub-object SUnitCreateParams embeds at its own +0x2C.
//  0x6F61D5A0 calls that base constructor with a single zero and then stamps
//  `??_7RCStaticString@@6B@` over the vtable pointer the base just wrote,
//  which is the ordinary derived-constructor shape and is what says the
//  sub-object is a *base* at offset 0 and not a member.
//
//  The vtable is written as a literal: nothing in this closure reaches a
//  slot of it, so there is no `virtual` to declare, and declaring one would
//  make this build stamp a vtable of its own (docs/msvc-vc8-idioms.md, "Two
//  vtable pointers a build must write as literals").
//----------------------------------------------------------------------------
//: 0x6F969F5C - ??_7RCStaticString@@6B@ (ida_query names RCStaticString).
const unsigned int kRCStaticStringVftable = 0x6F969F5Cu;

struct RCStaticString : public SUnitCreateParamsSub
{
    RCStaticString() : SUnitCreateParamsSub(0)
    {
        *(void**)this = (void*)kRCStaticStringVftable;
    }
};

//----------------------------------------------------------------------------
//  CBackdropGenerator - ".\\CBackdropGenerator.cpp", 0x50 bytes.
//
//  A nine-piece frame border: one background quad, four edge strips and four
//  corners, all cut out of two skin textures.  The generator holds the two
//  file names and the geometry; Generate() builds the pieces against the
//  frame that owns it, which is what CSimpleFrame::SetOwnedFrame calls it
//  for.
//
//  The name is the module's, not RTTI's: the class has no virtuals of its
//  own, so nothing in the binary carries `CBackdropGenerator` as a symbol -
//  what the shipped code does carry is the ".\\CBackdropGenerator.cpp"
//  string every one of Generate()'s nine allocations is tagged with.
//
//  The four insets are named from the geometry rather than from the one
//  caller (GameUI/gameuiunittip.cpp gives all four the same value, so it
//  cannot tell them apart): Generate() offsets the background's top-left
//  corner by (+m_insetLeft, -m_insetTop) and its top-right by
//  (-m_insetRight, -m_insetTop), which fixes all four.
//----------------------------------------------------------------------------
class CBackdropGenerator
{
public:
    CBackdropGenerator();                        // 0x6F61D5A0

    //  0x6F61CD10 - build every enabled piece against `frame`.
    void Generate(CSimpleFrame* frame);

    CSimpleTexture* m_pBackground;  // +0x00
    CSimpleTexture* m_pEdgeLeft;    // +0x04
    CSimpleTexture* m_pEdgeRight;   // +0x08
    CSimpleTexture* m_pEdgeTop;     // +0x0C
    CSimpleTexture* m_pEdgeBottom;  // +0x10
    RCStaticString  m_background;   // +0x14
    RCStaticString  m_border;       // +0x20
    //  bit 0 left edge, 1 right, 2 top, 3 bottom, 4-7 the four corners.
    unsigned int    m_pieces;       // +0x2C
    int             m_tiled;        // +0x30  the background's load flag
    char            m_reserved34[0x04]; // +0x34  never written by the ctor
    float           m_edgeSize;     // +0x38  starts 0.025f
    float           m_3C;           // +0x3C  zeroed, never read here
    float           m_insetTop;     // +0x40
    float           m_insetBottom;  // +0x44
    float           m_insetLeft;    // +0x48
    float           m_insetRight;   // +0x4C
};


//----------------------------------------------------------------------------
//  The module's globals.  Declared, never defined: build_mix.py binds each
//  one to its real address out of funcmap.DATA, and a definition here would
//  quietly give the .mix a private zero-filled copy of a global the real,
//  unhooked shipped code is still reading and writing.
//----------------------------------------------------------------------------
//  unk_6FACE740 / unk_6FACE9AC / unk_6FACE9C0 / unk_6FACE700 / unk_6FACE714 -
//  the five fixed-block pools this library allocates out of.  Objects, not
//  pointers: every reference to one is `mov ecx, offset`.
extern CDataAllocator g_frameNodePool;
extern CDataAllocator g_framePointPool;
extern CDataAllocator g_framePointRelativePool;
extern CDataAllocator g_simpleFrameNodeAllocator;
extern CDataAllocator g_regionNodeAllocator;
//  dword_6FACE758 - the one CSimpleTop, parked in every CSimpleFrame's +0x68.
extern CSimpleTop* g_pSimpleTop;
//  dword_6FA9A990 - the queue of frames waiting to be laid out again.  Its
//  terminator's `m_next` is what 0x6F605CC0 reads and writes as
//  `off_6FA9A994`; that operand is deliberately left unmapped, because the
//  compiler spells it as this object's own address plus four.
extern TSExplicitList<CLayoutFrame> g_layoutQueue;
//  flt_6F92E790 - 2^-22, the "did this scale really change" epsilon both
//  scale setters compare against (ida_query bytes: 0x34800000).
extern const float g_frameScaleEpsilon;


//----------------------------------------------------------------------------
//  CControl - CButtonFrame's base (0x6F602DE0), and a CSpriteFrame itself.
//  The name is the shipped one: its constructor stamps `??_7CControl@@6B@`
//  at +0 and the `_0` twin at +0xB4.
//----------------------------------------------------------------------------
extern void* const g_vftCControl;             // ??_7CControl@@6B@
extern void* const g_vftCControlSecondary;    // ??_7CControl@@6B@_0

class CControl : public CSpriteFrame
{
public:
    CControl(int a, int b, int c);               // 0x6F602DE0

    //  0x6F601DE0 - or `bits` into the state word at +0x1D4, using the
    //  neighbour at +0x1D8 as a second source, and tail-call slot 60 (+0xF0)
    //  with whatever actually changed.
    void SetStateBits(int bits);

    int m_field1B0;      // +0x1B0
    int m_field1B4;
    int m_field1B8;
    int m_field1BC;
    int m_field1C0;
    int m_field1C4;
    int m_field1C8;
    int m_field1CC;
    int m_field1D0;
    int m_stateBits;     // +0x1D4
    int m_stateMask;     // +0x1D8
    int m_field1DC;
    int m_field1E0;      // +0x1E0 - the only one that starts non-zero
};

class CButtonFrame : public CControl
{
public:
    CButtonFrame(int a, int b, int c);           // 0x6F6220C0
};

//----------------------------------------------------------------------------
//  CTextFrame - ".\\CTextFrame.cpp", the caption behind a text label.
//
//  The base chain is read off RTTI rather than guessed: `??_R2CTextFrame@@8`
//  lists CTextFrame, CControl, CSpriteFrame, CFrame, CLayer, CObserver,
//  TRefCnt and CLayoutFrame - eight classes, with CLayoutFrame's own base
//  descriptor spelling its offset `LE@` (0xB4).  So CTextFrame's own fields
//  start where CControl's end, at +0x1E4.
//
//  Only what this batch touches is named.  The three dirty flags at +0x224,
//  +0x22C and +0x230 and the fourth at +0x23C are set together by SetString
//  and nothing here reads them back.
//----------------------------------------------------------------------------
class CTextFrame : public CControl
{
public:
    //  0x6F611D40 - replace the caption's text.
    void SetString(const char* text);
    //  0x6F615B50 - the editable variant: re-size the buffer to hold `text`
    //  and copy it in, then park the caret.  `retn 8`.  Redirected in
    //  GameUI/gameui_thunks.cpp until its own module lands.
    void SetText(const char* text, int flags);
    //  0x6F611A80 - drop the text buffers.  `freeText` zero keeps the raw
    //  string and drops only the display copy.
    void FreeText(int freeText);
    //  0x6F611630 - are *all* of `mask`'s bits set in the style word at
    //  +0x1F0?  Both call sites ask for 0x100, the "mask the text out" bit.
    int  HasStyleBits(int mask);

    unsigned int  m_textCapacity;   // +0x1E4  bytes m_pText was sized for
    char*         m_pText;          // +0x1E8  the string as it was set
    char*         m_pDisplayText;   // +0x1EC  what is drawn: m_pText, or the
                                    //         run of '*' HasStyleBits picks
    const int*    m_pStyle;         // +0x1F0
    char          m_reserved1F4[0x30];  // +0x1F4
    int           m_224;            // +0x224
    char          m_reserved228[0x04];  // +0x228
    int           m_22C;            // +0x22C
    int           m_230;            // +0x230
    char          m_reserved234[0x08];  // +0x234
    int           m_23C;            // +0x23C
};

//  0x6F611090 - MakeMaskedText, the run of '*' a masked caption draws.  It
//  is a `static` in Frame/textframesetstring.cpp and deliberately has no
//  declaration here: that is what puts its argument in eax, which is where
//  the shipped function reads it.

//----------------------------------------------------------------------------
//  CTextButtonFrame - a CButtonFrame with an owned caption CTextFrame.
//
//  The class name is the shipped one (its constructor, 0x6F61E910, stamps
//  ??_7CTextButtonFrame@@6B@ at +0 and the `_0` twin at +0xB4) and the base
//  is CButtonFrame, called with (a, b, c) forwarded unchanged - the same
//  three-int shape every class in this file's chain passes straight down to
//  CFrame, where `a` is the parent.
//
//  Reconstructed as CGlueTextButtonWar3's own base (docs/targets/
//  CGlueTextButtonWar3.md); see that file for the fuller evidence trail.
//----------------------------------------------------------------------------
class CTextButtonFrame : public CButtonFrame
{
public:
    CTextButtonFrame(int a, int b, int c);        // 0x6F61E910, `retn 0Ch`

    //  +0x1E4 - the caption CTextFrame this button owns, made by its own
    //  constructor from `this` (the new label's parent is the button
    //  itself) and anchored to cover the whole button
    //  (SetAllPoints(this, 1)).  A real reference: addref'd going in,
    //  released going out, the same three-step swap CLAUDE.md's other
    //  targets use for a counted handle.
    CTextFrame* m_pCaption;      // +0x1E4
    //  +0x1E8 .. +0x1F4 - four floats the constructor zeroes and nothing in
    //  this dump reads back; offset-named rather than guessed, the same
    //  rule CScreenFrame's four insets follow above.
    float m_1E8;                 // +0x1E8
    float m_1EC;                 // +0x1EC
    float m_1F0;                 // +0x1F0
    float m_1F4;                 // +0x1F4
};

//----------------------------------------------------------------------------
//  CEventTimer - 0x6F62BBD0, six instructions and its own vtable symbol.
//  TRefCnt-derived: the constructor clears +0x04 before it stamps +0x00,
//  which is the reference count sitting behind a vtable.
//----------------------------------------------------------------------------
class CEventTimer : public TRefCnt
{
public:
    CEventTimer();                               // 0x6F62BBD0

    char m_reserved08[0x0C];        // +0x08
    int  m_14;                      // +0x14
    char m_reserved18[0x04];        // +0x18 - 0x1C bytes in all, the gap the
                                    //         two fade timers leave for it
};

//----------------------------------------------------------------------------
//  The event-handler registry (stru_6FACEA68): eight striped condition
//  variables at +0x20 and the eight buckets they guard at +0x80.  Both
//  strides are twelve bytes, which is what the shipped index arithmetic
//  (`lea eax,[esi+esi*2]` then a scale-4 addressing mode) says.
//----------------------------------------------------------------------------
struct SEventNode
{
    void*             m_00;         // +0x00
    TSLink<SEventNode> m_link;      // +0x04
    void*             m_pKey;       // +0x0C
};

class CEventRegistry
{
public:
    //  0x6F62AF10 - take the stripe `key` hashes to, find its node, and hand
    //  back both.  `*outLock` is the stripe index (plus eight when the lock
    //  was taken exclusively) or -1 when nothing was taken.
    void* AcquireBucket(void* key, int exclusive, int* outLock, int a, int b);

    //  The eight stripes.  Declared as raw bytes rather than as embedded
    //  ConditionVariables because that class has no modelled fields; Lock()
    //  is what puts the shipped `lea` in the stream.
    char m_reserved00[0x20];        // +0x00
    char m_lockStripes[8][12];      // +0x20
    TSList<SEventNode, 4> m_buckets[8];  // +0x80

    ConditionVariable* Lock(unsigned int stripe)
    {
        return (ConditionVariable*)m_lockStripes[stripe];
    }
};

extern CEventRegistry g_eventRegistry;      // stru_6FACEA68

//  unk_6FACEA88 - the same eight stripes as g_eventRegistry.m_lockStripes,
//  reached as a global rather than through the registry.  Two names for one
//  address is deliberate here and is the shipped code's own doing: 0x6F62AF10
//  addresses the stripes off `this` and 0x6F62B170 / 0x6F62B390 address them
//  absolutely, and a single spelling cannot produce both.
extern char g_eventRegistryLocks[8][12];

inline ConditionVariable* EventRegistryLock(unsigned int stripe)
{
    return (ConditionVariable*)g_eventRegistryLocks[stripe];
}

//----------------------------------------------------------------------------
//  One handler binding - ".?AUEVTHANDLER@@", 0x18 bytes - and the per-slot
//  list of them.  The list keeps its link offset in a field the shipped walk
//  reloads on every hop, which is what says TSExplicitList rather than
//  TSList.
//----------------------------------------------------------------------------
struct SEventHandler
{
    TSLink<SEventHandler> m_link;   // +0x00
    void*  m_pHandler;              // +0x08
    int    m_a;                     // +0x0C
    float  m_when;                  // +0x10
    int    m_14;                    // +0x14
};

typedef TSExplicitList<SEventHandler> SEventHandlerList;

class CEventBucketList
{
public:
    //  0x6F631DC0 - bind `handler` into slot `slot`, ordered by `when`.
    //  __fastcall: the shipped call site puts `slot` in edx next to the
    //  `this` in ecx (`mov edx,esi` / `mov ecx,eax` at 0x6F62B41E) and the
    //  body is `retn 0Ch` - three stack arguments, not four.
    void __fastcall InsertHandler(int slot, void* handler, int a, float when);

    char              m_reserved00[0x5C];   // +0x00
    SEventHandlerList m_slots[0x1D];        // +0x5C, twelve bytes each
};

//  0x6F62B170 - "is `key` registered in this thread's event table?"
int __fastcall IsEventKeyRegistered(void* key);
//  0x6F62B390 - bind `handler` to slot `slot` of this thread's table.
void __fastcall RegisterEventHandler(int slot, void* handler, int a,
                                     float when);
//  0x6F6329E0 - drop every binding of `handler`/`a` from `list`.  A free
//  __fastcall and not a member, because the shipped body takes the list in
//  ecx *and* the slot index in edx, which no MSVC member spelling reaches
//  (docs/msvc-vc8-idioms.md, "A receiver in ecx with a second argument in
//  edx is a free `__fastcall`").
void __fastcall RemoveEventHandlers(CEventBucketList* list, int slot,
                                    void* handler, int a, int flags);

#endif
