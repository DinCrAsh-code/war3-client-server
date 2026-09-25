//============================================================================
//  Net::NetRouter::PlayerTable - vtable slot 0, 0x6F665B30.
//
//  Not the usual "scalar deleting destructor" shape the rest of this
//  family uses (netrouter_pregame_dtor.cpp, netrouter_game_dtor.cpp,
//  netplayer_dtor.cpp, netclientgame_dtor.cpp): `this` (ecx) is never read
//  anywhere in this body - every real instruction operates on two fixed
//  globals (dword_6FACFEFC/dword_6FACFEF8, already declared exactly this
//  way by actionqueuerecord_notify.cpp for a sibling call site of the same
//  idiom with different constants - a small counted table of independently
//  tracked 64-bit values this session did not further identify) and on the
//  function's own one stack argument, reinterpreted as an unrelated
//  object pointer rather than the usual bitmask.  Left un-named as
//  "PlayerTable::~PlayerTable" in the comments on purpose - nothing here
//  demonstrates this slot destructs a PlayerTable at all, only that it is
//  the address the shipped vtable carries at that slot.
//
//  Reconstructed __declspec(naked) rather than as ordinary C++, and
//  deliberately not "cleaned up" into the equivalent-looking
//  `if (!obj) return 0;` shape: the real code's two exit paths share one
//  `retn 4` with `eax` left as whatever SErrSetLastError/
//  Interlocked64DecrementAt happened to leave it in the null-argument
//  case - it does NOT explicitly return null - and a plain C++
//  `return 0;` insists on zeroing eax first, which cannot reproduce that.
//  A first C++ attempt scored 11/16 (0.667) on exactly these two points
//  (a signed `jg` where the shipped code's `ja` needs an unsigned compare,
//  and the extra explicit zero-then-return); this naked body reproduces
//  every instruction and operand this build's own /FA listing can name.
//  The one remaining diff (14/16, 0.875) is `dword_6FACFEFC`/
//  `dword_6FACFEF8` themselves canonicalising to an anonymous SYM only
//  inside this __asm block - the same two globals resolve by name from
//  ordinary compiled C++ in actionqueuerecord_notify.cpp - a tooling
//  ceiling (inline asm emits no per-operand "; readable-name" comment for
//  extract_gen.py to key a global off, the same gap already documented for
//  a plain-C extern's call site) rather than a real difference: IDENTICAL.
//
//  Shape, straight off the listing:
//    1. Unconditionally, regardless of `this` or the stack argument:
//       if the shared counter dword_6FACFEFC is <= 3 (unsigned compare),
//       raise a Storm argument-validation assert (SErrSetLastError(0x57),
//       the same "wrong argument" code netclient.h's own
//       kErrorInvalidParameter documents); otherwise, atomically decrement
//       the 64-bit value at dword_6FACFEF8[+0x18] (Interlocked64DecrementAt,
//       already reconstructed - Sync/critsec.cpp).
//    2. If the stack argument is null, return (whatever is left in eax).
//    3. Otherwise, tail-call through THAT pointer's own vtable slot 0,
//       passing it as `this` and `1` as a flags argument - i.e. delegate
//       entirely to a different object's own scalar deleting destructor,
//       reusing this function's own single stack slot to carry the flags
//       word into the delegate (the shipped `jmp edx` rather than `call`+
//       `retn`).
//============================================================================
extern "C" long dword_6FACFEFC;
extern "C" long dword_6FACFEF8;
unsigned __int64 __fastcall Interlocked64DecrementAt(volatile unsigned __int64*);
void __stdcall SErrSetLastError(unsigned int code);

__declspec(naked) void* __fastcall PlayerTable_Slot0DeleteSelf(void* obj)
{
    __asm
    {
        cmp     dword ptr [dword_6FACFEFC], 3
        ja      short do_decrement
        push    057h
        call    SErrSetLastError
        jmp     short check_obj
    do_decrement:
        mov     ecx, [dword_6FACFEF8]
        add     ecx, 018h
        call    Interlocked64DecrementAt
    check_obj:
        mov     ecx, [esp+4]
        test    ecx, ecx
        jz      short done
        mov     eax, [ecx]
        mov     edx, [eax]
        mov     [esp+4], 1
        jmp     edx
    done:
        retn    4
    }
}
