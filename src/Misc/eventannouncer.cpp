//============================================================================
//  See eventannouncer.h - the one real definition of CEventAnnouncer::
//  PostEvent, shared by every embedded owner (src/Jass/jasstimerstartelapsed.cpp,
//  src/Agent/agenttypetag.cpp).
//============================================================================
#include "eventannouncer.h"

__declspec(naked) void* CEventAnnouncer::PostEvent(const int*, unsigned int,
                                                    void*, int, int)
{
    __asm { mov eax, 0x6F477A80 }
    __asm { jmp eax }
}
