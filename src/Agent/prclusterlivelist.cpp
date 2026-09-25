//============================================================================
//  0x6F48E630 - SAgentLiveList::InsertAtHead.  See prclusterlivelist.h.
//============================================================================
#include "prclusterlivelist.h"

void SAgentLiveList::InsertAtHead(void* node)
{
    char* n = (char*)node;
    *(void**)(n + 4) = this;
    void* oldFirst = m_first;
    *(void**)(n + 8) = oldFirst;
    oldFirst = m_first;
    if (oldFirst != 0)
        *(void**)((char*)oldFirst + 4) = node;
    m_first = node;
}
