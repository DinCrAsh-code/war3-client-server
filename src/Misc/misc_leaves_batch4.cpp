//============================================================================
//  A batch of small, self-contained leaves with no owning class otherwise
//  identified - each modelled as a bare-offset operation on an opaque
//  `this`, the same convention misc_leaves_batch3.cpp uses.
//============================================================================
#include <stdlib.h>

//  0x6F001910 - __thiscall(int index /*stack*/), retn 4.  `this+8` holds a
//  base pointer to an array of 12-byte elements; returns the address of the
//  element at `index`.
struct SArrayHolder8A
{
    char  m_reserved0[8];
    void* m_data;  // +0x8

    void* __thiscall GetElementAt(int index);
};
void* __thiscall SArrayHolder8A::GetElementAt(int index)
{
    return (char*)m_data + index * 12;
}

//  0x6F003020 - identical shape to SArrayHolder8A::GetElementAt above, a
//  different address so a different (unidentified) class.
struct SArrayHolder8B
{
    char  m_reserved0[8];
    void* m_data;  // +0x8

    void* __thiscall GetElementAt(int index);
};
void* __thiscall SArrayHolder8B::GetElementAt(int index)
{
    return (char*)m_data + index * 12;
}

//  0x6F001FC0 - __thiscall(const SIntBox* other /*stack*/), retn 4.  Copies
//  a single int field from `other` and returns `this`.
struct SIntBox
{
    int m_value;

    SIntBox* __thiscall CopyFrom(const SIntBox* other);
};
SIntBox* __thiscall SIntBox::CopyFrom(const SIntBox* other)
{
    m_value = other->m_value;
    return this;
}

//  0x6F003060 - __thiscall(), retn 0.  Default-constructs a 12-byte record
//  to {0, -1, 0}.
struct SRangeTriple
{
    int m_start;
    int m_end;
    int m_flags;

    SRangeTriple();
};
SRangeTriple::SRangeTriple() : m_start(0), m_end(-1), m_flags(0)
{
}

//  0x6F005D90 - __thiscall(unsigned int value /*stack*/), retn 4.  XORs a
//  field with `value` and rotates the result left by 3 bits back into the
//  same field.
struct SStateHolder
{
    char         m_reserved0[0xC];
    unsigned int m_state;  // +0xC

    void __thiscall MixState(unsigned int value);
};
void __thiscall SStateHolder::MixState(unsigned int value)
{
    m_state = _rotl(m_state ^ value, 3);
}
