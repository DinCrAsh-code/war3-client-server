//============================================================================
//  MovementModifier - the sixteen-byte record ".\Position.cpp" makes when
//  something asks a position to travel to a new point over a span of time.
//
//  It is the positional twin of FloatListener (floatboundarylistener.cpp)
//  and of FloatModifier (floatpropmodifier.h): the same four words, the
//  same Storm allocation with the module's own __FILE__/__LINE__, the same
//  "made object, then two of its own slots" construction, and the same
//  one-word counted reference handed back through MSVC's hidden return
//  buffer.  The three of them are one design used three times, not three
//  coincidences, which is why this header is spelled the way that one is.
//
//  The vtable is modelled as a *data symbol* rather than as a C++ class
//  with virtuals, for exactly the reason floatboundarylistener.cpp records:
//  declaring the slots would make this build emit its own short vtable and
//  then fail to link every body in it.  `mov [reg], SYM` either way, which
//  is all the instruction score sees; tools/abi_audit.py's VTABLE SYMBOL
//  bucket is what checks the name, and vtable_addresses.json is what binds
//  it.
//============================================================================
#ifndef POSITIONMODIFIER_H
#define POSITIONMODIFIER_H

#include "game.h"

//  ??_7MovementModifier@@6B@ - the shipped vtable, by address.
extern void* const g_vftMovementModifier;

//----------------------------------------------------------------------------
//  What a position's slot 5 hands back, and what the modifier is bound
//  against: a second handle-bearing record whose {handle, typeTag} pair
//  sits at +0x0C/+0x10 rather than at +0x08/+0x0C the way CPathRef's own
//  does (game.h).  Nothing in this call tree reads anything else of it, so
//  nothing else is named; it is called the *subject* because that is what
//  every caller does with it - resolve it and hand the result to the
//  modifier's own slot 6.
//----------------------------------------------------------------------------
struct SModifierSubject
{
    //  0x6F471A30 - tell the subject that a made object is now listening to
    //  it: which one (its handle), under which message id, on whose behalf.
    //  One call site, FloatListener::Init (src/Unit/floatlistenerinit.cpp),
    //  and it is named for what that site does with it.
    //
    //  The shipped body is four instructions and never reaches a `ret`:
    //  it loads the vtable of the sub-object at +0x14, takes its slot 2,
    //  rebases `this` by +0x14 and jumps - an adjustor forward, so the
    //  three stack words travel through untouched and the real callee is
    //  the one that cleans them.  Kept as that literal transcription in
    //  src/Misc/misc_handle_lookups.cpp because nothing in this repo
    //  resolves the +0x14 sub-object's own vtable.
    void AddValueListener(unsigned int listenerHandle, unsigned int msgId,
                          void* target);

    char         m_reserved00[0x0C];
    unsigned int m_handle;    // +0x0C
    int          m_typeTag;   // +0x10
};

//----------------------------------------------------------------------------
//  What the maker hands back: only the {handle, typeTag} pair at
//  +0x14/+0x18 is read, and it is read to be copied straight onto the
//  modifier itself, so the modifier and the made object end up naming the
//  same thing.
//----------------------------------------------------------------------------
struct SMadeModifier
{
    char         m_reserved00[0x14];
    unsigned int m_handle;    // +0x14
    int          m_typeTag;   // +0x18
};

//----------------------------------------------------------------------------
//  The 0x2C-byte request block the bind step builds on its stack.  It is
//  the same *shape* as SWidgetAgentQuery (widgetagentquery.h) - a
//  four-character tag, a type id, and a tail of -1 sentinels, submitted
//  through the same 0x6F48AB80 - but not the same block: where the widget
//  version parks a type object and a pool at +0x0C/+0x10, this one parks
//  encoded CFloats read out of g_CFloatZero, so it gets its own struct
//  rather than a cast that would silently mistype three fields.
//
//  The tag is 0x5E6D6F64, '^mod' read most significant byte first, in the
//  same hand-written four-character family as 'lga+' and the agile type
//  ids (agiletype.h).
//----------------------------------------------------------------------------
struct SModifierRequest
{
    unsigned int m_tag;          // +0x00 - '^mod'
    unsigned int m_typeId;       // +0x04
    unsigned int m_reserved08;   // +0x08 - 0
    CFloat       m_from;         // +0x0C
    CFloat       m_to;           // +0x10
    CFloat       m_rate;         // +0x14 - 0 in the FloatProp copy
    unsigned int m_reserved18;   // +0x18 - 0
    unsigned int m_reserved1C;   // +0x1C - 0
    unsigned int m_reserved20;   // +0x20 - 0
    int          m_mode;         // +0x24 - -1, or -2 for a synced subject
    int          m_reserved28;   // +0x28 - -1
};

//  0x6F48AB80 - widgetagentsubmit.cpp.  Re-declared against this module's
//  own block type would mangle to a second symbol, so the one declaration
//  in widgetagentquery.h is used and the block is cast at the call.
struct SWidgetAgentQuery;
void* __fastcall SubmitWidgetAgentQuery(SWidgetAgentQuery* query,
                                        int wantPrepare, int wantFinish);

//----------------------------------------------------------------------------
//  The modifier itself.  Four words, and the maker writes three of them
//  plus the vtable - which is why sixteen bytes is the allocation size and
//  not a guess.
//----------------------------------------------------------------------------
struct MovementModifier
{
    //  Placement-new'd over Storm's block; see floatboundarylistener.cpp
    //  for why that spelling and not an `if (p)` on a named pointer.
    MovementModifier()
    {
        m_refcount = 0;
        m_handle = (unsigned int)-1;
        m_typeTag = -1;
        m_vtable = &g_vftMovementModifier;
    }

    //  0x6F47C6A0 - build the request block, submit it, and adopt the
    //  handle pair of whatever came back.  `retn 8`.
    void* Bind(SModifierSubject* subject, int mode);
    //  0x6F47C770 - publish the travel rate onto the resolved object.
    void SetRate(const CFloat* rate);
    //  0x6F47C790 - publish the destination point, both axes flushed to
    //  zero under the module's own threshold.
    void SetTarget(const CFloat* x, const CFloat* y);
    //  0x6F47C7F0 - the whole of the above in shipped order.  `retn 14h`.
    void Init(SModifierSubject* subject, CPathRef* source,
              const CFloat* x, const CFloat* y, const CFloat* rate);

    const void*  m_vtable;    // +0x00
    int          m_refcount;  // +0x04
    unsigned int m_handle;    // +0x08
    int          m_typeTag;   // +0x0C
};

//  The counted reference MakeMoveModifier returns by value.  It really is
//  counted: the caller that holds one as a temporary (CCameraWar3::PanTo)
//  drops it on scope exit, with the `-= 1` / re-read / `== 0` spelling
//  agenttypedslots.h documents for the same shape.
struct SMovementModifierRef
{
    ~SMovementModifierRef()
    {
        MovementModifier* held = m_ptr;
        if (held != 0)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                ((void (__thiscall *)(void*))(*(void***)held)[0])(held);
        }
    }

    MovementModifier* m_ptr;
};

//----------------------------------------------------------------------------
//  What a modifier's own handle pair resolves to: the object the rate and
//  the destination are actually published on.  Both are *members* and not
//  __fastcall free functions taking the receiver, because the shipped call
//  sites leave only that receiver in ecx and push everything else - a
//  __fastcall would take the first of them in edx and clean four or eight
//  bytes too few, which is exactly the `ret <n>` mismatch
//  tools/thunk_abi_audit.py exists to catch (and did catch, on this file's
//  first draft).
//----------------------------------------------------------------------------
//  What the sink publishes a movement through.  Only the one member the
//  publish path reaches is named; nothing else in this tree touches it.
//  What the sink publishes a movement through is a CPathTrace (game.h): the
//  one thing 0x6F4A73B0 does with the receiver it is handed is call
//  CPathTrace::RecomputeOrigin on it.  That address is
//  CPathTrace::PublishVelocityDelta now (Pathfinding/pathtracepublishvelocity.cpp)
//  and no longer a member of this struct, which stays only as the type the
//  sink's own +0x40 accessor is spelled to return.
struct SPositionPublisher;

struct SPositionSink
{
    //  0x6F4A8450 - `retn 0Ch`.
    void SetRate(const CFloat* rate, int a, int b);
    //  0x6F4A7CB0 - Widget/positionsinksettarget.cpp.  `retn 4`.
    void SetTarget(const CGridVec2* point);

    char      m_reserved00[0x50];
    //  +0x50 - the point the sink last published, i.e. what a new target is
    //  measured against.  SetTarget is the only thing in this tree that
    //  reads or writes it.
    CGridVec2 m_target;         // +0x50 .. +0x58
};

//  The default the module substitutes for a zero rate / a zero span: a
//  *pointer* global (`off_6FA83A88`), not a value, and nothing in this
//  call tree says what it points at beyond "the same thing the caller's
//  own CFloat pointer would have been".
extern const CFloat* const g_pDefaultModifierRamp;

//  flt_6FAAE614 - the "close enough to zero" threshold the FPU compares
//  directly.  Already in funcmap.DATA as g_CFloatEqualityTolerance; declared
//  here with that name and that type, because a second declaration with a
//  different one is a different mangled name and a link failure verify.py
//  cannot see.
extern const float g_CFloatEqualityTolerance;

#endif
