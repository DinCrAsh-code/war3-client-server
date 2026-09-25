//============================================================================
//  0x6F277770 - see the declaration in unit.h.
//============================================================================
#include "unit.h"

void CUnit::StoreMessageField0x34ToInt194(void* message)
{
    void* inner = *(void**)((char*)message + 0xC);
    m_int194 = *(unsigned int*)((char*)inner + 0x34);
}
