//============================================================================
//  0x6F7B4420 - a tiny counted-slot setter built out of two leaves this
//  build already has under other names: SCountedOwner::AddOneToBoth
//  (0x6F7B43D0, sweep_predicates.cpp) and DecField0x24_6F7B43F0
//  (0x6F7B43F0, trivialaccessors_07.cpp) - re-declared here rather than
//  given a second naked body, the same "one shipped address, one C++ name"
//  rule thunk_inventory.py's own notes state.
//
//  Reached from CUnit::RefreshSpriteVisibility's slot-61 override
//  (unit_refreshspritevisibility.cpp) through SModelInstanceNotify's own
//  +0xB8/+0xBC attachment array (modelinstancenotify.cpp).
//
//  Own translation unit: both leaves are reached with real calls (one of
//  them a tail `jmp`), and putting this in the same TU as either would let
//  /Ob2 inline the two-instruction body away - measured, not assumed: with
//  all three in one file this function's own listing loses the `call`
//  entirely and scores 0/7.
//============================================================================

struct SCountedOwner
{
    SCountedOwner* __thiscall AddOneToBoth();
};

extern void __fastcall DecField0x24_6F7B43F0(void* self);

//----------------------------------------------------------------------------
//  0x6F7B4420 - `value` really does arrive in edx (`mov [eax],edx` after the
//  call, nothing read off the stack), so this is a plain __fastcall(ecx,edx)
//  free function rather than a member of SCountedOwner itself.
//
//  Two calls to `self`, not one to the pointer AddOneToBoth() hands back:
//  the shipped code reloads ecx from esi (its own copy of `self`) ahead of
//  the store rather than reusing eax for the final `jmp`, which is exactly
//  what writing `self->DecField0x24_6F7B43F0()` as its own statement (via
//  the free-function spelling below) produces - eax and the reloaded ecx
//  hold the same address either way, but only this shape gets the extra
//  `mov ecx, esi` the dump has.
//----------------------------------------------------------------------------
void __fastcall SetSlotValueGuarded_6F7B4420(SCountedOwner* self, int value)
{
    *(int*)self->AddOneToBoth() = value;
    DecField0x24_6F7B43F0(self);
}
