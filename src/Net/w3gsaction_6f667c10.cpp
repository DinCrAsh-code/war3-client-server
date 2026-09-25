//============================================================================
//  0x6F667C10 - Probably_W3GS_ActionHandler's case 70. Reads a genuinely
//  UNDECLARED fourth argument straight out of `edx` at entry - the shipped
//  body's very first instructions after the `self` load are `push edi` /
//  `mov edi, edx`, and `edi` (i.e. that edx value) is later used as the
//  `this` pointer for a `FixedPatternKey::Matches()` call
//  (netprovider_keypattern.cpp, `sub_6F6506A0` - null-safe: `!this ||
//  memcmp(...)==0`). This dispatcher's own declared, 3-stack-argument
//  signature (self, lpvReserved, fdwReason - see w3gsactionhandler.cpp) has
//  no slot for it, and nothing in this call tree's own dump says what a
//  real caller would leave there - the same "hidden second argument
//  arriving in edx" defect class docs/notes/thunk-removal-pass.md already
//  documents for a different function. Modelled with the disassembly
//  transcribed literally (an inline `mov` capturing whatever the caller
//  left in edx, exactly where the shipped body reads it) rather than
//  guessing at a real value or silently dropping the check - correct for
//  any caller that reaches this address with the same edx state the
//  shipped body expects, and DIFFERS (not IDENTICAL) because that
//  precondition is exactly what this reconstruction cannot establish from
//  source alone. Not to be hooked without first confirming what a live
//  caller actually leaves in edx here.
//============================================================================

struct FixedPatternKey
{
    int Matches();
};

int __fastcall ParseDwordAction_6F665FE0(void* src, unsigned int declaredLength, unsigned int* out);

int __stdcall W3GSAction_6F667C10(void* self, unsigned int arg4, unsigned int arg8)
{
    void* hiddenEdx;
    __asm { mov hiddenEdx, edx }

    if (*(int*)((char*)self + 0x40) == 0)
        return 1;

    unsigned int parsed;
    if (!ParseDwordAction_6F665FE0((void*)arg4, arg8, &parsed))
        return 1;

    if (*(unsigned int*)((char*)self + 0x58) != parsed || parsed == 0)
    {
        if (!((FixedPatternKey*)hiddenEdx)->Matches())
            return 1;
    }

    *(unsigned int*)((char*)self + 0x58) = 0;
    return 2;
}
