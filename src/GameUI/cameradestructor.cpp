//============================================================================
//  0x6F308340 - CCameraWar3::~CCameraWar3().  See gameui.h.
//
//  Its own translation unit for the same reason agent_deleteself.cpp's own
//  header gives for CAgent: DeleteSelf (GameUI/cameradeleteself.cpp, vtable
//  slot 1) reaches this destructor with a real, non-virtual `call
//  sub_6F308340` in the shipped code (`this->CCameraWar3::~CCameraWar3()`),
//  and compiling it beside that caller would only invite MSVC to inline the
//  whole release chain into it instead - which the shipped stream, with a
//  real `call` at that site, says it does not do.
//
//  A plain member teardown, reverse declaration order (the shape a
//  compiler-generated destructor visiting members in reverse always has),
//  touching in order: +0x4C0 (m_localAngle's own second modifier slot -
//  the only one of the ten property fields whose *own* +0x14 this call tree
//  ever writes anywhere, per gameui.h's note on CancelModifiers), then the
//  +0x468 Position-shaped block and +0x464 (m_pPanMovement), the two
//  count-2 modifier-pointer arrays (m_pOffsetModifiers/m_pRampModifiers),
//  m_panZ and m_targetZ (each releases only its own +0x10 modifier - the
//  dump never touches either one's +0x14, unlike the nine properties
//  below), m_target's own vtable word, then the nine SCameraProp
//  properties from m_roll down to m_listenerDistance (each: release both
//  modifier pointers, then reset the property's own SUnitTrackedRef vtable
//  to plain TRefCnt's - constructor.cpp's own InitProp<Float/Angle> is what
//  stamped it away from that in the first place), the two `TSGrowableArray`
//  members this destructor's own field inventory establishes at +0xA8/+0xBC
//  (gameui.h), m_updateTimer (CAgentTimer), and finally m_pPanJob.  Past
//  this body, CAgent::~CAgent() (0x6F0017F0, agent_dtor.cpp) runs for free
//  from `: public CAgent`.
//
//  **Unreproducible SEH frame, not just a score gap.**  The shipped function
//  carries a genuine __CxxFrameHandler3 frame - the cookie-XORed-into-ESP,
//  single-scope-table-pointer shape docs/msvc-vc8-idioms.md documents under
//  "An `__except_handler4`-shaped frame this toolchain cannot reproduce" -
//  because every one of the count-2 modifier-pointer releases above is
//  reached in the shipped build through a real `eh vector destructor
//  iterator' call, which needs unwind protection in case a release's own
//  virtual `ReleaseSelf` call throws.  This toolchain's fixed `/GS- /EHs-c-`
//  has no exception model for that helper to protect at all, so - per the
//  same doc's "`/EHs-c-` turns an array-member's ... iterator call into a
//  plain loop" - nothing this file can spell reaches either the frame or
//  the iterator calls themselves; both are written here as their plain,
//  unprotected equivalent (a same-effect helper call per element) instead.
//  Marked DIFFERS for exactly these two reasons, not IDENTICAL - see
//  pipeline/CLAUDE.md's "The behaviour verdict": a wrong belief that the
//  gap is register/scheduling noise is not something to assert here, and
//  it plainly is not - the SEH furniture and the iterator-vs-loop shape are
//  real, understood, structural differences the diff shows directly.
//
//  Two new sub-objects this session's own field inventory establishes,
//  neither previously named anywhere in this class (see gameui.h for the
//  full reasoning): +0xA8 a `TSGrowableArray<NTempest::C3Vector>` (only its
//  `m_data` word is touched here, inlined rather than through a real
//  member-destructor call - the loop a trivial-`T` instantiation's own
//  destructor would have collapses away, same as agentptrarray.cpp's own
//  note on the pointer/int instantiations), and +0xBC a real
//  `TSGrowableArray<CAgentPtr>` destructed through the already-reconstructed
//  0x6F286590 (Agent/agentptrarray.cpp).
//============================================================================
#include "gameui.h"
#include "refcnt.h"
#include "storm.h"

//  The compiler's own "drop one reference, self-destruct once the count
//  hits zero" shape, inlined identically at every field below: `test`,
//  `add [obj+4], -1`, and - only on the transition to zero - `call [[obj]]`,
//  i.e. slot 0 (TRefCnt::ReleaseSelf) reached through the raw vtable
//  pointer.  The same shape Player/playerwar3_dtor.cpp's own ReleaseRef and
//  GameUI/escmenudtor.cpp's own ReleaseRefSlot already establish for this
//  family of destructor.
static void ReleaseRef(TRefCnt* p)
{
    if (p != 0 && --p->m_refcount == 0)
        p->ReleaseSelf();
}

//  One SCameraProp's own teardown: release both modifier-pointer slots (the
//  `eh vector destructor iterator' pair the dump reaches through
//  RefHolder_6F029560::Release, Misc/trivialaccessors_08.cpp - see this
//  file's own header for why the iterator call itself does not reproduce),
//  then reset the property's own SUnitTrackedRef vtable word to plain
//  TRefCnt's.
static __forceinline void TeardownProp(SCameraProp& prop, void* trefCntVtbl)
{
    ReleaseRef(prop.m_pModifier);
    ReleaseRef(prop.m_pModifier2);
    *(void**)&prop.m_value = trefCntVtbl;
}

//  ".?AVC3Vector@NTempest@@" - the RTTI-descriptor-shaped tag the shipped
//  free of the +0xA8 array's own `m_data` carries (IDA's `aAvc3vectorNtem`
//  label), the same one Containers/vectorfree_c3vector.cpp's own
//  `~TSGrowableArray<NTempest::C3Vector>` instantiation (0x6F352F30) uses.
static const char kC3VectorTag[] = ".?AVC3Vector@NTempest@@";

CCameraWar3::~CCameraWar3()
{
    //  +0x4C0 - m_localAngle's own second modifier slot.
    ReleaseRef(m_localAngle.m_pModifier2);

    //  One TRefCnt-base vtable stamp, computed once and reused for every
    //  vtable reset below - the same trick CCameraSetup::~CCameraSetup
    //  (camerasetupdtor.cpp) already uses to get ??_7TRefCnt@@6B@ without a
    //  named global.
    TRefCnt stamp;
    void* trefCntVtbl = *(void**)&stamp;

    *(void**)&m_localAngle.m_value    = trefCntVtbl;   // +0x4AC
    *(void**)((char*)this + 0x468)    = trefCntVtbl;   // +0x468 - Position-shaped
                                                        //  block (Method_0x24's own
                                                        //  vtable-slot-3 receiver;
                                                        //  cameraunregisterproperties.cpp)

    ReleaseRef(m_pPanMovement);                        // +0x464

    //  +0x45C..+0x460 / +0x454..+0x458 - the two arrays of two modifier
    //  references CancelModifiers (camerapan.cpp/cameracancelmodifiers.cpp)
    //  also walks, released in the shipped order: offset modifiers first,
    //  then ramp modifiers.
    ReleaseRef(m_pOffsetModifiers[0]);
    ReleaseRef(m_pOffsetModifiers[1]);
    ReleaseRef(m_pRampModifiers[0]);
    ReleaseRef(m_pRampModifiers[1]);

    //  m_panZ / m_targetZ - only the +0x10 modifier is ever released for
    //  these two; unlike the nine SCameraProp properties below, this call
    //  tree never touches either one's own +0x14.
    ReleaseRef(m_panZ.m_pModifier);                    // +0x1E0
    *(void**)&m_panZ.m_value = trefCntVtbl;            // +0x1D0

    ReleaseRef(m_targetZ.m_pModifier);                 // +0x1C4
    *(void**)&m_targetZ.m_value = trefCntVtbl;         // +0x1B4

    *(void**)&m_target = trefCntVtbl;                  // +0x1A4

    //  The nine properties, m_roll down to m_listenerDistance - reverse
    //  declaration order, each a release of both modifier pointers followed
    //  by its own SUnitTrackedRef vtable reset.
    TeardownProp(m_roll,             trefCntVtbl);
    TeardownProp(m_angleOfAttack,    trefCntVtbl);
    TeardownProp(m_rotation,         trefCntVtbl);
    TeardownProp(m_fieldOfView,      trefCntVtbl);
    TeardownProp(m_nearZ,            trefCntVtbl);
    TeardownProp(m_farZ,             trefCntVtbl);
    TeardownProp(m_targetDistance,   trefCntVtbl);
    TeardownProp(m_listenerAngle,    trefCntVtbl);
    TeardownProp(m_listenerDistance, trefCntVtbl);

    //  +0xBC - TSGrowableArray<CAgentPtr>, already reconstructed
    //  (Agent/agentptrarray.cpp).  Cast rather than a typed member for the
    //  double-destruction reason gameui.h's own m_agentListStorage note
    //  gives.
    ((TSGrowableArray<CAgentPtr>*)m_agentListStorage)->~TSGrowableArray();

    //  +0xA8 - TSGrowableArray<NTempest::C3Vector>.  A trivial-T
    //  instantiation's own element loop optimises away entirely (same
    //  reasoning as Agent/agentptrarray.cpp's pointer/int instantiations),
    //  leaving just the +0x08 `m_data` word's own free - so it is written
    //  here as that free directly rather than as a call through a typed
    //  member (see gameui.h's own m_pointListStorage note).
    void* pointData = *(void**)((char*)this + 0xB0);
    if (pointData)
        SMemFree(pointData, kC3VectorTag, -2, 0);

    m_updateTimer.~CAgentTimer();                      // +0x84

    ReleaseRef(m_pPanJob);                             // +0x44

    //  CAgent::~CAgent() (0x6F0017F0, agent_dtor.cpp) runs from here for
    //  free, exactly as CCameraSetup::~CCameraSetup's own tail documents.
}
