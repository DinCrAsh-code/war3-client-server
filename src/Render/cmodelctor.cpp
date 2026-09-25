//============================================================================
//  0x6F4FB580 - CModel::CModel(int).  Zero the object from +0x04 to +0x60,
//  set twelve floats from +0x64 to +0x90 (the first, +0x64, to 1.0f via the
//  same `fld1` the vtable-stamp block never pops; the rest to 0.0f), store
//  the one stack argument at +0x94, and zero +0x98/+0x9C.  `retn 4`.
//
//  0x6F4FF760 - CModelComplex_::CModelComplex_().  Base-constructs a CModel
//  with the literal `0x10`, re-stamps its own derived vtable over CModel's,
//  sets +0xA0 to -1 and zeroes every dword from +0xA4 to +0x110.  `retn 0`.
//
//  Neither class has any other member reconstructed in this batch, and
//  neither has a committed vtable in docs/targets/vtables - both are
//  declared with the minimal two virtuals every other local RTTI stand-in
//  in this repo uses purely to make the compiler stamp the real
//  `??_7CModel@@6B@`/`??_7CModelComplex_@@6B@` symbols the two constructors
//  need (Misc/ccamera.cpp's own CBaseManaged is the same trick); neither
//  class is otherwise asserted to have exactly two virtual slots.
//  ctor_vtable_audit.py has no worklist entry for either class, so both
//  constructors report UNKNOWN rather than a finished/unfinished verdict -
//  see CLAUDE.md's own note on what that means.
//============================================================================
class CModel
{
public:
    virtual void ReleaseSelf() {}
    virtual void OnZeroRefCount() {}

    CModel(int param94);

    char         m_reserved04[0x94 - 0x04];     // +0x04
    int          m_param94;                     // +0x94
    char         m_reserved98[0x9C - 0x98 + 4]; // +0x98
};

CModel::CModel(int param94)
{
    for (char* p = m_reserved04; p < (char*)&m_param94; p += 4)
        *(int*)p = 0;

    *(float*)&m_reserved04[0x64 - 0x04] = 1.0f;
    for (int off = 0x68; off <= 0x90; off += 4)
        *(float*)((char*)this + off) = 0.0f;

    m_param94 = param94;
    *(int*)m_reserved98 = 0;
    *(int*)(m_reserved98 + 4) = 0;
}

class CModelComplex_ : public CModel
{
public:
    CModelComplex_();

    char m_reservedA0[0xA4 - 0xA0];       // +0xA0 - the dword is -1
    char m_reservedA4[0x110 - 0xA4 + 4];  // +0xA4..+0x110, zeroed
};

CModelComplex_::CModelComplex_() : CModel(0x10)
{
    *(int*)m_reservedA0 = -1;
    for (char* p = m_reservedA4; p <= m_reservedA4 + (0x110 - 0xA4); p += 4)
        *(int*)p = 0;
}
