//============================================================================
//  0x6F00D9E0 - "show or hide the visual attachment in this slot".
//
//  Was a `mov eax,<address> / jmp eax` redirect; it is six instructions -
//  fetch the large Storm singleton and poke bit 0x20 of its indexed
//  attachment record.  Both halves were already reconstructed here
//  (GetStormSingletonA, 0x6F01F5A0, lazysingleton.cpp; and
//  SIndexedArrayFlags::SetOrClearBit0x20, 0x6F741B00,
//  misc_leaves_batch3.cpp), so nothing about this one was ever out of
//  reach.
//
//  Both arguments are pushed *before* the singleton fetch runs (the shipped
//  `push edx` / `push ecx` / `call`), which is exactly the order MSVC
//  evaluates a call whose receiver is itself a call: arguments first, then
//  the object expression.  Writing it as `SIndexedArrayFlags* s =
//  GetStormSingletonA(); s->SetOrClearBit0x20(slot, on);` reverses that and
//  costs the two pushes their position.
//
//  Own translation unit: two real calls out of it, and its four callers
//  (selectable_vtable.cpp, item_vtable2.cpp, unit_attachments.cpp) must
//  reach it with real calls of their own.
//============================================================================

struct SStormSingletonA;
SStormSingletonA* __fastcall GetStormSingletonA();

//  0x6F741B00 - misc_leaves_batch3.cpp.  Declared, not re-thunked.
struct SIndexedArrayFlags
{
    void* __thiscall SetOrClearBit0x20(int index, int setFlag);
};

void __fastcall AttachmentSetVisible(unsigned int slot, int on)
{
    ((SIndexedArrayFlags*)GetStormSingletonA())->SetOrClearBit0x20((int)slot, on);
}
