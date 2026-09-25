//============================================================================
//  0x6F3309F0 - CSelectable's vtable slot 103 (+0x19C).
//
//  Was a `mov eax,<address> / jmp eax` redirect; it is two instructions.
//  The whole slot is "play the InterfaceClick sound": it discards its own
//  `this` (ecx is overwritten before anything reads it) and tail-jumps into
//  the sound dispatcher with the cue name in its place.
//
//  The dispatcher (0x6F32F980) is the one thing here that stays a thunk: it
//  reads two rendering-side globals, and on the "sounds are off" path
//  reaches a logging sink through a vtable slot with a literal message
//  string ("master sounds disabled, aborting") - three subsystems this
//  repo has no other reason to enter.  It is `retn` with nothing on the
//  stack, so a __fastcall declaration taking the name in ecx pushes exactly
//  the nothing it cleans; that is what tools/thunk_abi_audit.py checks.
//
//  The gate's own return value is whatever the dispatcher leaves in eax.
//  Nothing reads it - selectable_vtable.cpp returns it straight out of the
//  slot - but it has to be declared as a value return, because that is what
//  makes MSVC emit the tail jump rather than a call and a `xor eax,eax`.
//
//  Own translation unit: it reaches the dispatcher with a real tail jump.
//============================================================================

//  0x6F32F980 - shipped code; see above.
int __fastcall PlayInterfaceSound(const char* cue);

__declspec(naked) int __fastcall PlayInterfaceSound(const char*)
{
    __asm
    {
        mov     eax, 06F32F980h
        jmp     eax
    }
}

int __fastcall SelectableSlot0x19CGate(void* /*self*/)
{
    return PlayInterfaceSound("InterfaceClick");
}
