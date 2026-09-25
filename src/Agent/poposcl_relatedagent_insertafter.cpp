//============================================================================
//  0x6F493830 - NIpse::SPoSeparateGlobalListAnchor::InsertAfter. See
//  poposcl.h's own declaration.
//============================================================================
#include "poposcl.h"

namespace NIpse {

void SPoSeparateGlobalListAnchor::InsertAfter(CPoSeparate* node)
{
    char* nodeBase = (char*)node;
    *(void**)(nodeBase + 4) = this;
    *(void**)(nodeBase + 8) = m_next;

    if (m_next != 0)
        *(void**)((char*)m_next + 4) = node;

    m_next = node;
}

}  // namespace NIpse
