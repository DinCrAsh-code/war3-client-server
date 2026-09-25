//============================================================================
//  Net::CGameDescStore - vtable slots 5, 6, 7, 8 and 10 (0x6F970E80).
//  All five read/write the same four-field layout
//  (+0x04/+0x08/+0x0C/+0x10/+0x14) CDataStoreScratch's own class already
//  documents (cdatastorescratch.h), and slot 7 calls through vtable+0x0C -
//  exactly CDataStoreScratch's own Grow slot (vtable slot 3) - with Grow's
//  own seven-argument shape. Consistent with the same /OPT:ICF folding
//  vtable_classes.json's own CGameDescStore notes already flag for slots
//  0/1/9/4 (this session's own DeleteSelf finding,
//  cdatastorescratch_deleteself.cpp): these are almost certainly more of
//  CDataStoreScratch's own methods, not genuinely CGameDescStore's code.
//  Modelled the same non-committal way - a standalone shim class matching
//  the field layout, not wired into CDataStoreScratch's own `virtual`
//  table (this session did not establish real slot indices there).
//============================================================================

class CDataStoreScratchFieldsShim
{
public:
    void*  m_vtable;    // +0x00
    void*  m_field4;    // +0x04
    int    m_field8;    // +0x08
    int    m_field0xC;  // +0x0C
    int    m_field10;   // +0x10
    int    m_field14;   // +0x14

    typedef int (__thiscall *GrowFn)(void*, unsigned int, unsigned int, void**, int*, int*, int, int);

    //  0x6F2C83E0 - vtable slot 5.  `this` unused; returns 0.  `retn` with
    //  no operand.
    int ZeroAnswer_Slot5();

    //  0x6F2C8400 - vtable slot 6.  `m_field14 == m_field10`.  `retn` with
    //  no operand.
    int IsAtEnd_Slot6();

    //  0x6F2C8410 - vtable slot 7.  If m_field0xC == -1, zero m_field4 and
    //  m_field0xC (through the same &m_field4+0xC alias the destructor's
    //  own tail uses). If m_field8 > 0, call Grow(0, 0, &m_field4,
    //  &m_field8, &m_field0xC, 0, 0) through this object's own vtable slot
    //  3 (Grow's real slot, CDataStoreScratch::Grow). Then unconditionally
    //  m_field14 = -1, m_field10 = 0. `retn` with no operand.
    void Reset_Slot7();

    //  0x6F2C8460 - vtable slot 8.  `m_field14 = 0`.  `retn` with no
    //  operand.
    void ZeroField14_Slot8();

    //  0x6F654260 - vtable slot 10, IDA's `nullsub_1622`.  `retn 0Ch` -
    //  `this` plus three stack dwords, all unused, no body at all. Same
    //  "NoOpFinallyStub" shape as nullsub_45 (Misc/nullstub.cpp), but a
    //  genuine thiscall member here (three stack ints) rather than a free
    //  function - a free `void(int,int,int)` compiles caller-cleanup
    //  (`retn` with no operand) at this build's default convention and
    //  does not match.
    void NoOpSlot10(int, int, int);
};

int CDataStoreScratchFieldsShim::ZeroAnswer_Slot5()
{
    return 0;
}

int CDataStoreScratchFieldsShim::IsAtEnd_Slot6()
{
    return m_field14 == m_field10;
}

void CDataStoreScratchFieldsShim::Reset_Slot7()
{
    if (m_field0xC == -1)
    {
        m_field4 = 0;
        m_field0xC = 0;
    }
    if (m_field8 > 0)
    {
        GrowFn grow = (GrowFn)(*(void***)this)[3];
        grow(this, 0, 0, &m_field4, &m_field8, &m_field0xC, 0, 0);
    }
    m_field14 = -1;
    m_field10 = 0;
}

void CDataStoreScratchFieldsShim::ZeroField14_Slot8()
{
    m_field14 = 0;
}

void CDataStoreScratchFieldsShim::NoOpSlot10(int, int, int)
{
}
