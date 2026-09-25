//============================================================================
//  0x6F620ED0 - CBackdropFrame::CBackdropFrame.
//
//  A CModelFrame, both vtables, one non-zero float (0.025 at +0x1B8), nine
//  cleared fields, two flag merges and seven more cleared words.
//
//  The two flag merges are written through a helper and not as four separate
//  `|=` statements: the shipped stream materialises the constant into ecx and
//  `or`s it into both words (`mov ecx, 20h` / `or [esi+10h], ecx` /
//  `or [esi+0Ch], ecx`, then the same with 2), which is what an inlined
//  two-line helper produces and what two independent statements do not.
//
//  Own translation unit: its base's constructor is a real call.
//============================================================================
#include "modelframe.h"

//  m_flagMask and m_flags in that order - the shipped stores are
//  `or [+0x10]` then `or [+0x0C]`.
static __forceinline void MergeLayerFlags(CLayer* layer, int bits)
{
    layer->m_flagMask |= bits;
    layer->m_flags    |= bits;
}

CBackdropFrame::CBackdropFrame(int a, int b, int c)
    : CModelFrame(a, b, c)
{
    m_float1B8 = 0.025f;

    m_field1C0 = 0;
    m_float1BC = 0.0f;
    m_field1C4 = 0;
    m_inset[0] = 0.0f;
    m_field1D8 = 0;
    m_inset[1] = 0.0f;
    m_inset[2] = 0.0f;
    m_inset[3] = 0.0f;

    MergeLayerFlags(this, 0x20);
    MergeLayerFlags(this, 2);

    m_field178 = 0;
    m_field17C = 0;
    m_field180 = 0;
    m_field184 = 0;
    m_field188 = 0;
    m_field18C = 0;
    m_field190 = 0;
}
