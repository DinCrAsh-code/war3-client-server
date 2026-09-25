//============================================================================
//  CWidget's leaf vtable slots: the ones whose whole body is a constant, a
//  field read, or a single forward.  See widget.h for the declarations and
//  docs/targets/CWidget_CSelectable_CItem__vtables.md for the survey they
//  came out of.
//
//  Almost all of them live in the 0x6F266xxx cluster, which is the same
//  module CSelectable::CSelectable (0x6F266C30) and CAgentWar3::~CAgentWar3
//  (0x6F266D00) sit in - so the shipped source had CWidget and CSelectable
//  in one file, which is also why this reconstruction can put a slice of
//  both in one translation unit.  None of the functions here calls another
//  (every internal call in this group goes through the vtable), so nothing
//  the shipped code kept out of line gets inlined away by grouping them.
//============================================================================
#include "selectable.h"   // CWidget via widget.h, plus slot 102 below
#include "timesyncbounds.h"   // SHandleWithType::SetLow24Bits, slot 87's callee

//  Both from CFloat.h's own extern block; repeated here as the leaves that
//  need them are the only users in this file.
extern const CFloat g_CFloatZero;        // dword_6FAAE470 - encoded 0.0
extern const CFloat g_slopeThreshold1;   // dword_6FAAE4F4 - encoded 1.0

//  flt_6F931ED0 - a plain IEEE single in .rdata, not a CFloat: slot 47
//  `fld`s it straight onto the x87 stack.  Its value is not recoverable
//  from the disassembly, only its type and address.
extern const float g_flt6F931ED0;

//----------------------------------------------------------------------------
//  0x6F2AB0C0 - slot 7.  'w','3','w','+' read low byte first, the same
//  reversed-FourCC spelling agiletype.h's kAgileRootTypeId ('a','g','a','+')
//  already uses.  A bare `mov eax, imm32`, so the literal and not a call to
//  anything that would return the same constant (item.cpp makes the same
//  point about CItem's own 'item').
//----------------------------------------------------------------------------
unsigned int CWidget::GetAgileTypeId() const
{
    return 0x2B773377;
}

//----------------------------------------------------------------------------
//  0x6F2AB070 - slot 22.
//----------------------------------------------------------------------------
const char* CWidget::GetClassName()
{
    return "CWidget";
}

//----------------------------------------------------------------------------
//  0x6F2669C0 / 0x6F2669D0 - slots 43 and 44.  Two separate functions with
//  the same two instructions, at two addresses eight bytes apart: more of
//  the same evidence agentdefaults.cpp records that this image was linked
//  without /OPT:ICF.
//----------------------------------------------------------------------------
int CWidget::GetDefaultCellCentres()
{
    return 1;
}

int CWidget::Method_0xB0()
{
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F2AB780 - slot 45.  A tail jump straight into this object's own slot
//  65, arguments and all: `mov eax,[ecx]` / `mov edx,[eax+104h]` / `jmp edx`
//  with nothing pushed and nothing cleaned, so slot 65 takes no arguments
//  beyond `this` either.
//----------------------------------------------------------------------------
typedef int (__thiscall *Slot0x104Fn)(void* self);

int CWidget::Method_0xB4()
{
    return ((Slot0x104Fn)(*(void***)this)[0x104 / 4])(this);
}

//----------------------------------------------------------------------------
//  0x6F2669E0 - slot 46.  A plain widget has no handle-bearing sub-object.
//----------------------------------------------------------------------------
SHandleWithType* CWidget::GetHandleObject()
{
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F2AB9A0 - slot 47.
//----------------------------------------------------------------------------
float CWidget::Method_0xBC()
{
    return g_flt6F931ED0;
}

//----------------------------------------------------------------------------
//  0x6F266A50 - slot 60.  One stack argument, never read.
//----------------------------------------------------------------------------
int CWidget::QueryVisible(int)
{
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F267160 - slot 67.  Returns the out-pointer in eax, which is the
//  `mov eax,[esp+arg_0]` the shipped code opens with.  Nothing reads it
//  back at any call site in these three trees, and this body would leave
//  it there either way - but CUnit's override of the same slot
//  (0x6F28B330, unit_thunks.cpp) does *not*: declared void it puts the
//  field in eax and the out pointer in ecx, the opposite of the shipped
//  register assignment.  So the slot says `int*` and both bodies return
//  it, which costs this one nothing.
//----------------------------------------------------------------------------
int* CWidget::Method_0x10C(int* out)
{
    *out = -1;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F266A60 / 0x6F266A70 - slots 69 and 71.
//----------------------------------------------------------------------------
int CWidget::Method_0x114()
{
    return 0;
}

int CWidget::Method_0x11C()
{
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F266B60 / 0x6F266B70 / 0x6F266B80 / 0x6F266B90 / 0x6F266BA0 - slots
//  70, 74, 75, 76 and 77.  Five copies of "hand back encoded zero", four
//  with one argument and one (slot 74) with two.  The returned pointer is
//  the caller's own buffer, the same class-return-by-pointer shape
//  CFloat's operator+ uses.
//----------------------------------------------------------------------------
CFloat* CWidget::Method_0x118(CFloat* out)
{
    *out = g_CFloatZero;
    return out;
}

//  Returned by value rather than through a named out-pointer: the slot's
//  signature is CItem's (item.h), and a one-dword class returned by value
//  is the same (retbuf, arg) / `retn 8` ABI this leaf has.
CFloat CWidget::AddLife(const CFloat*)
{
    return g_CFloatZero;
}

CFloat* CWidget::GetLife(CFloat* out)
{
    *out = g_CFloatZero;
    return out;
}

CFloat* CWidget::GetMaxLife(CFloat* out)
{
    *out = g_CFloatZero;
    return out;
}

CFloat* CWidget::GetMinLife(CFloat* out)
{
    *out = g_CFloatZero;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F266A90 - slot 78.  Forward to slot 76 and return the same buffer,
//  not slot 76's own return value: the shipped code keeps the argument in
//  esi across the call and reloads eax from it afterwards, which is what
//  ignoring the callee's result and returning the parameter looks like.
//----------------------------------------------------------------------------
typedef CFloat* (__thiscall *Slot0x130Fn)(void* self, CFloat* out);

CFloat* CWidget::Method_0x138(CFloat* out)
{
    ((Slot0x130Fn)(*(void***)this)[0x130 / 4])(this, out);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F266BB0 - slot 79.  Ask slot 75 for a CFloat into a scratch slot and
//  answer "is it at or below zero".
//
//  Two things in the shipped code pin the spelling down:
//
//   * `fld [eax]` (the callee's answer) *then* `fld g_CFloatZero`, followed
//     by `test ah, 1` - the C0-only mask MSVC uses for `<=`/`>=` with the
//     left operand loaded first.  `zero < value` compiles to the two loads
//     in the other order and `test ah, 41h`.
//   * `mov eax,1` / `xor eax,eax` (whole dwords) followed by `movzx eax,al`
//     on both paths - an int 0/1 narrowed to a byte and widened again, i.e.
//     a bool conversion applied to an int result.  A bool computed directly
//     gives `mov al,1` and no `movzx` before it.
//
//  The scratch is raw int bits, not a CFloat: CFloat's default constructor
//  zeroes, and the shipped code leaves the slot uninitialised because the
//  call writes it before anything reads it - the same trade widget.cpp's
//  MoveTo already makes for its own facing scratch.
//----------------------------------------------------------------------------
typedef CFloat* (__thiscall *Slot0x12CFn)(void* self, CFloat* out);

int CWidget::Method_0x13C()
{
    int scratch;
    CFloat* value =
        ((Slot0x12CFn)(*(void***)this)[0x12C / 4])(this, (CFloat*)&scratch);

    int atOrBelowZero;
    if (*(const float*)value <= *(const float*)&g_CFloatZero)
        atOrBelowZero = 1;
    else
        atOrBelowZero = 0;
    return (unsigned char)atOrBelowZero;
}

//----------------------------------------------------------------------------
//  0x6F266AD0 / 0x6F266B00 / 0x6F266B10 / 0x6F266B30 / 0x6F266B50 - slots
//  86, 88, 89, 92 and 95: five more fixed answers.
//----------------------------------------------------------------------------
int CWidget::GetPlacementMask()
{
    return 0;
}

int CWidget::Method_0x160()
{
    return 10;
}

int CWidget::Method_0x164()
{
    return 2;
}

int CWidget::GetPositionNotifyHandle()
{
    return -1;
}

unsigned int CWidget::Method_0x17C()
{
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F266AE0 - slot 87.  Ask slot 46 for the handle-bearing sub-object and
//  push the argument's low 24 bits into it.
//
//  `retn 8` for one used argument: the second is never read.  The shipped
//  `push eax` before `call eax` is *not* an argument to slot 46 - slot 46
//  takes none and cleans none (0x6F2669E0's own `retn`) - it is
//  SetLow24Bits' argument, pushed early and left sitting across the virtual
//  call for the callee after it to clean.  Reading it the other way round
//  is what makes the two functions' `retn` counts disagree, which is
//  exactly the class of defect tools/abi_audit.py exists to catch.
//----------------------------------------------------------------------------
typedef void* (__thiscall *GetHandleObjectFn)(void* self);

void CWidget::Method_0x15C(unsigned int bits, int)
{
    //  One expression, not a named `sub` local: with the call and the
    //  member call written apart, MSVC schedules the argument push *after*
    //  the virtual call; as one expression it pushes the argument first and
    //  leaves it across the call, which is the shipped order.
    ((SHandleWithType*)((GetHandleObjectFn)(*(void***)this)[0xB8 / 4])(this))
        ->SetLow24Bits(bits);
}

//----------------------------------------------------------------------------
//  0x6F266BF0 - slot 90.  Encoded 1.0, through the caller's buffer.
//----------------------------------------------------------------------------
CFloat* CWidget::GetPlacementRadius(CFloat* out)
{
    *out = g_slopeThreshold1;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F266C10 - slot 94.
//----------------------------------------------------------------------------
unsigned int* CWidget::Method_0x178(unsigned int* out)
{
    *out = m_reserved40;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F266A30 - slot 58.  `fldz`, nothing else.
//----------------------------------------------------------------------------
float CWidget::Method_0xE8()
{
    return 0.0f;
}


//============================================================================
//  The empty slots.
//
//  Nine of CWidget's slots and one of CSelectable's have a body that is a
//  bare `retn` in the shipped image - IDA's nullsub_1192..nullsub_1201, ten
//  consecutive empty functions in this same 0x6F266xxx cluster.  They are
//  reconstructions, not placeholders: an empty virtual *is* the shipped
//  body, and a class that declares the slot without defining it cannot emit
//  its vtable at all.
//
//  What matters here is the argument count, not the body.  Every one of
//  these was a `__declspec(naked)` redirect until now, and a naked thunk
//  has no `ret` of its own - which is exactly why tools/abi_audit.py filed
//  all ten under "NOT AUDITED - no `ret` to compare" and could not see that
//  four of them were declared `void()` against a shipped `retn 4`.  Under
//  a callee-cleanup convention that is a live stack-corruption crash on the
//  first call, and the score cannot see it either.  The counts below are
//  read straight off each dump's `retn`.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F2669F0 / 0x6F266A00 / 0x6F266A10 / 0x6F266A20 - slots 48-51, four
//  consecutive one-argument slots that do nothing here.
//
//  CUnit is the only class in the image that overrides them
//  (0x6F28B0F0/0x6F28B100/0x6F28B110/0x6F28B120), and all four of its
//  overrides have the same shape: `add ecx, 164h` to reach a sub-object,
//  resolve a handle out of its +0x08/+0x0C pair through LookupHandle
//  (0x6F03FA30, handle.cpp), then apply the single argument to the
//  resolved target - slot 49 stores a dword at its +0xBC, slots 50 and 51
//  write two adjacent 4-bit fields of the dword at its +0xD8 (bits 8-11
//  and 12-15), and slot 48 compares a converted value against
//  flt_6FAAE47C first.  That is enough to fix the argument count and the
//  fact that the argument is a value rather than a pointer; it is *not*
//  enough to say what the value means, so these keep slot-indexed names
//  rather than invented ones.
//----------------------------------------------------------------------------
void CWidget::Method_0xC0(int) {}
void CWidget::Method_0xC4(int) {}
void CWidget::Method_0xC8(int) {}
void CWidget::Method_0xCC(int) {}

//----------------------------------------------------------------------------
//  0x6F266A80 - slot 73.  CItem's override (item_valueset.cpp) publishes a
//  clamped value through FloatMiniB and may call OnValueFloorReached; a
//  plain widget tracks no value and does nothing.
//----------------------------------------------------------------------------
void CWidget::SetLife(const CFloat*) {}

//----------------------------------------------------------------------------
//  0x6F266AB0 / 0x6F266AC0 - slots 80 and 81, the {min, max} half of the
//  same tracked value.  CItem's overrides (item_vtable.cpp) each write one
//  of FloatMiniB's two fields and re-write the other with the value it
//  already had; a plain widget has no FloatMiniB, so both are empty.
//----------------------------------------------------------------------------
void CWidget::SetMinLife(const CFloat*) {}
void CWidget::SetMaxLife(const CFloat*) {}

//----------------------------------------------------------------------------
//  0x6F266B20 - slot 91.  CItem's override (item_radius.cpp) asks slot 90
//  for its placement radius, clamps it up to 1.0 and pushes it into
//  m_position; a plain widget has no position record to push it into.
//----------------------------------------------------------------------------
void CWidget::RefreshPlacementRadius() {}

//----------------------------------------------------------------------------
//  0x6F266B40 - slot 93, the setter whose getter one slot up (0x6F266B30,
//  slot 92) returns the fixed -1: a plain widget has no notify handle, so
//  there is nothing to store and nothing to hand back.
//----------------------------------------------------------------------------
void CWidget::SetPositionNotifyHandle(int) {}

//----------------------------------------------------------------------------
//  0x6F266CF0 - CSelectable's slot 102, and the one function in this file
//  that is not CWidget's.  It sits at the top of the same cluster (twelve
//  bytes past CSelectable::CSelectable at 0x6F266C30), which is the same
//  evidence this file's header comment already gives for the two classes
//  having shipped in one source file.
//
//  CItem's override (item_selectionrefresh.cpp) walks all sixteen players'
//  selection sets and refreshes the overlay; a CSelectable that is not an
//  item has nothing to refresh.
//----------------------------------------------------------------------------
void CSelectable::RefreshSelectionState() {}
