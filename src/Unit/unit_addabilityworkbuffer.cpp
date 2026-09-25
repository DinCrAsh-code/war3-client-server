//============================================================================
//  0x6F011E20 - CUnit slot 107 (Method_0x1AC/the ability-registration
//  initialiser, still thunked) closure leaf, cunit_vtable_closure_worklist
//  batch agent-cunit-batch-17.
//
//  Zeroes a run of fields on a large (>=0x387D-byte) work object: this is
//  the reset step for the same 0x3884-byte local buffer sub_6F01DC90
//  (`asm/sub_6F01D9A0_...`, still blocked on this build's fixed `/GS-`
//  plain-stack-cookie ceiling per docs/msvc-vc8-idioms.md) allocates on its
//  own stack frame with `mov eax, 3884h / call __alloca_probe` and hands
//  this function by pointer.  Nothing else in this closure names the
//  object's real type or its full size, so only the touched offsets are
//  given names; everything else is an untouched reserved gap per CLAUDE.md.
//============================================================================
#pragma pack(push, 1)
struct SUnitAddAbilityWorkBuffer
{
    char  m_reserved000[0x408];
    int   m_int408;
    int   m_int40C;
    int   m_int410;
    int   m_int414;
    int   m_int418;
    int   m_int41C;
    float m_float420;
    float m_float424;
    float m_float428;
    float m_float42C;
    float m_float430;
    float m_float434;
    float m_float438;
    float m_float43C;
    char  m_reserved440[0x3850 - 0x440];
    int   m_int3850;
    char  m_reserved3854[0x3879 - 0x3854];
    int   m_int3879;

    void __thiscall Reset();
};
#pragma pack(pop)

void __thiscall SUnitAddAbilityWorkBuffer::Reset()
{
    m_int408 = 0;
    m_int40C = 0;
    m_int410 = 0;
    m_int414 = 0;
    m_int418 = 0;
    m_int41C = 0;

    m_float420 = 0.0f;
    m_float424 = 0.0f;
    m_float428 = 0.0f;
    m_float42C = 0.0f;
    m_float430 = 0.0f;
    m_float434 = 0.0f;
    m_float438 = 0.0f;
    m_float43C = 0.0f;

    m_int3850 = 0;
    m_int3879 = 0;
}
