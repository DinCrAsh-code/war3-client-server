//============================================================================
//  0x6F495F50 - NIpse::CPoPosCl::Slot4, vtable slot 4. See poposcl.h's own
//  header comment for the full "m_poSeparate allocator chain" this
//  session closed. Own translation unit, separate from
//  TeardownAndOptionallyRecreatePoSeparate
//  (poposcl_relatedagentslot4_teardown.cpp) - see that file's own header
//  comment for why.
//============================================================================
#include "poposcl.h"

namespace NIpse {

void CPoPosCl::Slot4(unsigned int flags)
{
    char* base = (char*)this;
    *(unsigned int*)(base + 0x4C) |= 0x1000000;

    void* prev = *(void**)(base + 4);
    void* next = *(void**)(base + 8);
    if (prev != 0)
        *(void**)((char*)prev + 8) = next;
    if (next != 0)
        *(void**)((char*)next + 4) = prev;
    *(void**)(base + 4) = 0;
    *(void**)(base + 8) = 0;

    TeardownAndOptionallyRecreatePoSeparate(0, 0, 0, 0);

    if (m_relatedAgent != 0)
    {
        typedef void* (__thiscall *Slot4Fn)(void*, unsigned int);
        void* vtbl = *(void**)m_relatedAgent;
        Slot4Fn fn = (Slot4Fn)*(void**)((char*)vtbl + 0x10);
        fn(m_relatedAgent, 0);
        m_relatedAgent = 0;
    }

    ((CPoPos*)this)->Slot4((int)flags);
}

}  // namespace NIpse
