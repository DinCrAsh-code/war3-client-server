//============================================================================
//  0x6F4C42E0 - `retn 4`, plain `__thiscall` (no `edx` read at all): compare
//  two objects' own `+0x08` field for equality.  Reached from
//  sub_6F3F8FA0 and from sub_6F45E9D0 (JassInstance::ExecuteOpcodeStream,
//  jassinstance.h - still THUNK), neither in this session's own closure, so
//  the object's real class is not established here; kept a raw offset
//  struct rather than a guessed class, the same choice
//  jassdeclhelpers.cpp's own header comment makes for its own "+0x18"
//  object.  `__thiscall` on a free function is rejected by this compiler
//  (docs/msvc-vc8-idioms.md), hence the one-member wrapper class.
//============================================================================
struct SFieldEightIdentity
{
    char  m_reserved0[8];
    void* m_field8;   // +0x08

    int MatchesField8(SFieldEightIdentity* other);
};

//  Out-of-line (not defined in the class body): an inline definition with
//  no caller in this TU is never emitted at all, and this function needs
//  to compile to a real, externally visible symbol for verify.py to score.
int SFieldEightIdentity::MatchesField8(SFieldEightIdentity* other)
{
    return m_field8 == other->m_field8;
}
