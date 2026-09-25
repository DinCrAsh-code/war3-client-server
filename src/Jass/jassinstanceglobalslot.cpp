//============================================================================
//  0x6F4596D0 - the address of one slot of the global-variable array a
//  JassInstance was constructed against.  `retn 4`.
//============================================================================
#include "jassinstance.h"

void** JassInstance::GetGlobalSlot(unsigned int index)
{
    void** base = (void**)*m_field2858;
    return &base[index];
}
