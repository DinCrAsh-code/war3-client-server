//============================================================================
//  `??_7Game@NetClient@Net@@6B@_0` @ 0x6F9717E8 - IDA's own disambiguation
//  suffix for a second RTTI symbol demangling to the same qualified name as
//  `??_7Game@NetClient@Net@@6B@` (0x6F971820, netclientgame_dtor.cpp).
//
//  Correction to this wave's own claim: vtable_classes.json originally
//  recorded this table (and its sibling at 0x6F971820) as one slot each,
//  from a partial `ida_query vtable` query that only surfaced method_index
//  0. The real table has 13 slots (re-queried this session, see
//  vtable_classes.json's own updated entry and
//  docs/targets/vtables/Net_NetClient_networking_family.md) - this file
//  reconstructs slot 0 only; slots 1-12 remain unaddressed for a future
//  wave.
//
//  Slot 0 itself, 0x6F6EE390, is a pure adjustor-forwarder: it re-reads
//  `this`'s own (currently-active, i.e. this same 13-slot) vtable pointer
//  and tail-jumps through slot 8 of THAT SAME table (0x676E00,
//  method_index 8, +0x20 - not through the sibling 1-slot table at
//  0x6F971820, which has no slot 8 to reach). No `this` adjustment, no
//  register or stack touched beyond `eax` - every argument the real caller
//  pushed, and `this` itself in `ecx`, reach 0x6F676E00 completely
//  unmodified. Reconstructed as the literal 3-instruction indirect jump
//  rather than a typed C++ call: this is not a fixed-address thunk into
//  Game.dll (thunk_abi_audit.py's own pattern), and guessing a typed
//  signature for slot 8 here would be exactly the risk
//  vtable_dispatch_audit.py exists to catch (CLAUDE.md's own worked
//  example) - the raw register-preserving jump carries every argument
//  correctly no matter what 0x6F676E00's real signature turns out to be.
//============================================================================
__declspec(naked) void __fastcall NetClientGame0_Slot0Forward(void* self)
{
    __asm { mov eax, [ecx] }
    __asm { mov eax, [eax+20h] }
    __asm { jmp eax }
}
