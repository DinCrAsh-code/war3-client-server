//============================================================================
//  0x6F052320-0x6F0526A0 - CAbility's own contiguous run of default
//  answers (ability.h; the first four, 0x6F0522E0-0x6F052310, are inline
//  in the header as the virtuals this session's earlier pass placed them
//  at).  Every one of them is two to four instructions of constant, and
//  none of them reads `this`.  A later session (worklist slots 81-91,
//  95-104, 110, 118, 119) added the second half of the same table -
//  DefaultIntFieldQueryA..S/DefaultFloatFieldQueryA..D/DefaultFieldTypeTag
//  below - to this file rather than a new one for the same reason: same
//  shipped module, same address neighbourhood, nothing here calls
//  anything else in it either.
//
//  Also: three real overrides of already-virtual CAgent slots (7, 9, 10,
//  22, 23 - GetAgileTypeId/Method_0x24/GetRecordVersion/GetClassName/
//  Method_0x5C) and three more of the same non-virtual default-answer
//  family (36, 38, 40) plus the +0x48 increment/decrement pair
//  (0x6F03FFA0/0x6F03FFD0) and three of the slot-78..104 accessor run
//  (78/79/80).  See ability.h for each declaration.
//
//  Its own translation unit rather than an addition to ability.cpp: nothing
//  here calls anything, but keeping CAbility::CAbility - whose score is
//  sensitive to exactly what the optimiser can see in the same TU - away
//  from these new bodies costs nothing and removes the question.
//============================================================================
#include "ability.h"
#include "CFloat.h"
#include "agenttimer.h"
#include "storm.h"   // placement new

//  dword_6FAAE470 is g_CFloatZero (CFloat.h) - the encoded software-float
//  zero the simulation module owns.  Declared there with its real type
//  rather than re-declared as a bare void* here: MSVC mangles a data
//  symbol's type into its name, so a second spelling would be a second
//  symbol the linker never resolves (docs/notes/verifier-gate-link-
//  failures.md).

int CAbility::Method_6F052320()          { return 0; }
int CAbility::Method_6F052340()          { return 0; }
int CAbility::Method_6F052350(int)       { return 0; }
int CAbility::Method_6F052360()          { return 0x33; }
int CAbility::Method_6F052370(int)       { return 0; }
int CAbility::Method_6F052380()          { return 0x43; }
int CAbility::Method_6F052390(int)       { return 0; }
int CAbility::Method_6F0523A0()          { return 0x53; }
int CAbility::Method_6F0523B0(int)       { return 0; }
int CAbility::Method_6F052410()          { return 0x0A; }
int CAbility::Method_6F052420(int, int)  { return 0; }

//  Both of these store the global through the caller's pointer and hand
//  that same pointer back - the shipped code keeps arg_0 in eax across the
//  store and returns it untouched.
CFloat* CAbility::Method_6F0523C0(CFloat* out)
{
    *out = g_CFloatZero;
    return out;
}

CFloat* CAbility::Method_6F0523D0(CFloat* out)
{
    *out = g_CFloatZero;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F052440-0x6F0525D0 / 0x6F052620 / 0x6F052670 / 0x6F052680 - the rest
//  of this same default-answer table (ability.h has the naming rationale
//  and the vtable-slot list: 81-91, 95-104, 110, 118, 119).
//----------------------------------------------------------------------------
int CAbility::DefaultIntFieldQueryA(int, int)  { return 0; }
int CAbility::DefaultIntFieldQueryB(int, int)  { return 0; }
int CAbility::DefaultIntFieldQueryC(int, int)  { return 0; }
int CAbility::DefaultIntFieldQueryD(int, int)  { return 0; }
int CAbility::DefaultIntFieldQueryE(int, int)  { return 0; }

CFloat* CAbility::DefaultFloatFieldQueryA(CFloat* out, int, int)
{
    *out = g_CFloatZero;
    return out;
}

CFloat* CAbility::DefaultFloatFieldQueryB(CFloat* out, int, int)
{
    *out = g_CFloatZero;
    return out;
}

CFloat* CAbility::DefaultFloatFieldQueryC(CFloat* out, int)
{
    *out = g_CFloatZero;
    return out;
}

int CAbility::DefaultIntFieldQueryF(int, int)  { return 0; }
int CAbility::DefaultIntFieldQueryG(int, int)  { return 0; }
int CAbility::DefaultIntFieldQueryH(int)       { return 0; }

//  0x6F052540 - same "constant, retn 0" shape as Method_6F052360/0x3A0/
//  0x410 above; see ability.h's own note on the 0x0A/0x1A/0x2A step.
int CAbility::DefaultFieldTypeTag()            { return 0x2A; }

int CAbility::DefaultIntFieldQueryI(int, int, int) { return 0; }

CFloat* CAbility::DefaultFloatFieldQueryD(CFloat* out)
{
    *out = g_CFloatZero;
    return out;
}

int CAbility::DefaultIntFieldQueryJ()          { return 0; }
int CAbility::DefaultIntFieldQueryK()          { return 0; }
int CAbility::DefaultIntFieldQueryL()          { return 0; }
int CAbility::DefaultIntFieldQueryM()          { return 0; }
int CAbility::DefaultIntFieldQueryN()          { return 0; }
int CAbility::DefaultIntFieldQueryO()          { return 0; }
int CAbility::DefaultIntFieldQueryP()          { return 0; }
int CAbility::DefaultIntFieldQueryQ()          { return 0; }
int CAbility::DefaultIntFieldQueryR()          { return 0; }
int CAbility::DefaultIntFieldQueryS()          { return 0; }
//============================================================================
//  0x6F021200 - slot 7, GetAgileTypeId.  `'a','b','i','l'` read low byte
//  first - the same multichar-literal spelling CAgentWar3's own '+w3a'
//  (agentwar3_slots.cpp) already establishes for this family, just with
//  CAbility's own family tag instead of CAgentWar3's.
//============================================================================
unsigned int CAbility::GetAgileTypeId() const
{
    return 'abil';
}

//----------------------------------------------------------------------------
//  0x6F02DF20 - slot 9.  A bare `jmp sub_6F471BF0`, straight into
//  CAgent::Method_0x24 (agentdefaults.cpp) - the qualified-call-to-base
//  shape CAgentWar3::Method_0x24 (agentwar3_slots.cpp) already documents
//  one class up.
//----------------------------------------------------------------------------
int CAbility::Method_0x24(int context)
{
    return CAgent::Method_0x24(context);
}

//----------------------------------------------------------------------------
//  0x6F02E390 - slot 10.  CAgentWar3's own answer (agentwar3_slots.cpp,
//  "the base contributes 0 and each derived layer adds its own one") plus
//  ten more - CAbility's own layer of the same "record format version"
//  accessor.
//----------------------------------------------------------------------------
int CAbility::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 0x0A;
}

//----------------------------------------------------------------------------
//  0x6F0211B0 - slot 22, GetClassName.
//----------------------------------------------------------------------------
const char* CAbility::GetClassName()
{
    return "CAbility";
}

//----------------------------------------------------------------------------
//  0x6F052260 - slot 23.  A bare tail jump straight back into
//  CAgent::Method_0x5C (agent_slot5c.cpp) - CAbility does not touch this
//  slot's behaviour at all, only re-declares it to keep this TU's own
//  vtable entry pointed at a real address.
//----------------------------------------------------------------------------
void CAbility::Method_0x5C()
{
    CAgent::Method_0x5C();
}

//----------------------------------------------------------------------------
//  0x6F052220/0x6F052230/0x6F052250 - three more non-virtual default
//  answers.  0x6F052230 takes one stack argument and ignores it (`retn 4`
//  with the constant already loaded before the argument is even read).
//----------------------------------------------------------------------------
int CAbility::Method_6F052220()           { return 0; }
int CAbility::Method_6F052230(int)        { return 0xDD; }
int CAbility::Method_6F052250()           { return 0; }

//----------------------------------------------------------------------------
//  0x6F03FFA0/0x6F03FFD0 - the +0x48 crossing-boundary pair.  Neither
//  path stores a return value on the shipped side (no `mov eax,` between
//  the guarded call and the shared `retn 4`), which is what a `void`
//  source return looks like at this compiler - the eax the shipped
//  `retn` carries on the no-call path is simply whatever was last in it,
//  not a real answer.  Slots 58/59 (+0xE8/+0xEC) are not named anywhere
//  in this build, so they are reached by raw vtable offset rather than a
//  guessed call.
//----------------------------------------------------------------------------
typedef void (__thiscall *VtableSlot58Fn)(void*);
typedef void (__thiscall *VtableSlot59Fn)(void*);

void CAbility::Method_6F03FFA0(int delta)
{
    int before = m_field48;
    m_field48 += delta;
    if (before > 0)
        return;
    if (m_field48 <= 0)
        return;
    ((VtableSlot58Fn)(*(void***)this)[0xE8 / 4])(this);
}

void CAbility::Method_6F03FFD0(int delta)
{
    int before = m_field48;
    m_field48 -= delta;
    if (before <= 0)
        return;
    if (m_field48 > 0)
        return;
    ((VtableSlot59Fn)(*(void***)this)[0xEC / 4])(this);
}

//----------------------------------------------------------------------------
//  0x6F052430/0x6F0524A0/0x6F052490 - three of the slot-78..104 run.
//----------------------------------------------------------------------------
int CAbility::Method_6F052430()           { return 0x1A; }
int CAbility::Method_6F0524A0(int, int)   { return 0; }
int CAbility::Method_6F052490(int, int)   { return 0; }

//============================================================================
//  Worklist batch C - 14 more of CAbility's own vtable slots.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F052690-0x6F052710 - slots 120-128.  Nine more `xor eax, eax` /
//  `retn` entries continuing DefaultIntFieldQueryA..S above (worklist
//  slots 81-91/95-104/110/118/119); see ability.h's own note by the
//  declarations for why the alphabet just continues past S rather than
//  starting a second family name for an identical shape.
//----------------------------------------------------------------------------
int CAbility::DefaultIntFieldQueryT()  { return 0; }
int CAbility::DefaultIntFieldQueryU()  { return 0; }
int CAbility::DefaultIntFieldQueryV()  { return 0; }
int CAbility::DefaultIntFieldQueryW()  { return 0; }
int CAbility::DefaultIntFieldQueryX()  { return 0; }
int CAbility::DefaultIntFieldQueryY()  { return 0; }
int CAbility::DefaultIntFieldQueryZ()  { return 0; }
int CAbility::DefaultIntFieldQueryAA() { return 0; }
int CAbility::DefaultIntFieldQueryAB() { return 0; }

//----------------------------------------------------------------------------
//  0x6F020B30 - slot 138.  `or eax, 0FFFFFFFFh` / `retn 4`: a constant -1,
//  `this` and the one stack argument both ignored.
//----------------------------------------------------------------------------
int CAbility::Method_6F020B30(int) { return -1; }

//----------------------------------------------------------------------------
//  0x6F021710 - slot 140.  `mov eax, 44h` / `retn`: a constant 0x44,
//  neither `this` nor any argument read (there is none - `retn 0`).
//----------------------------------------------------------------------------
int CAbility::Method_6F021710() { return 0x44; }

//----------------------------------------------------------------------------
//  0x6F02E150 - slot 164.  `this` only, `retn 0`.  Shipped shape:
//      xor eax, eax
//      cmp [ecx+3Ch], eax        ; m_field3C vs 0 (signed)
//      jg  short return-with-0   ; m_field3C > 0 -> false
//      cmp [ecx+44h], eax        ; m_field44 vs 0 (signed)
//      setle al                  ; al = (m_field44 <= 0)
//      retn
//  i.e. true only when m_field3C <= 0 AND m_field44 <= 0.  Written with
//  the same short-circuit shape rather than a single `&&` of two
//  independently-evaluated comparisons, because the shipped code never
//  computes the second comparison when the first already answers false -
//  observable if either field's read could ever fault, and free to keep
//  either way.
//----------------------------------------------------------------------------
int CAbility::Method_6F02E150() const
{
    if ((int)m_field3C > 0)
        return 0;
    return (int)m_field44 <= 0;
}

//  Declared here rather than pulled from a shared header:
//  abilitylevelfield.cpp defines it but does not expose it, and a second
//  declaration with the identical __fastcall(unsigned int, int) signature
//  is the same symbol as far as the linker is concerned.
extern void* __fastcall LookupAbilityTypeRecord(unsigned int typeId, int selector);

//----------------------------------------------------------------------------
//  0x6F02E6A0 - CAbility::QueryLevelRowRaw.  Not a vtable slot itself
//  (SetTypeId below reaches it through an ordinary `call`), but real work
//  now: this batch's own worklist names it directly (a depth<=5 closure
//  member of slots 16/32/33/42/185/186).  Same record-caching shape as
//  CAbilityInventory::GetLevelField (abilitylevelfield.cpp) - resolves
//  m_typeId to its type record via LookupAbilityTypeRecord and caches it
//  at m_field54 - but reads the level row's raw dword at offset 0 rather
//  than the CFloat at +0x20.  Both a negative level and every cache-miss/
//  out-of-range path share one tail that zeroes eax before returning
//  (`xor eax,eax` / `retn 4`), so - unlike an earlier session's own guess
//  in this comment - the negative case answers a real 0, the same
//  explicit-zero shape GetLevelField's own top-of-function guard uses.
//----------------------------------------------------------------------------
struct SAbilityTypeRecordRaw
{
    char            m_reserved00[0x2C];
    int             m_ready;        // 0x2C - 0 means present but rowless
    char            m_reserved30[0x20];
    unsigned int    m_rowCount;     // 0x50
    char*           m_rows;         // 0x54 - 0x68 bytes per row
};

const int kAbilityLevelRowRawStride = 0x68;

int CAbility::QueryLevelRowRaw(int level)
{
    if (level < 0)
        return 0;

    if (m_field54 == 0)
        m_field54 = (unsigned int)LookupAbilityTypeRecord(m_typeId, 0);

    SAbilityTypeRecordRaw* record = (SAbilityTypeRecordRaw*)m_field54;
    if (record == 0 || record->m_ready == 0 ||
        record->m_rowCount <= (unsigned int)level)
        return 0;

    return *(int*)(record->m_rows + level * kAbilityLevelRowRawStride);
}

//----------------------------------------------------------------------------
//  0x6F03FF40 - slot 42, SetTypeId.  When the new rawcode already matches
//  m_typeId, every bit of cache-refresh work is skipped and the function
//  falls straight through to the slot-108 notification.  Otherwise it
//  stores the new id, and - unless either of two per-derived-class gates
//  (slots 120/121, this batch's own DefaultIntFieldQueryT/U above, both a
//  constant 0 in this build with nothing yet overriding them) answers
//  nonzero - refreshes m_field54 (the type record itself, the same
//  LookupAbilityTypeRecord call abilitylevelfield.cpp already makes) and
//  m_field4C (the current level's raw row value, via the out-of-scope
//  0x6F02E6A0 above).  Every path ends the same way: dispatch slot 108
//  (nullsub_91 in this build - a per-derived-class "type changed" notify
//  with nothing to do here).  Slots 108/120/121 are reached by raw vtable
//  offset, not a guessed named call, for the same reason
//  Method_6F03FFA0/0xFFD0 above already give for slots 58/59.
//----------------------------------------------------------------------------
typedef int  (__thiscall *TypeIdChangeGateFn)(void*);
typedef void (__thiscall *TypeIdChangeNotifyFn)(void*);

void CAbility::SetTypeId(unsigned int typeId)
{
    //  Written with the shipped code's own two early `jz`s straight to the
    //  shared tail, rather than a nested `if`, because that is what the
    //  dump actually does: the "already this type" case and both gated-off
    //  cache-refresh cases all skip to the same slot-108 dispatch, they
    //  don't fall out of a block first.
    if (typeId == m_typeId)
        goto dispatch;

    m_typeId = typeId;

    if (((TypeIdChangeGateFn)(*(void***)this)[0x1E0 / 4])(this) != 0)
        goto dispatch;
    if (((TypeIdChangeGateFn)(*(void***)this)[0x1E4 / 4])(this) != 0)
        goto dispatch;

    m_field54 = (unsigned int)LookupAbilityTypeRecord(m_typeId, 0);
    m_field4C = QueryLevelRowRaw((int)m_field50);

dispatch:
    ((TypeIdChangeNotifyFn)(*(void***)this)[0x1B0 / 4])(this);
}


//  0x6F32C8E0 - LookupAbilityFieldRecord: real body now, not a thunk -
//  see ability_typefieldrecord.cpp.  Its earlier description here ("the
//  generic ability-field lookup GetPlacementModel below tail-calls
//  into") does not survive reading it: GetPlacementModel's own callee is
//  the *separate*, unrelated helper 0x6F32E570 folded into its body
//  below, not this function at all; this one is the fallback
//  ability_fieldart.cpp's eight accessors call on their own per-instance
//  cache miss, and its second argument is a Storm allocation-log tag,
//  never a field-name selector - see the header for why.
#include "ability_typefieldrecord.h"

//----------------------------------------------------------------------------
//  0x6F02E220 - slot 172, GetPlacementModel.  A tail jump into a shared
//  helper (0x6F32E570, folded into this body rather than reconstructed
//  as its own function - it has no vtable slot of its own and is reached
//  only from here in this call tree) that looks the ability's own type
//  record up by the literal string "placementModel" and hands back its
//  +0x40 field, or 0 if the record was not found at all.
//----------------------------------------------------------------------------
void* CAbility::GetPlacementModel()
{
    void* record = LookupAbilityFieldRecord(m_typeId, "placementModel");
    if (record == 0)
        return 0;
    return *(void**)((char*)record + 0x40);
}

//============================================================================
//  Worklist batch E - the final round, 27 more of CAbility's own vtable
//  slots.  Closes out every slot in docs/targets/vtables/CAbility.txt.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F052870-0x6F0528C0 - slots 155-160, six more of the contiguous
//  default-answer table above (worklist slots 81-91/95-104/110/118/119/
//  120-128 already used every letter through AB); continuing the same
//  alphabet, the same reason the 120-128 run continued past S.
//----------------------------------------------------------------------------
int CAbility::DefaultIntFieldQueryAC(int, int) { return 0; }
int CAbility::DefaultIntFieldQueryAD(int)      { return 0; }
int CAbility::DefaultIntFieldQueryAE()         { return 0; }
int CAbility::DefaultIntFieldQueryAF()         { return 0; }
int CAbility::DefaultIntFieldQueryAG()         { return 0; }
int CAbility::DefaultIntFieldQueryAH()         { return 0; }

//----------------------------------------------------------------------------
//  0x6F0528F0 - slot 163.  `mov eax,[esp+arg_0]` / `retn 4`: hands its one
//  stack argument straight back rather than answering a fixed 0 - see
//  ability.h's own note on why this is not part of the DefaultIntFieldQuery
//  family.
//----------------------------------------------------------------------------
int CAbility::IdentityIntFieldQuery(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F052900 - slot 174.  Same g_CFloatZero-into-out-pointer shape as
//  DefaultFloatFieldQueryD above (the next letter of that family).
//----------------------------------------------------------------------------
CFloat* CAbility::DefaultFloatFieldQueryE(CFloat* out)
{
    *out = g_CFloatZero;
    return out;
}

//============================================================================
//  Worklist batch D - 26 of the final 28 CAbility vtable slots.  Two
//  (19, 52) were left `pending` by this batch and closed by a later
//  depth<=5 closure-verification session - see ability_typemigrate.cpp
//  (slot 19, 0x6F040AE0, the 900+-instruction FourCC field-tag switch)
//  and HasPendingTypeUpgrade above (slot 52, 0x6F052990).
//
//  Everything else here turned out to be reachable through pieces this
//  repo already has: LookupAbilityTypeRecord (0x6F265ED0, a thunk
//  abilitylevelfield.cpp already declares) and QueryLevelRowRaw's own
//  real body (0x6F02E6A0, above) between them cover every "refresh the
//  type record cache" tail these slots share with SetTypeId, and
//  SAttachmentOwnerQuery::QueryOwner / SAttachmentTarget::Hide /
//  CAgent::SetTrackedTarget / PackFourCCString were all already real
//  bodies elsewhere in this tree - which is why three of the "released"
//  batch-A slots (16, 32, 33) closed this round after all: their own
//  blocking closures (sub_6F265ED0, sub_6F02E6A0) already had thunks
//  covering them by the time this session read the dumps again.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F052720-0x6F0527F0 (minus the gaps used elsewhere) - ten more of the
//  same "constant 0, this and every stack argument ignored" shape as
//  DefaultIntFieldQueryT..AB above (slots 129/130/132/133/134/135/139/
//  142/143/146) - continuing the same alphabet past AB the way that run's
//  own header comment already explains for going past Z.  Picks up at AJ,
//  not AC - see ability.h's own note by the declarations for why.
//----------------------------------------------------------------------------
int CAbility::DefaultIntFieldQueryAJ()     { return 0; }
int CAbility::DefaultIntFieldQueryAK()     { return 0; }
int CAbility::DefaultIntFieldQueryAL()     { return 0; }
int CAbility::DefaultIntFieldQueryAM()     { return 0; }
int CAbility::DefaultIntFieldQueryAN()     { return 0; }
int CAbility::DefaultIntFieldQueryAO(int)  { return 0; }
int CAbility::DefaultIntFieldQueryAP(int)  { return 0; }
int CAbility::DefaultIntFieldQueryAQ()     { return 0; }
int CAbility::DefaultIntFieldQueryAR(int, int) { return 0; }
int CAbility::DefaultIntFieldQueryAS(int)  { return 0; }

//----------------------------------------------------------------------------
//  0x6F052790/0x6F052780 - slots 136/137.  `or eax,0FFFFFFFFh` - constant
//  -1, not the DefaultIntFieldQuery family's own 0, so named by address
//  the same way Method_6F020B30 above is.
//----------------------------------------------------------------------------
int CAbility::Method_6F052790(int, int, int) { return -1; }
int CAbility::Method_6F052780(int, int)      { return -1; }

//----------------------------------------------------------------------------
//  0x6F0527E0/0x6F052860/0x6F052850/0x6F052840/0x6F052830/0x6F052820/
//  0x6F052810/0x6F052800 - eight more of Method_6F052230's own shape
//  (`mov eax,0DDh` - constant 0xDD), each with its own stack-argument
//  count and nothing else different.
//----------------------------------------------------------------------------
int CAbility::Method_6F0527E0(int, int, int)                       { return 0xDD; }
int CAbility::Method_6F052860(int, int, int, int, int)             { return 0xDD; }
int CAbility::Method_6F052850(int, int, int, int)                  { return 0xDD; }
int CAbility::Method_6F052840(int, int, int, int, int, int)        { return 0xDD; }
int CAbility::Method_6F052830(int, int, int, int, int)             { return 0xDD; }
int CAbility::Method_6F052820(int, int, int, int, int)             { return 0xDD; }
int CAbility::Method_6F052810(int, int, int, int)                  { return 0xDD; }
int CAbility::Method_6F052800(int, int, int)                       { return 0xDD; }

//----------------------------------------------------------------------------
//  0x6F021720 - slot 141.  Same "write to caller's out pointer, hand the
//  pointer back" shape as Method_6F0523C0/Method_6F0523D0 above, but the
//  value is a real computation - pathfinding's own GetBuildingAngle()
//  (Pathfinding/buildingangle.cpp) rather than the shared zero constant.
//  Declared locally the same way jasssetitemposition.cpp already does:
//  buildingangle.cpp does not expose it through a shared header.
//----------------------------------------------------------------------------
extern CFloat __fastcall GetBuildingAngle();

CFloat* CAbility::Method_6F021720(CFloat* out)
{
    new (out) CFloat(GetBuildingAngle());
    return out;
}

//----------------------------------------------------------------------------
//  0x6F052910 - slot 175.  Writes CFloatFromInt(400) (softfloat.cpp)
//  rather than g_CFloatZero - a real encoded non-zero constant, not the
//  default-answer family above (see ability.h).
//----------------------------------------------------------------------------
CFloat* CAbility::DefaultFloatFieldQuery400(CFloat* out)
{
    //  Placement-construct directly at *out rather than `*out =
    //  CFloatFromInt(400);` - an assignment materialises the return value
    //  in a stack temp first and copies it, where the shipped code passes
    //  `out` itself as CFloatFromInt's own hidden-return-buffer argument.
    new (out) CFloat(CFloatFromInt(400));
    return out;
}

//----------------------------------------------------------------------------
//  0x6F052930 - slot 180, another int-default entry.
//----------------------------------------------------------------------------
int CAbility::DefaultIntFieldQueryAI() { return 0; }

//----------------------------------------------------------------------------
//  0x6F052960/0x6F052970 - slots 183/184.  Same shape as
//  DefaultFloatFieldQueryC above (CFloat*, int, retn 8) - two more letters
//  of that family.
//----------------------------------------------------------------------------
CFloat* CAbility::DefaultFloatFieldQueryF(CFloat* out, int)
{
    *out = g_CFloatZero;
    return out;
}

CFloat* CAbility::DefaultFloatFieldQueryG(CFloat* out, int)
{
    *out = g_CFloatZero;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F021300 - slot 176.  `this` is never read: packs "TargetingColors"/
//  "TargetLightColor" into the caller's buffer via GetConfigColor
//  (configcolor.cpp) and hands the same pointer back.  Written as two
//  statements rather than `return GetConfigColor(...)` because the shipped
//  code reloads the out pointer from its own callee-saved register after
//  the call (`mov eax, esi`) instead of trusting GetConfigColor's own
//  return value, even though the two are always equal.
//----------------------------------------------------------------------------
unsigned int* __fastcall GetConfigColor(unsigned int* out, const char* section,
                                        const char* key, int index);

unsigned int* CAbility::GetTargetLightColor(unsigned int* out)
{
    GetConfigColor(out, "TargetingColors", "TargetLightColor", 0);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F472890 - SAttachmentOwnerQuery::QueryOwner (attachmentownerquery.cpp),
//  redeclared here rather than pulled from a header: that file keeps the
//  struct private, and a second declaration with the identical signature
//  is the same symbol as far as the linker is concerned.
//----------------------------------------------------------------------------
struct SAttachmentOwnerQuery
{
    void* __thiscall QueryOwner();
};

//----------------------------------------------------------------------------
//  0x6F29C1B0 - a fifth AGILE_TYPE_ID probe into the unit-type registry
//  (unittypetable.h/.cpp), the same `Ptr(id.Hash(), id)` shape
//  unittypequeries.cpp's own four already use, at field +0x1C0 - the
//  address-suffixed naming those four already establish for a field this
//  call tree does not otherwise identify.
//----------------------------------------------------------------------------
int __fastcall UnitTypeField_6F29C1B0(unsigned int fourCC);

//----------------------------------------------------------------------------
//  0x6F31F5D0 - ResolveSkinValueByRace (gameuiskinvalue.cpp): maps a race
//  index (1-5) to the GameUI skin section name and tail-calls
//  ResolveSkinValue, or - for any other index - looks the "Default"
//  section up and reads the key out of it directly, skipping
//  ResolveSkinValue's own CustomSkin-override check.  Declared here rather
//  than pulled from a header for the same reason QueryOwner is: that file
//  does not expose it, and the signature is enough to link.
//----------------------------------------------------------------------------
const char* __fastcall ResolveSkinValueByRace(const char* key, int race);

//----------------------------------------------------------------------------
//  0x6F02E230 - slot 173, GetPlacementCursorTexture.  Resolves this
//  ability's owner (cached at +0x30, the same SAttachmentOwnerQuery shape
//  SAttachmentTarget::Show's own +0x30 cache uses - see
//  attachmentownerquery.cpp), reads the owner's own type FourCC at +0x30,
//  and looks up "PlacementCursor" in the GameUI skin keyed by that type's
//  race (UnitTypeField_6F29C1B0).  Falls back to a fixed selection-arrow
//  texture when nothing resolves.
//----------------------------------------------------------------------------
const char* CAbility::GetPlacementCursorTexture()
{
    void* owner = (void*)m_field30;
    if (owner == 0)
        owner = ((SAttachmentOwnerQuery*)this)->QueryOwner();

    unsigned int ownerTypeId = *(unsigned int*)((char*)owner + 0x30);
    int race = UnitTypeField_6F29C1B0(ownerTypeId);

    const char* value = ResolveSkinValueByRace("PlacementCursor", race);
    if (value != 0)
        return value;
    return "ReplaceableTextures\\Selection\\SpellAreaOfEffect.blp";
}

//----------------------------------------------------------------------------
//  0x6F02E640 - GetCachedFieldRecord.  Resolves and caches the ability's
//  own generic field-lookup record the same way SetTypeId refreshes
//  m_field54 (via LookupAbilityTypeRecord), then hands back that record's
//  own +0x28 cache slot when the record is present and its +0x2C "ready"
//  flag is set - the same two fields SetTypeId's own type-record cache
//  already carries, one module over.  Not a vtable slot: every one of the
//  eight field accessors below reaches it with an ordinary `call`.
//----------------------------------------------------------------------------
struct SAbilityTypeRecordFieldCache
{
    char  m_reserved00[0x28];
    void* m_fieldRecord; // +0x28
    int   m_ready;       // +0x2C
};

void* CAbility::GetCachedFieldRecord()
{
    if (m_field54 == 0)
        m_field54 = (unsigned int)LookupAbilityTypeRecord(m_typeId, 0);

    SAbilityTypeRecordFieldCache* record =
        (SAbilityTypeRecordFieldCache*)m_field54;
    if (record != 0 && record->m_ready != 0)
        return record->m_fieldRecord;
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F02E890 - slot 187, QueryLevelRowField14.  The same per-level type-
//  record row lookup as CAbilityInventory::GetLevelField
//  (abilitylevelfield.cpp) - negative level -> g_CFloatZero, lazily
//  resolve/cache the type record at m_field54, bounds-check the level
//  against the record's own row count - but reads the row's +0x14 dword
//  rather than GetLevelField's own +0x20.  A local, minimal view of the
//  same type record abilitylevelfield.cpp's own SAbilityTypeRecord names,
//  not a shared one: this module reads different fields of it than that
//  one does, and CLAUDE.md's rule is that only touched members get named.
//----------------------------------------------------------------------------
struct SAbilityTypeRecordRowView
{
    char         m_reserved00[0x2C];
    int          m_ready;      // +0x2C
    char         m_reserved30[0x50 - 0x30];
    unsigned int m_rowCount;   // +0x50
    char*        m_rows;       // +0x54
};

const int kAbilityLevelRowStride14 = 0x68;
const int kAbilityLevelRowField14  = 0x14;

CFloat* CAbility::QueryLevelRowField14(CFloat* out, int level)
{
    if (level < 0)
    {
        *out = g_CFloatZero;
        return out;
    }

    if (m_field54 == 0)
        m_field54 = (unsigned int)LookupAbilityTypeRecord(m_typeId, 0);

    SAbilityTypeRecordRowView* record = (SAbilityTypeRecordRowView*)m_field54;
    if (record != 0 && record->m_ready != 0 &&
        record->m_rowCount > (unsigned int)level)
    {
        *out = *(const CFloat*)(record->m_rows +
                                level * kAbilityLevelRowStride14 +
                                kAbilityLevelRowField14);
        return out;
    }

    *out = g_CFloatZero;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F02E840 - reached only from slot 52 (0x6F052990) below.  The same
//  per-level row lookup as QueryLevelRowField14/QueryLevelRowRaw above,
//  reading the row's +0x10 dword this time - a plain `int`, not a CFloat,
//  matching this function's own `retn 4` (no hidden CFloat return buffer)
//  and its caller passing m_field50 as a plain stack argument rather than
//  address-of-out.  Negative level and every other bail answer 0, the
//  same explicit-zero tail QueryLevelRowRaw (ability_typemigrate's own
//  sibling closure member, 0x6F02E6A0) already establishes for this
//  family.
//----------------------------------------------------------------------------
const int kAbilityLevelRowField10 = 0x10;

int CAbility::QueryLevelRowField10(int level)
{
    if (level < 0)
        return 0;

    if (m_field54 == 0)
        m_field54 = (unsigned int)LookupAbilityTypeRecord(m_typeId, 0);

    SAbilityTypeRecordRowView* record = (SAbilityTypeRecordRowView*)m_field54;
    if (record == 0 || record->m_ready == 0 ||
        record->m_rowCount <= (unsigned int)level)
        return 0;

    return *(int*)(record->m_rows + level * kAbilityLevelRowStride14 + kAbilityLevelRowField10);
}

//----------------------------------------------------------------------------
//  0x6F02E570 - reached only from slot 52 below.  Same lazy m_field54
//  cache/refresh as every sibling in this family, but its own field is
//  the type record's +0x3C dword (a different member than either
//  QueryLevelRowField14's +0x14 row field or QueryLevelRowRaw's row-0
//  field - not named further per CLAUDE.md's own "only touched members"
//  rule) and its "not ready" answer is 1, not 0 - the caller below reads
//  it as an upgrade-step count and treats "no record yet" as "one step
//  still pending" rather than "none".
//----------------------------------------------------------------------------
struct SAbilityTypeRecordStepCountView
{
    char         m_reserved00[0x2C];
    int          m_ready;      // +0x2C
    char         m_reserved30[0x3C - 0x30];
    unsigned int m_stepCount;  // +0x3C
};

unsigned int CAbility::QueryUpgradeStepCount()
{
    if (m_field54 == 0)
        m_field54 = (unsigned int)LookupAbilityTypeRecord(m_typeId, 0);

    SAbilityTypeRecordStepCountView* record =
        (SAbilityTypeRecordStepCountView*)m_field54;
    if (record != 0 && record->m_ready != 0)
        return record->m_stepCount;
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F052990 - slot 52, previously pending ("calls two functions with no
//  dump anywhere in this repo" - both now reconstructed above from a
//  fresh depth-5 dump).  Gate: bit 0x20 of CAgentWar3::m_flags blocks
//  outright, then three per-derived-class hooks (slots 118/122/123 -
//  DefaultIntFieldQueryR/V/W, all a constant 0 in this build) each block
//  on a nonzero answer *except* the third, which instead short-circuits
//  straight to the "yes" answer on nonzero.  With all three gates at
//  their base-class 0, the real question is upgrade-step-and-row data:
//  yes unless the type has at most one upgrade step *and* its own
//  m_field50 (the ability's current level) row has no +0x10 data left.
//----------------------------------------------------------------------------
typedef int (__thiscall *AbilityIntGateFn)(void*);

int CAbility::HasPendingTypeUpgrade()
{
    if (m_flags & 0x20)
        return 0;

    if (((AbilityIntGateFn)(*(void***)this)[0x1EC / 4])(this) != 0)
        return 0;
    if (((AbilityIntGateFn)(*(void***)this)[0x1E8 / 4])(this) != 0)
        return 0;
    if (((AbilityIntGateFn)(*(void***)this)[0x1D8 / 4])(this) != 0)
        return 1;

    if (QueryUpgradeStepCount() > 1)
        return 1;

    return QueryLevelRowField10((int)m_field50) != 0;
}

//----------------------------------------------------------------------------
//  0x6F05F550 - slot 41.  Hide this ability's own attachment target
//  (SAttachmentTarget::Hide, Widget/attachmentcounters.cpp - declared,
//  not defined, here the same way Item/item_attachments.cpp already
//  reaches it), clear the "targeted" flag bit unless slot 51 answers
//  zero (dispatching slot 56 first when it does), notify slot 49, and
//  cancel the embedded CAgentTimer.  `this` only, `retn 0`.
//----------------------------------------------------------------------------
struct SAttachmentTarget { void Hide(int alsoTotal, int delta); };

typedef int  (__thiscall *VtableSlot51QueryFn)(void*);
typedef void (__thiscall *VtableSlot56NotifyFn)(void*);
typedef void (__thiscall *VtableSlot49NotifyFn)(void*);

void CAbility::Method_6F05F550()
{
    ((SAttachmentTarget*)this)->Hide(0, 1);

    if (((VtableSlot51QueryFn)(*(void***)this)[0xCC / 4])(this) == 0)
        ((VtableSlot56NotifyFn)(*(void***)this)[0xE0 / 4])(this);

    m_flags &= ~2u;

    ((VtableSlot49NotifyFn)(*(void***)this)[0xC4 / 4])(this);

    m_timer.Cancel();

    m_flags |= 0x40;
}

//----------------------------------------------------------------------------
//  0x6F02E4B0 - slot 16.  Resolve this ability's owner
//  (SAttachmentOwnerQuery::QueryOwner, Widget/attachmentownerquery.cpp -
//  the same struct GetPlacementCursorTexture above already declares in
//  this TU) into m_field30, and refresh the type-record cache (m_field54)
//  via the same LookupAbilityTypeRecord call SetTypeId above already
//  makes.  `this` only, `retn 0`.  Named RefreshOwningPlayerColor in
//  ability.h, not for what it does - see that header's own comment on why
//  this specific slot has to keep CAgent's own name.
//----------------------------------------------------------------------------
void CAbility::RefreshOwningPlayerColor()
{
    m_field30 = (unsigned int)((SAttachmentOwnerQuery*)this)->QueryOwner();
    m_field54 = (unsigned int)LookupAbilityTypeRecord(m_typeId, 0);
}

//----------------------------------------------------------------------------
//  0x6F052A60 - a private helper, not itself a vtable slot: point this
//  ability's tracked-target link at `target` (CAgent::SetTrackedTarget,
//  Agent/agentsettrackedtarget.cpp), reset the +0x24/+0x28 handle-ref
//  pair CUnitRefList owns to "not yet bound", notify slot 57, remember
//  the target at m_field30, and - only when the target's own +0x5C bit 2
//  is set, tested on the raw pointer with no null guard, exactly as the
//  shipped code has it - flag this object as "targeted" (bit 2) and
//  notify slot 50.  Finally, only when the type-record cache is still
//  empty and both of the slot-120/121 gates (DefaultIntFieldQueryT/U,
//  always 0 in this build) answer 0, refresh the cache the same way
//  SetTypeId's own tail does.  `this` plus one stack argument, `retn 4`.
//  Reached by both slot 32 (Method_6F052B10) and slot 33
//  (Method_6F052AF0) below.
//----------------------------------------------------------------------------
typedef void (__thiscall *VtableSlot57NotifyFn)(void*);
typedef void (__thiscall *VtableSlot50NotifyFn)(void*);

void CAbility::Method_6F052A60(void* target)
{
    m_flags |= 0x10;

    SetTrackedTarget(target);

    m_field28 = -1;
    m_field24 = -1;

    m_field30 = (unsigned int)target;

    ((VtableSlot57NotifyFn)(*(void***)this)[0xE4 / 4])(this);

    if ((((const char*)target)[0x5C] & 4) != 0)
    {
        m_flags |= 2;
        ((VtableSlot50NotifyFn)(*(void***)this)[0xC8 / 4])(this);
    }

    if (m_field54 != 0)
        return;
    if (((TypeIdChangeGateFn)(*(void***)this)[0x1E0 / 4])(this) != 0)
        return;
    if (((TypeIdChangeGateFn)(*(void***)this)[0x1E4 / 4])(this) != 0)
        return;

    m_field54 = (unsigned int)LookupAbilityTypeRecord(m_typeId, 0);
    m_field4C = QueryLevelRowRaw((int)m_field50);
}

//----------------------------------------------------------------------------
//  0x6F052AF0 - slot 33.  Refresh m_typeId from this object's own
//  GetAgileTypeId (slot 7, +0x1C - raw vtable dispatch rather than a
//  direct call for the same reason attachmentcounters.cpp's own slot-0xCC
//  dispatch is raw: this class's declared virtuals do not span the real
//  189-slot table, so a direct call would not land on the real slot),
//  then hand the one stack argument straight to Method_6F052A60 above.
//  `this` plus one stack argument, `retn 4`.
//----------------------------------------------------------------------------
typedef unsigned int (__thiscall *VtableSlot7Fn)(void*);

void CAbility::Method_6F052AF0(void* target)
{
    m_typeId = ((VtableSlot7Fn)(*(void***)this)[0x1C / 4])(this);
    Method_6F052A60(target);
}

//----------------------------------------------------------------------------
//  0x6F052B10 - slot 32.  Resolve the second stack argument's effective
//  type id (SAliasableRecord::ResolveTypeId, abilityaliastyperesolve.cpp -
//  its own TU: 0x6F262Axxx is a different module and inlining it here
//  would fold away the real call the shipped code makes), refresh the
//  type-record cache from it, hand the first stack argument to
//  Method_6F052A60 above, then clear this object's own "targeted" flag
//  bit unless Method_6F02E540 (ability_typerecordcache.cpp - same
//  cross-module reasoning) answers nonzero.  `this` plus two stack
//  arguments, `retn 8`.
//----------------------------------------------------------------------------
class SAliasableRecord { public: unsigned int __thiscall ResolveTypeId(); };

void CAbility::Method_6F052B10(void* arg0, void* recordField)
{
    unsigned int typeId = ((SAliasableRecord*)recordField)->ResolveTypeId();

    m_field54 = (unsigned int)LookupAbilityTypeRecord(typeId, 0);
    m_field4C = QueryLevelRowRaw((int)m_field50);

    m_typeId = typeId;
    Method_6F052A60(arg0);

    if (Method_6F02E540() == 0)
        m_flags &= ~0x10u;
}

//----------------------------------------------------------------------------
//  0x6F0529F0 - slot 131.  `Method_6F02E5A0() > 1` as a 0/1 answer - the
//  shipped `cmp`/`sbb`/`neg` unsigned-less-than idiom, spelled here as the
//  comparison it computes.  Method_6F02E5A0 is its own TU
//  (ability_typerecordcache.cpp) for the same cross-module reason as
//  Method_6F052B10 above: 0x6F02E5A0 and this root are different modules,
//  and /Ob2 folds a same-TU callee this small into its caller, which is
//  not what the shipped code (a real `call`) does.  `this` only, `retn 0`.
//----------------------------------------------------------------------------
int CAbility::Method_6F0529F0()
{
    return 1u < (unsigned int)Method_6F02E5A0();
}
