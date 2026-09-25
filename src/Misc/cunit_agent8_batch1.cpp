//============================================================================
//  A batch of small, mostly self-contained leaves with no owning class
//  otherwise identified - same convention as misc_leaves_batch3/4/5.cpp -
//  reached from CUnit's own vtable BFS closure (cunit_agent8_worklist.md).
//============================================================================
#include "refcnt.h"
#include "agent.h"
#include "agiletype.h"
#include "storm.h"
#include "game.h"

//  0x6F03B940 - two instructions: reduce some other, unidentified
//  TRefCnt-derived object back to a bare TRefCnt by explicitly invoking the
//  (empty) base destructor - the same idiom misc_leaves_batch5.cpp's
//  SNotifyTargetLostPayload::ResetToTRefCnt already documents for a
//  different address.
struct SUnidentifiedRefCntHolder6F03B940 : public TRefCnt
{
    void __thiscall ResetToTRefCnt();
};
void __thiscall SUnidentifiedRefCntHolder6F03B940::ResetToTRefCnt()
{
    this->TRefCnt::~TRefCnt();
}

//  0x6F29F000 - populate an out buffer from an object's own cached fourCC
//  field, deferring to UnitTypeField18C_6F29C010 (0x6F29C010, the unit-type
//  registry's own +0x18C CFloat probe, Unit/unittypequeries.cpp - now a
//  real reconstruction rather than the naked `UTQ_6F29C010` placeholder
//  this comment used to name) and returning the same `out` pointer handed
//  in. `this` is not identified beyond the one field this slot reads.
//
//  Re-declared here (rather than `#include "unittypequeries.h"`) to match
//  that file's own signature exactly - `CFloat* __fastcall(CFloat*,
//  unsigned int)` - so this call binds onto that definition instead of
//  inventing a second symbol (tools/link_check.py's first defect class).
class CFloat;
extern CFloat* __fastcall UnitTypeField18C_6F29C010(CFloat* out, unsigned int fourCC);

struct SFourCCHolder030
{
    void* __thiscall PopulateTypeInstance(void* out);

    char         m_reserved00[0x30];
    unsigned int m_fourCC;   // +0x30
};

void* __thiscall SFourCCHolder030::PopulateTypeInstance(void* out)
{
    UnitTypeField18C_6F29C010((CFloat*)out, m_fourCC);
    return out;
}

//  0x6F29CA60 - the unit-type registry's own g_unitTypeTable.Ptr(id.Hash(),
//  id) lookup (unittypetable.h/unittypequeries.cpp already establish both
//  the table and the idiom), returning the found record's +0x228 field.
//  "the second, smaller registry" unit_typeinstancepopulate.cpp's own
//  header comment names as dword_6FAB445C without identifying it further -
//  this agent's own worklist slice carries the real dump for it, so the
//  identification lives here now.  Currently declared as a naked
//  `UTQ_THUNK` (UTQ_6F29CA60, unit_typeinstancepopulate.cpp) rather than
//  reconstructed; that thunk is left as-is (its own translation unit is
//  out of this agent's scope to edit).  Nested `Ptr(id.Hash(), id)`, not
//  the staged form - same reasoning unittypequeries.cpp's own header gives.
#include "unittypetable.h"

void* __fastcall LookupSecondRegistryField0x228(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field228 : 0;
}

//  0x6F3331F0 - tell the game UI about `this`, if there is one.  Same
//  GetGameUI(0, 0) shape gameui.h/item_placegate.cpp already establish;
//  the callee (0x6F2FBA90) is not otherwise identified.
struct SGameUI
{
    void NotifySelf(void* self);   // 0x6F2FBA90 - not this agent's address
};
extern SGameUI* __fastcall GetGameUI(int create, int reset);

struct SNotifiesGameUI6F3331F0
{
    void NotifyGameUIIfPresent();
};

void SNotifiesGameUI6F3331F0::NotifyGameUIIfPresent()
{
    SGameUI* ui = GetGameUI(0, 0);
    if (ui)
        ui->NotifySelf(this);
}

//  0x6F476250 - resolve this holder's own cached {handle, typeTag} pair
//  through the global handle table (LookupHandle, handle.cpp), then
//  forward the resolved object's own +0xA8 field and a halved caller
//  argument to an unidentified helper (0x6F49DFF0). `this` has a cached
//  handle at +0x08 and a handle-table type tag at +0x0C; neither the
//  resolved object's real type nor the helper is established beyond that.
class CHandleObject;
extern CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag);

struct STargetOf6F49DFF0
{
    int Forward(unsigned int halved);   // 0x6F49DFF0 - not this agent's address
};

struct SCachedHandleHolder08
{
    int ForwardHalved(unsigned int value);

    char         m_reserved00[0x08];
    unsigned int m_handle;    // +0x08
    int          m_typeTag;   // +0x0C
};

int SCachedHandleHolder08::ForwardHalved(unsigned int value)
{
    unsigned int halved = value >> 1;
    CHandleObject* obj = LookupHandle(m_handle, m_typeTag);
    STargetOf6F49DFF0* target = *(STargetOf6F49DFF0**)((char*)obj + 0xA8);
    return target->Forward(halved);
}

//  0x6F62A580 - resolve this holder's own +0x08 object and, if present,
//  forward it and the caller's argument to an unidentified helper
//  (0x6F62A1C0); returns 0 with no call at all when +0x08 is null.
struct STargetOf6F62A1C0
{
    int Forward(int value, int zero);   // 0x6F62A1C0 - not this agent's address
};

struct SOptionalHolder08
{
    int ForwardIfPresent(int value);

    char             m_reserved00[0x08];
    STargetOf6F62A1C0* m_pTarget;   // +0x08
};

int SOptionalHolder08::ForwardIfPresent(int value)
{
    int result = 0;
    STargetOf6F62A1C0* target = m_pTarget;
    if (target)
        result = target->Forward(value, 0);
    return result;
}

//  0x6F282880 - free two inline-array overflow buffers if either grew past
//  its 48 (0x30) built-in slots. `this` is some object BFS reached from
//  CUnit's own vtable (unidentified beyond that - the two count/pointer
//  pairs are +0xDC/+0xE4 and +0x10/+0x18, too far apart to be one small
//  struct's own two members, so no struct is invented here; see
//  CLAUDE.md's own rule against inventing fields the call tree doesn't
//  need). Storm_403 (SMemFree) is tagged with the same string both times -
//  IDA's own "aPavcunit" label, read back as its literal content
//  "Pavcunit" (a memory-tracking tag, not identified further).

struct SFreesOverflowArrays6F282880
{
    void FreeOverflowArraysIfGrown();
};

void SFreesOverflowArrays6F282880::FreeOverflowArraysIfGrown()
{
    void* self = this;
    if (*(unsigned int*)((char*)self + 0xDC) > 0x30)
        SMemFree(*(void**)((char*)self + 0xE4), "Pavcunit", -2, 0);
    if (*(unsigned int*)((char*)self + 0x10) > 0x30)
        SMemFree(*(void**)((char*)self + 0x18), "Pavcunit", -2, 0);
}

//----------------------------------------------------------------------------
//  Link-completion thunks - each of these addresses was declared (not
//  defined) elsewhere in this file because comments record it as another
//  parallel agent's own worklist target, but the two agents independently
//  invented different class/struct names for the same shared address, so
//  the declaring extern reference never actually resolves against the
//  sibling's real (differently-named) reconstruction. Defining a plain
//  redirect thunk here - the same `mov eax,<addr>/jmp eax` idiom this
//  repo already uses for any out-of-scope callee - makes this TU link
//  without re-claiming the address (no funcmap.py row points at these).
//----------------------------------------------------------------------------
__declspec(naked) int STargetOf6F49DFF0::Forward(unsigned int halved)
{
    __asm { mov eax, 06F49DFF0h }
    __asm { jmp eax }
}

__declspec(naked) int STargetOf6F62A1C0::Forward(int value, int zero)
{
    __asm { mov eax, 06F62A1C0h }
    __asm { jmp eax }
}

__declspec(naked) void SGameUI::NotifySelf(void* self)
{
    __asm { mov eax, 06F2FBA90h }
    __asm { jmp eax }
}
