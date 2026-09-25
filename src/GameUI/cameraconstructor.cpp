//============================================================================
//  0x6F307B00 - CCameraWar3::CCameraWar3().
//  asm/sub_6F307B00_0x6F307B00_calltree_asm.md (now archived) is the dump.
//
//  This is also the constructor whose own first field write -
//  `mov dword ptr [esi], offset ??_7CCameraWar3@@6B@` - is what corrected
//  this class's name from the guessed `CGameCamera` (see gameui.h's own
//  note above `class CCameraWar3`).
//
//  Same `__except_handler4`-shaped frame every other multi-sub-object
//  constructor in this codebase has and cannot reproduce under this build's
//  fixed /GS- /EHs-c- - see docs/msvc-vc8-idioms.md, "An `__except_handler4`-
//  shaped frame this toolchain cannot reproduce", and item.cpp/agent_ctor.cpp
//  for the same call.  The base call (CAgent::CAgent(), sub_6F001730) and
//  this class's own vtable stamp both come for free from `: public CAgent`
//  and from being a properly-shaped derived class - see gameui.h.
//
//  Session update (correctness pass on the five documented gaps below -
//  see docs/targets/CCameraWar3.md's dated entry for the full account):
//
//    1. SEH frame - left as-is; unreproducible under this build's fixed
//       /GS- /EHs-c-, same as every other multi-sub-object constructor.
//    2. `SCheckedUnitSlot(candidate=0)` at +0x44 - now a genuine placement
//       `new` over `m_pPanJob`, the same "real out-of-line constructor over
//       existing storage" technique Unit/unit_ctor.cpp's own m_pRef2C4 uses
//       (agenttypedslots.h).  `m_pPanJob` stays declared `TRefCnt*`
//       (unittouched by any other call site that would need it retyped);
//       the placement `new` just reinterprets its 4 bytes of storage for
//       the one instruction that constructs over them.
//    3. The `sub_6F4B4150` record-builder cluster (four calls into
//       +0x1EC/+0x284/+0x324/+0x3BC, and its own callees
//       `sub_6F302A70`/`sub_6F302A30`/`sub_6F30A860`/`sub_6F3079D0`) -
//       **not reconstructed this pass**.  `sub_6F4B4150` itself (113
//       instructions: a custom hash/pack routine at its one callee,
//       `sub_6F4AB660`, built from four Granlund-Montgomery-style
//       magic-multiply divisions by 59/61/53 and a fourth, not yet
//       identified, combined into a bitfield) is a substantial standalone
//       reconstruction with real correctness risk if guessed at under this
//       session's time budget - left zeroed (ZeroRange below) rather than
//       forced.  `sub_6F30A860` (SetLastCameraWar3Raw) and `sub_6F3079D0`
//       (SFilterSlotArray::SetCount) are NOT part of this cluster - they
//       are real, already-reconstructed leaves the constructor's own tail
//       calls for real; see the fix below.
//    4. The bounds/matrix/weight tail, +0x4CC..+0x594 - **fully
//       reconstructed this pass** using gameui.h's own already-named
//       `m_boundsMin*`/`m_boundsMax*`/`m_toBoundsSpace`/`m_fromBoundsSpace`/
//       `m_sampleWeights` fields (all zero, then a pair of row-major 3x3
//       identity matrices, then a third identity-shaped array for the
//       sample weights - transcribed straight off the dump's own
//       `fld1`/`fldz`/`fxch` sequence), plus the two preference-driven
//       weight slots at +0x4FC/+0x500 (reserved, unread by anything else in
//       either tree) that the dump computes from preference ids 41/39 via
//       two tiny leaf pairs - see cameraweightclamp.cpp.  While reading
//       this range for real, two more real bugs turned up and are fixed
//       here too, both about the tail of the constructor:
//         - `dword_6FAB542C` (g_pLastCameraWar3) was being set to `this`
//           directly.  The shipped code instead calls the already-
//           reconstructed `SetLastCameraWar3Raw` with `[esi+20h]` - which
//           is `m_presetIndex`, zeroed at the top of this same constructor
//           and never written again before this call - so the real net
//           effect is `g_pLastCameraWar3 = 0`, not `this`.  Fixed to call
//           the real function with the real argument.
//         - the +0xA8 `SFilterSlotArray` (now named `m_filterSlots` -
//           gameui.h) was entirely unconstructed: neither its own three
//           zeroed fields nor the trailing `SetCount(8)` call the dump
//           makes were reproduced.  Both added.
//    5. The `??_7Position@@6B@` vtable stamp on `m_target`'s own
//       `CPathRef` sub-object - **not reproduced this pass**; see the
//       comment at m_target below for why (CPathRef has no reconstructed
//       default constructor yet, and this session judged giving it one
//       from here too high-risk for the widely-shared type - see
//       docs/targets/CCameraWar3.md).
//
//  Everything else below is a direct, offset-for-offset translation of the
//  dump's own raw stores.
//============================================================================
#include "gameui.h"
#include "preferences.h"
#include "cameraweightclamp.h"

//  0x6F009F90 - already reconstructed (Config/configint.cpp); forward
//  declared locally the way every other caller outside Config/ does.
int __fastcall GetConfigInt(const char* section, const char* key, int index);

//----------------------------------------------------------------------------
//  Local stand-ins that exist purely to reproduce the vtable *symbols*
//  ??_7Float@@6B@ / ??_7Angle@@6B@ the shipped constructor stamps into each
//  SCameraProp's own m_value (SUnitTrackedRef, unittrackedref.h) - same
//  technique Position/PositionVtableBase (position.h) already uses for
//  ??_7Position@@6B@.  The real Float/Angle classes are unreconstructed and
//  live entirely in Game.dll; only the mangled name has to agree.
//  SUnitTrackedRef itself is left untouched (agent.h/unittrackedref.h - it
//  is a plain, non-virtual struct used elsewhere with a real TRefCnt vtable
//  stamped into it by other code, e.g. ~CUnit), so these two stay local to
//  this file rather than becoming SUnitTrackedRef's own declared type.
//----------------------------------------------------------------------------
//  Deliberately *not* in an anonymous namespace: that would decorate the
//  mangled vtable symbol (`??_7Float@?A0x....@@6B@` instead of the plain
//  `??_7Float@@6B@` the dump has), defeating the whole trick.  Both classes
//  are trivial/inline (COMDAT), so a second, identical declaration of
//  either one elsewhere in this build folds harmlessly at link time rather
//  than colliding.
class Float
{
public:
    virtual void ReleaseSelf() {}
    virtual void OnZeroRefCount() {}
    ~Float() {}
};
class Angle
{
public:
    virtual void ReleaseSelf() {}
    virtual void OnZeroRefCount() {}
    ~Angle() {}
};

//  Stamp `prop`'s own SUnitTrackedRef with `kind`'s vtable and the same
//  refcount(0)/handle(-1)/typeTag(-1) triple every one of the nine
//  properties gets, then null both counted-reference modifier slots - the
//  eh_vector-constructed pair the dump builds with a placement ctor/dtor
//  callback pair this pass does not reproduce (see the file header); zero
//  is its net effect for "no modifier installed yet".
template <class Kind>
static __forceinline void InitProp(SCameraProp& prop)
{
    Kind stamp;
    *(void**)&prop.m_value = *(void**)&stamp;
    prop.m_value.m_refcount = 0;
    prop.m_value.m_handle   = (unsigned int)-1;
    prop.m_value.m_typeTag  = -1;
    prop.m_pModifier  = 0;
    prop.m_pModifier2 = 0;
}

static __forceinline void ZeroRange(void* begin, void* end)
{
    for (unsigned char* p = (unsigned char*)begin; p < (unsigned char*)end; ++p)
        *p = 0;
}

//  dword_6FAB542C - the last-constructed CCameraWar3 (funcmap.py DATA).
extern void* g_pLastCameraWar3;
//  0x6F30A860 - GameUI/cameramiscleaves.cpp.  `retn 0`, `__fastcall`.
extern void __fastcall SetLastCameraWar3Raw(void* value);
//  dbl_6F875AB0 - Config/preferenceapply.cpp's own percent-to-fraction
//  divisor; every percentage preference (including these two) converts
//  through `(float)((unsigned int)value / g_preferencePercentDivisor)`.
extern const double g_preferencePercentDivisor;

CCameraWar3::CCameraWar3()
{
    //  +0x20..+0x2C - four plain words right after CAgent's own 0x20-byte
    //  base subobject.  0x78/1 are the shipped constants.  +0x20 is
    //  m_presetIndex (gameui.h); the other three still have no better name
    //  than their offset - nothing in either tree reads them back.
    m_presetIndex                = 0;
    m_cursorMode                = 0;
    *(int*)((char*)this + 0x28) = 0x78;
    *(int*)((char*)this + 0x2C) = 1;

    m_preferenceField30 = 0;
    m_pController        = 0;
    m_controllerMode      = -1;
    m_pAltController      = 0;
    m_pDefaultController  = 0;
    //  +0x44 - SCheckedUnitSlot(candidate=0), for real: a genuine
    //  out-of-line constructor call over m_pPanJob's own storage
    //  (`lea ecx,[esi+44h]` / `push 0` / `call sub_6F038450` in the dump -
    //  no null-check ahead of it, since `&m_pPanJob` is a constant offset
    //  of `this` and this build's optimiser proves it non-null the same
    //  way it does for Unit/unit_ctor.cpp's own placement-new sites that
    //  *do* still carry the check; the two differ only in whether the
    //  compiler could prove it here, not in the C++ spelling).
    new ((void*)&m_pPanJob) SCheckedUnitSlot(0);

    *(int*)((char*)this + 0x48) = 0;
    *(int*)((char*)this + 0x4C) = 0;
    m_offset.m_x = 0.0f;
    m_offset.m_y = 0.0f;
    m_offset.m_z = 0.0f;
    m_pEventSink = 0;

    //  +0x60..+0x84 - nine dwords, all 1.0f (fld1, restored eight times
    //  without popping before the ninth `fstp`).  Unnamed: nothing in
    //  either tree reads this range back.
    for (int off = 0x60; off < 0x84; off += 4)
        *(float*)((char*)this + off) = 1.0f;

    //  +0x84 - m_updateTimer (CAgentTimer, already a typed member) is
    //  default-constructed automatically ahead of this body, matching the
    //  shipped `call sub_6F0418E0` on `[esi+84h]`.

    //  +0xA8 - m_filterSlots (SFilterSlotArray, gameui.h): alloc/count/data
    //  all zeroed, then +0xB4 separately (a plain int this file does not
    //  otherwise name).  SetCount(8) is called on it at the very end of
    //  this constructor, matching the dump's own late `lea ecx,[esi+0A8h]`
    //  / `call sub_6F3079D0`.
    m_filterSlots.m_alloc = 0;
    m_filterSlots.m_count = 0;
    m_filterSlots.m_data  = 0;
    *(int*)((char*)this + 0xB4) = 0;

    //  +0xCC..+0x1A4 - the nine SCameraProp properties, in the same order
    //  gameui.h already declares them.  Six publish through SetValue
    //  (Float) and three through AssignWrappedValue (Angle) per that
    //  file's own note - plus m_listenerAngle and m_roll, which this
    //  constructor's own vtable stamps show are Angle too (an update to
    //  that note - see gameui.h).
    InitProp<Float>(m_listenerDistance);
    InitProp<Angle>(m_listenerAngle);
    InitProp<Float>(m_targetDistance);
    InitProp<Float>(m_farZ);
    InitProp<Float>(m_nearZ);
    InitProp<Angle>(m_fieldOfView);
    InitProp<Angle>(m_rotation);
    InitProp<Angle>(m_angleOfAttack);
    InitProp<Angle>(m_roll);

    //  +0x1A4..+0x1B4 - m_target (CPathRef).  The shipped constructor
    //  stamps ??_7Position@@6B@ here too - CPathRef is part of the same
    //  Position-derived family SCameraProp's own m_value is (gameui.h's own
    //  note on the FloatProp/Position layer) - but CPathRef has no
    //  reconstructed default constructor of its own yet, so only the other
    //  three words of the four-word block are reproduced here; the vtable
    //  word is left at whatever CPathRef's own (unwritten) constructor
    //  would put there.  Tracked in the BEHAVIOUR entry, not silently
    //  dropped - see the file header's gap 5.
    *(int*)((char*)&m_target + 0x04) = 0;
    *(int*)((char*)&m_target + 0x08) = -1;
    *(int*)((char*)&m_target + 0x0C) = -1;

    InitProp<Float>(m_targetZ);
    m_panActive = 0;
    InitProp<Float>(m_panZ);

    //  +0x1EC..+0x454 - four `sub_6F4B4150` record-builder calls (see the
    //  file header's gap 3).  Zeroed rather than left uninitialised.
    ZeroRange((char*)this + 0x1EC, (char*)this + 0x454);

    m_pRampModifiers[0]   = 0;
    m_pRampModifiers[1]   = 0;
    m_pOffsetModifiers[0] = 0;
    m_pOffsetModifiers[1] = 0;
    m_pPanMovement        = 0;

    //  +0x468..+0x478 - another Position-shaped four-word block (see
    //  m_target above for why the vtable word is not reproduced here).
    *(int*)((char*)this + 0x46C) = 0;
    *(int*)((char*)this + 0x470) = -1;
    *(int*)((char*)this + 0x474) = -1;

    //  +0x478..+0x4AC - twelve more zeroed floats (fldz, restored without
    //  popping), part of the same record area as +0x1EC above (gap 3).
    //  +0x484 is inside this run in the dump too, but gets overwritten for
    //  real near the end of this constructor (GetConfigInt) - zeroing it
    //  here first is a harmless extra store, not a behavioural gap.
    ZeroRange((char*)this + 0x478, (char*)this + 0x4AC);

    InitProp<Angle>(m_localAngle);

    //  +0x4CC..+0x594 - the clamp bounds, the two 3x3 change-of-basis
    //  matrices and the nine terrain-sample weights ProjectGroundPoint
    //  reads (gameui.h), reconstructed for real off the dump's own
    //  `fld1`/`fldz`/`fxch` sequence (gap 4).  All four bounds are exactly
    //  zero; both matrices and the sample-weight array are the identical
    //  row-major {1,0,0, 0,1,0, 0,0,1} pattern.
    m_boundsMinY = 0.0f;
    m_boundsMinX = 0.0f;
    m_boundsMaxY = 0.0f;
    m_boundsMaxX = 0.0f;

    //  +0x4DC..+0x528 - reserved (nothing in either tree reads this range
    //  back), mostly zero except the two preference-driven weight slots at
    //  +0x4FC/+0x500 - both start at the same literal 1.0f the dump stores
    //  here, then get a real, conditional overwrite below via the
    //  preference reads (cameraweightclamp.h).
    ZeroRange((char*)this + 0x4DC, (char*)this + 0x4FC);
    *(float*)((char*)this + 0x4FC) = 1.0f;
    *(float*)((char*)this + 0x500) = 1.0f;
    *(float*)((char*)this + 0x504) = 1.0f;
    ZeroRange((char*)this + 0x508, (char*)this + 0x514);
    *(float*)((char*)this + 0x514) = 1.0f;
    ZeroRange((char*)this + 0x518, (char*)this + 0x524);
    *(float*)((char*)this + 0x524) = 1.0f;

    static const float kIdentity3x3[9] = { 1.0f, 0.0f, 0.0f,
                                            0.0f, 1.0f, 0.0f,
                                            0.0f, 0.0f, 1.0f };
    for (int i = 0; i < 9; ++i)
    {
        m_toBoundsSpace[i]   = kIdentity3x3[i];
        m_fromBoundsSpace[i] = kIdentity3x3[i];
        m_sampleWeights[i]   = kIdentity3x3[i];
    }

    //  +0x594..+0x5BC - reserved; all zero (three floats immediately after
    //  the sample weights, then the rest of the gap up to m_smoothingFactor).
    ZeroRange((char*)this + 0x594, (char*)this + 0x5BC);

    //  +0x30/+0x4FC/+0x500 - the three preference reads the dump makes back
    //  to back off one `GetPreferences()` call, ids 41/39/40 in that order.
    //  Ids 41 and 39 are percent-shaped (0..100, converted to a 0..1
    //  fraction and skipped entirely if the raw value is out of that
    //  unsigned range) and feed the two reserved weight slots above through
    //  SetCameraPreferenceFractionSecondary/Primary (cameraweightclamp.h) -
    //  the same two functions CPreferencesWar3::ApplyPreference
    //  (Config/preferenceapply.cpp) already calls live for the same two
    //  ids, so this is a genuine shared call, not a parallel copy; id 40 is
    //  stored straight through with no conversion into m_preferenceField30.
    CPreferencesWar3* prefs = GetPreferences();

    int pref41 = 0;
    prefs->GetIntValue(0x29, &pref41);
    if ((unsigned int)pref41 <= 100)
        SetCameraPreferenceFractionSecondary(
            this, (float)((unsigned int)pref41 / g_preferencePercentDivisor));

    int pref39 = 0;
    prefs->GetIntValue(0x27, &pref39);
    if ((unsigned int)pref39 <= 100)
        SetCameraPreferenceFractionPrimary(
            this, (float)((unsigned int)pref39 / g_preferencePercentDivisor));

    int pref40 = 0;
    prefs->GetIntValue(0x28, &pref40);
    m_preferenceField30 = pref40;

    //  dword_6FAB542C - SetLastCameraWar3Raw(m_presetIndex).  The shipped
    //  code calls the setter with `[esi+20h]` (m_presetIndex, zeroed at the
    //  very top of this constructor and never written again before this
    //  point), not with `this` - see the file header's gap 4 note.
    SetLastCameraWar3Raw(*(void**)((char*)this + 0x20));

    //  +0xA8 - m_filterSlots.SetCount(8), the dump's own trailing
    //  `mov ecx,ebx(=esi+0A8h)` / `push 8` / `call sub_6F3079D0`.
    m_filterSlots.SetCount(8);

    //  +0x484 - GetConfigInt("HotKeys", "UnitCenter", 0).
    *(int*)((char*)this + 0x484) = GetConfigInt("HotKeys", "UnitCenter", 0);

    //  +0x5BC - a config-driven constant float, copied straight through.
    *(float*)((char*)this + 0x5BC) = *(float*)0x6F8761B8;
}
