//============================================================================
//  0x6F60E8C0 - CSimpleRegion::CSimpleRegion.
//
//  The five 0xFF bytes at +0x68 are written as one byte store and one dword
//  store at +0x69, which is what says they are one array rather than a dword
//  colour and a separate byte.
//============================================================================
#include "frame.h"
#include "framethunks.h"

//  No vtable store by hand: CSimpleRegion inherits CLayoutFrame's one
//  `virtual`, so MSVC stamps ??_7CSimpleRegion@@6B@ itself - which is the
//  shipped `mov dword ptr [esi], offset ??_7CSimpleRegion@@6B@` at
//  0x6F60E8F7, and the symbol the shipped operand names rather than its
//  address as a literal.
CSimpleRegion::CSimpleRegion(CSimpleFrame* parent, int layer, int attach)
{
    m_color[0] = 0xFF;
    *(unsigned int*)&m_color[1] = 0xFFFFFFFFu;
    m_80 = 1.0f;
    m_70 = 0;
    m_pParent = 0;
    m_layer = 0;
    m_regionFlags = 0;

    if (parent != 0)
        SetParentLayer(parent, layer, attach);
}
