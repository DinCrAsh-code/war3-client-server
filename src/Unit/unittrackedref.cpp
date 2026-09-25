//============================================================================
//  0x6F4773E0 / 0x6F477440 / 0x6F477460 / 0x6F477400 - four of
//  SUnitTrackedRef's own methods.  See unittrackedref.h.
//
//  Own translation unit.  JASS_UnitItemInSlot's tree added two more of
//  them, 0x6F4773A0 (GetValue) and 0x6F4774A0 (SetRange); the last one
//  the header declares, 0x6F477350 (SetValue), is still a thunk in
//  unreconstructed_thunks.cpp.  None of the seven calls any of the
//  others, so they share this TU without risk of being folded together.
//============================================================================
#include "unittrackedref.h"
#include "game.h"

//  0x6F4A9390 (`retn 4`) - publish a {low, high} pair onto the resolved
//  object.  A thiscall member of whatever LookupHandle returns, not a
//  __fastcall free function: the shipped call puts the object in ecx and
//  *pushes* the pair (`mov ecx, eax` / `push edx` / `call`), where
//  __fastcall would have carried the pair in edx and cleaned nothing.
//  Declared on a minimal local type rather than added to CHandleObject,
//  which nothing else here needs to grow; reconstructed with this target,
//  in handleobjectrange.cpp, on this same type - MSVC puts the class name
//  in the mangled name, so the definition cannot be moved onto a fuller
//  one without leaving this call unresolved.
struct SHandleObjectRange
{
    void SetRange(const CFloat* pair);   // 0x6F4A9390
};

//  GetValue below reaches CPathTrace::GetTravelDistance (0x6F497460) and
//  CPathTrace::RampValueAt (0x6F477180) on the object LookupHandle hands
//  back, which is therefore a CPathTrace - the same conclusion
//  CPathRef::ToWorldVec3 reached about its own resolved object.  Both are
//  already reconstructed and both are declared in game.h, included above.

//  The resolved object's own low/high words.  Raw offsets rather than
//  fields on CHandleObject: nothing else in this repo reaches them, and
//  CLAUDE.md's rule is that only what is touched gets a name.
static CFloat* LowOf(CHandleObject* obj)  { return (CFloat*)((char*)obj + 0x80); }
static CFloat* HighOf(CHandleObject* obj) { return (CFloat*)((char*)obj + 0x84); }

//  Both getters hand the caller's own buffer straight back.  The two slot
//  bodies one class up (unit_life.cpp) reload it for themselves
//  and never read the result - but returning it is what puts `out` in eax
//  rather than the value, which is the shipped register assignment.
CFloat* SUnitTrackedRef::GetHigh(CFloat* out)
{
    *out = *HighOf(LookupHandle(m_handle, m_typeTag));
    return out;
}

CFloat* SUnitTrackedRef::GetLow(CFloat* out)
{
    *out = *LowOf(LookupHandle(m_handle, m_typeTag));
    return out;
}

void SUnitTrackedRef::SetLow(const CFloat* value)
{
    CHandleObject* obj = LookupHandle(m_handle, m_typeTag);
    //  kCFloatNoInit on both: the zeroing default constructor would put a
    //  dead `mov [pair+4], 0` in front of the real store, which the
    //  shipped code does not have.
    //  kCFloatNoInit on both: the zeroing default constructor would put a
    //  dead `mov [pair+4], 0` in front of the real store, which the
    //  shipped code does not have.
    //
    //  One instruction out of order and it is the scheduler's, not the
    //  source's: the shipped code loads the high end before storing the
    //  low one, this build stores then loads.  Staging both through named
    //  locals first was measured and comes out worse (it moves the *store*
    //  order too); its twin SetHigh below is EXACT with the identical
    //  shape, which is what says the difference is allocation and not
    //  spelling.
    CFloat pair[2] = { CFloat(kCFloatNoInit), CFloat(kCFloatNoInit) };
    pair[0] = *value;
    pair[1] = *HighOf(obj);
    ((SHandleObjectRange*)obj)->SetRange(pair);
}

void SUnitTrackedRef::SetHigh(const CFloat* value)
{
    CHandleObject* obj = LookupHandle(m_handle, m_typeTag);
    CFloat pair[2] = { CFloat(kCFloatNoInit), CFloat(kCFloatNoInit) };
    pair[0] = *LowOf(obj);
    pair[1] = *value;
    ((SHandleObjectRange*)obj)->SetRange(pair);
}

//----------------------------------------------------------------------------
//  0x6F4773A0.  The published value: resolve the object, ask it how far
//  along it is, and evaluate its ramp at that distance into the caller's
//  buffer.
//
//  `out` is returned, and that is what puts it in eax - the shipped
//  `mov eax,edi` right before the epilogue.  Every caller in this tree
//  ignores the result and reads its own buffer instead, so nothing
//  observes it; it is here because the register assignment does.
//
//  The intermediate distance is a real named local whose address is
//  taken, and the pointer GetTravelDistance hands back is passed straight
//  on rather than dereferenced: the shipped code pushes eax, the returned
//  pointer, not `[eax]`.
//----------------------------------------------------------------------------
CFloat* SUnitTrackedRef::GetValue(CFloat* out)
{
    CPathTrace* trace = (CPathTrace*)LookupHandle(m_handle, m_typeTag);

    //  Bound as a reference, not copied into a named CFloat: the shipped
    //  code pushes the pointer GetTravelDistance returned straight into
    //  RampValueAt's argument list, which is exactly what pathtraceramp.cpp
    //  already records as this pair's idiom one class up.
    const CFloat& travelled = trace->GetTravelDistance();
    trace->RampValueAt(out, &travelled);

    return out;
}

//----------------------------------------------------------------------------
//  0x6F4774A0.  Both ends at once.
//
//  The pair is staged through a local array exactly the way SetLow and
//  SetHigh stage theirs - two loads into the frame, then one push of its
//  address - and the resolve happens *after* both stores, which is why
//  `lea eax,[esp+var_8]` and its `push` sit between the field loads and
//  the LookupHandle call rather than after it.
//----------------------------------------------------------------------------
void SUnitTrackedRef::SetRange(const CFloat* low, const CFloat* high)
{
    CFloat pair[2] = { CFloat(kCFloatNoInit), CFloat(kCFloatNoInit) };
    pair[0] = *low;
    pair[1] = *high;

    ((SHandleObjectRange*)LookupHandle(m_handle, m_typeTag))->SetRange(pair);
}
