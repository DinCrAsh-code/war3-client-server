//============================================================================
//  0x6F6E2C60 - LISTENSLOTS::Construct.  The vftable, one list, one lock, and
//  the cursor and count zeroed.  Same explicit-helper shape as
//  TCPMGR::Construct, and for the same reason.
//============================================================================
#include "ostcp.h"
#include "crtseh.h"

namespace OsNet
{

void __fastcall ListenListConstruct(void*);   // 0x6F6E10D0
void __fastcall ListenListDestruct(void*);    // 0x6F6E0FD0
void __fastcall CriticalSectionConstruct(void*);  // unknown_libname_950
void __fastcall CriticalSectionDestruct(void*);   // unknown_libname_953

LISTENSLOTS* LISTENSLOTS::Construct()
{
    EhVectorConstructorIterator(&m_list, 0x0C, 1,
                                (void (__thiscall*)(void*))ListenListConstruct,
                                (void (__thiscall*)(void*))ListenListDestruct);
    EhVectorConstructorIterator(&m_lock, 0x18, 1,
                                (void (__thiscall*)(void*))CriticalSectionConstruct,
                                (void (__thiscall*)(void*))CriticalSectionDestruct);

    m_cursor = 0;
    m_count = 0;
    return this;
}

}  // namespace OsNet
