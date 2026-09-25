//============================================================================
//  0x6F6E30D0 - TCPMGR::Construct.  Written as a plain function rather than
//  a C++ constructor: the shipped stream builds its two object arrays through
//  the CRT's `eh vector constructor iterator`, and this build's fixed
//  /EHs-c- lowers an array construction into a counted loop instead (the same
//  toolchain gap CAbilityInventory::CAbilityInventory's own BEHAVIOUR entry
//  records from the other side), so the helper is called explicitly here.
//  Everything else is the shipped order, field for field.
//============================================================================
#include "ostcp.h"
#include "crtseh.h"

namespace OsNet
{

//: The four element ctor/dtor pairs the two arrays are built and torn down
//: with.  None has a dump body in this tree; each is a naked redirect in
//: ostcp_thunks.cpp.
void __fastcall ConnStateConstruct(void*);      // 0x6F6E2BD0
void __fastcall ConnStateDestruct(void*);       // 0x6F6E0D10
void __fastcall ListenSlotsConstructEl(void*);  // 0x6F6E2D70
void __fastcall ListenSlotsDestructEl(void*);   // 0x6F6E0DB0

TCPMGR* TCPMGR::Construct()
{
    m_refcount = 1;
    m_workerCount = 0;
    m_pumpThreads.m_alloc = 0;
    m_pumpThreads.m_count = 0;
    m_pumpThreads.m_data = 0;
    m_pumpThreads.m_chunk = 0;

    InitializeCriticalSection(&m_lock);

    m_pollThread = 0;
    m_pollEvent = 0;
    m_timerThread = 0;
    m_timerEvent = 0;

    m_inputs.m_linkoffset = 8;
    m_inputs.Reset();
    m_listB.m_linkoffset = 8;
    m_listB.Reset();
    m_listC.m_linkoffset = 0x6C;
    m_listC.Reset();

    EhVectorConstructorIterator(&m_states[0], 0x12C, 4,
                                (void (__thiscall*)(void*))ConnStateConstruct,
                                (void (__thiscall*)(void*))ConnStateDestruct);

    m_selectThread = 0;
    m_listens.Construct();

    m_listenThread = 0;
    m_listenWakeEvent = 0;
    m_listenStopping = 0;
    m_listenDoneEvent = 0;

    EhVectorConstructorIterator(&m_connects[0], 0x30, 4,
                                (void (__thiscall*)(void*))ListenSlotsConstructEl,
                                (void (__thiscall*)(void*))ListenSlotsDestructEl);

    m_iocp = 0;
    *(int*)m_reserved61C = 0;
    m_idleEvent = CreateEventA(0, FALSE, TRUE, 0);
    *(int*)m_reserved624 = 0;

    m_threads.m_linkoffset = 0;
    m_threads.Reset();

    return this;
}

}  // namespace OsNet
