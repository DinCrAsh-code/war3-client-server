//============================================================================
//  0x6F29F180 - publish a fresh +0xFC tracked-ref value from this object's
//  own cached type-190 field, then (unless the global item-slot table's own
//  +0x80 flag says skip it) nudge that same ref by the delta between the
//  live type-190 reading and the value just published - the same
//  "publish, then patch by a delta on top of the current value" shape
//  SUnitTrackedRef::SetLow/SetHigh already document (unittrackedref.h).
//
//  Unlike the bare-receiver QueryField190 wrapper (Unit/
//  cunit_agent1_unittypefield190.cpp), `this` here really is CUnit*: the
//  tail call is to the already-thunked CUnit::RefreshField10C() (unit.h,
//  Unit/unit_fogrefresh_thunks.cpp) with the same `this` unchanged, which
//  only makes sense for a genuine CUnit member, so this one is written
//  against the real unit.h class and its own m_refFC (+0xFC) rather than a
//  private redeclaration.  The +0x20 flags word unit.h does not yet name is
//  read/written through a raw offset instead of guessing a field name onto
//  that struct - the same "only touched members get names" rule CLAUDE.md's
//  own house rules give, just applied to a field that belongs to an
//  *established* class rather than an unidentified one.
//
//  Own translation unit, deliberately not folded into
//  cunit_agent1_unittypefield190.cpp alongside QueryField190 (0x6F29F020)
//  itself, even though the two addresses sit right next to each other:
//  with both definitions visible in the same TU the compiler inlines
//  QueryField190's one-line body straight into this call site, but the
//  dump's own canonical form still names a real out-of-line
//  `call SUnitTypeField190Wrapper::QueryField190` here - the two addresses
//  were compiled from genuinely separate translation units in the shipped
//  build (a real "own TU" case, not a guess) even though address-adjacent.
//  Declaring QueryField190 without defining it here is the same technique
//  SFourCCHolder030::PopulateTypeInstance's own header comment gives for
//  UTQ_6F29C010 (Misc/cunit_agent8_batch1.cpp).
//
//  SFourCCHolder030 and HandleRefFieldOwner_6F473170 are re-declared here
//  rather than shared through a header, matching every other private
//  one-field view struct in this codebase.
//
//  typeInstance is populated once and handed to SetValue immediately, then
//  read a *second* time (not cached) as the left-hand side of the delta -
//  dump-confirmed: the getter/populate call only happens once, but the
//  buffer it filled is referenced twice, matching a plain C++ local reused
//  across two statements.  Every CFloat local that an out-pointer call
//  fills before it is ever read is declared with the `kCFloatNoInit` tag
//  (CFloat.h) rather than default-constructed - the plain default ctor
//  zero-fills, which is one extra store the dump does not have at any of
//  these three locals.
//============================================================================
#include "itemhandlemain.h"
#include "unit.h"

struct SFourCCHolder030
{
    void* __thiscall PopulateTypeInstance(void* out);
    char         m_reserved00[0x30];
    unsigned int m_fourCC;   // +0x30
};

struct HandleRefFieldOwner_6F473170 { int __thiscall QueryField78(); };

//  0x6F29F020 - Unit/cunit_agent1_unittypefield190.cpp; declared, not
//  defined, here (see the file header comment above).
struct SUnitTypeField190Wrapper { CFloat* __thiscall QueryField190(CFloat* out); };

void __fastcall CUnit_PublishType190DeltaToTrackedRefFC(CUnit* self)
{
    *(unsigned int*)((char*)self + 0x20) |= 0x400;

    CFloat typeInstance(kCFloatNoInit);
    ((SFourCCHolder030*)self)->PopulateTypeInstance(&typeInstance);
    self->m_refFC.SetValue(&typeInstance);

    if (!((HandleRefFieldOwner_6F473170*)
              ((char*)g_pItemSlotHost->GetSlotTable() + 0x80))->QueryField78())
    {
        CFloat currentType190(kCFloatNoInit);
        ((SUnitTypeField190Wrapper*)self)->QueryField190(&currentType190);
        CFloat delta = currentType190 - typeInstance;

        CFloat current(kCFloatNoInit);
        self->m_refFC.GetValue(&current);
        CFloat sum = current + delta;
        self->m_refFC.SetValue(&sum);
    }

    self->RefreshField10C();
}
