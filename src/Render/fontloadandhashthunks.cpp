//============================================================================
//  0x6F553CC0 and 0x6F685B40, thunked rather than reconstructed this
//  session - a deliberate judgement call, not a shortcut past tedium (see
//  CLAUDE.md's "don't thunk something just because it's tedious"):
//
//  * 0x6F553CC0 (130 instructions) is a font-record loader driving four
//    further, not-yet-reconstructed callees (sub_6F5559A0, sub_6F553B60,
//    sub_6F553AA0 - called three times with three different embedded font
//    file names - and sub_6F553C50) through a class this call tree never
//    otherwise touches (fields only at +0, +4, +8, growth logic borrowed
//    from none of this repo's own established containers).  Getting its
//    *shape* plausible was reachable; getting it byte-for-byte would mean
//    first reconstructing all four callees cold, well beyond this batch's
//    own 43.
//  * 0x6F685B40 (61 instructions) drives four more not-yet-reconstructed
//    callees (sub_6F685AD0/A90/CA0/C00) through what looks like a
//    checksum/hash accumulator over 0x34 bytes, called once per byte - the
//    same "needs its whole closure reconstructed first" shape.
//
//  Both are reached from gamearchivehwcheck.cpp (0x6F009950, one of this
//  batch's own 43); their retn sizes below are read straight out of their
//  own dumps (thunk_abi_audit.py's own preferred source), not inferred.
//============================================================================

//  0x6F553CC0 - retn 4 (thiscall + edx + one stack arg, `retn 4` in its own
//  dump).
__declspec(naked) int __fastcall LoadFontRecordsThunk(void* /*ecx*/, void* /*edx*/, int /*stackArg*/)
{
    __asm { mov eax, 06F553CC0h }
    __asm { jmp eax }
}

//  0x6F685B40 - retn 8 (thiscall + edx + two stack args, `retn 8` in its
//  own dump).
__declspec(naked) void __fastcall HashDeviceStringThunk(void* /*ecx*/, void* /*edx*/, void*, void*)
{
    __asm { mov eax, 06F685B40h }
    __asm { jmp eax }
}
