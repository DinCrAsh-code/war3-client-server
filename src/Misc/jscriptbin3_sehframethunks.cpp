//============================================================================
//  jscript-bin3's own worklist - functions whose prologue is the
//  `push -1 / push offset SEH_<addr> / mov eax, fs:0` cookie-protected SEH
//  frame docs/msvc-vc8-idioms.md's own "An __except_handler4-shaped frame
//  this toolchain cannot reproduce" section documents: this repo's fixed
//  `/GS- /EHs-c-` toolchain cannot emit this exact frame shape (the
//  personality-routine trampoline the dump appends after the function's own
//  `retn`), so even a faithful transcription of the body scores low on
//  furniture alone. Left self-redirect thunks rather than reconstructed and
//  scored artificially low; each comment below is what this session could
//  read of the body anyway; none is game code new to this closure's own
//  common.j/blizzard.j scope.
//============================================================================

#define SELF_REDIRECT(addr)          \
    {                                 \
        __asm { mov eax, addr }      \
        __asm { jmp eax }             \
    }

//  0x6F457E70 - `retn 0` (__thiscall).  SEH-framed; body not read past the
//  frame furniture given the ceiling above.
__declspec(naked) void SehFramedMethod457E70(void* self)
    SELF_REDIRECT(06F457E70h)

//  0x6F4623E0 - `retn 0` (__thiscall).  SEH-framed; body not read past the
//  frame furniture given the ceiling above.
__declspec(naked) void SehFramedMethod4623E0(void* self)
    SELF_REDIRECT(06F4623E0h)

//  0x6F70FD90 - `retn 0` (__thiscall).  SEH-framed.  References
//  "Units\\UpgradeData"/"upgradeid"/"height" (the same CObjectDataTable
//  upgrade-sheet lookup docs/msvc-vc8-idioms.md's RaceNameLookup note
//  already covers) - out of this closure's own .j-loading scope regardless
//  of the frame ceiling.
__declspec(naked) void GetUpgradeDataFieldByName(void* self)
    SELF_REDIRECT(06F70FD90h)

//  0x6F71C410 - `retn 0` (__thiscall).  SEH-framed; references a build-path
//  string ("e:\\Drive1\\temp\\buildwar3x\\War3\\Sou...") consistent with a
//  version/build-info accessor - out of this closure's own scope regardless
//  of the frame ceiling.
__declspec(naked) void GetBuildVersionString(void* self)
    SELF_REDIRECT(06F71C410h)

//  0x6F728290 - `retn 4` (__fastcall ecx/edx, 1 stack arg).  Not SEH-framed
//  itself, but a large (158-instruction) `|`/`<...>`-tag string-format
//  expander (a jump table over ~44 escape codes, %/+ numeric-format flags,
//  a float-to-string path through Storm_578) genuinely out of this round's
//  own budget - unrelated to the common.j/blizzard.j loading family the
//  rest of this worklist covers.
__declspec(naked) void ExpandTaggedFormatString(const char*, char*, int)
    SELF_REDIRECT(06F728290h)

#undef SELF_REDIRECT
