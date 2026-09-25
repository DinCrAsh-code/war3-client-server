//============================================================================
//  0x6F009AE0 - `retn 4`: `ecx = &a`, one stack arg `&b` - `__thiscall`
//  shaped (`ecx` plus a stack argument; `__fastcall` would put `&b` in
//  `edx` instead, which this compiler does and the dump does not), i.e. a
//  member/`this`-style call rather than a two-pointer free function.  No
//  caller in this session's own batch identifies the owning class, and
//  MSVC refuses `__thiscall` on a free function ("can only be used on
//  native member functions") with no class to hang it off, so it is
//  declared `__fastcall` here instead - a real, permanent ABI gap (`&b`
//  lands in `edx` rather than on the stack), not register-allocation noise.
//
//  A two-operand `fucompp`/`fnstsw ax`/`test ah,44h`/`jp` compare (mask
//  `C3|C2` - equal or unordered) rather than the `test ah,41h` two-operand
//  shape docs/msvc-vc8-idioms.md's own Floating point section already
//  covers: this compiler's own `!=` spelling produces `test ah,68h`
//  instead (mask `C3|C2|C0`, one extra bit), a real, unresolved
//  mask/spelling gap - the JASS VM idiom doc's four-row two-operand table
//  does not have a `!=` row, and none of `==`/`<`/`>`/`<=`/`>=` was tried
//  against this exact mask before time ran out this session.
//============================================================================

int __fastcall FloatValuesDiffer(const float* a, const float* b)
{
    return *a != *b;
}
