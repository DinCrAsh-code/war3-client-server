//============================================================================
//  0x6F6E3220 - TCPMGR::Destruct.
//
//  Two loops and then the members in reverse declaration order.  The first
//  loop is the interesting one: each CONNSTATE hands its whole contents to a
//  *stack* list (link offset 8, born reset) rather than being drained in
//  place, and that list is then walked front to back releasing every
//  connection.  Doing it that way is what lets the shard's own lock be
//  dropped before any NETCONN destructor runs.
//
//  Written as a plain function for the same reason Construct is, and the
//  member teardowns are spelled out in the shipped order rather than left to
//  a compiler-generated destructor this build would lower differently.
//============================================================================
#include "ostcp.h"
#include "crtseh.h"

namespace OsNet
{

void __fastcall ConnStateDestruct(void*);       // 0x6F6E0D10
void __fastcall ListenSlotsDestructEl(void*);   // 0x6F6E0DB0

//: 0x6F6DF670 - retn 4.  Move every node of every shard onto `out`.
void __fastcall ConnStateTakeAll(CONNSTATE* self, TSExplicitList<NETCONN>* out);
//: 0x6F6DE330 / 0x6F6DE270 / 0x6F6DE4A0 - retn 0.  Three of the four
//: TSExplicitList<T>::UnlinkAll instantiations this module reaches; the
//: fourth (0x6F6DE160) is TSExplicitList<LOOPINPUT>::UnlinkAll, reconstructed
//: for real (ostcp_thunks.cpp) and called as m_inputs.UnlinkAll() below.
void __fastcall ListUnlinkAllConn(void*);
//: batch-J-w3gsaction-tiers - the raw `ListUnlinkAllConnAtInputAddr` redirect
//: this file used to call for m_listB (same shipped address, 0x6F6DE160, as
//: TSExplicitList<LOOPINPUT>::UnlinkAll above - identical-COMDAT-folded in
//: the shipped binary) is gone: TSExplicitList<T>::UnlinkAll's own body
//: (tslist.inl) never reads its LINKOFFSET template argument - only the
//: runtime m_linkoffset field TCPMGR::Construct already sets to 8 for both
//: m_inputs and m_listB - and TSLink<T> itself carries no T-dependent
//: layout, so TSExplicitList<NETCONN>::UnlinkAll() compiles to the exact
//: same instructions as the LOOPINPUT instantiation already scored above:
//: calling m_listB.UnlinkAll() directly is a second real instantiation of
//: the same template, not a guess, and removes a thunk this session can
//: account for byte-for-byte instead of leaving it a placeholder.
void __fastcall ListUnlinkAllConn6C(void*);
void __fastcall ListUnlinkAllThread(void*);
//: 0x6F6E0E30 - retn 0.  Drain one connect-slot set.
void __fastcall ListenSlotsDrain(void* slots);
//: 0x6F6E2CF0 - retn 0.  LISTENSLOTS' own teardown.
void __fastcall ListenSlotsDestruct(void* slots);

void TCPMGR::Destruct()
{
    int remaining = 4;
    CONNSTATE* state = &m_states[0];

    do
    {
        TSExplicitList<NETCONN> taken;
        taken.m_linkoffset = 8;
        taken.Reset();

        ConnStateTakeAll(state, &taken);

        for (;;)
        {
            int node = taken.TailLink();
            if (node <= 0)
                break;

            //  0x6F6DA3E0 - NETCONN::ReleaseRaw(), already reconstructed for
            //  real in netconnrefraw.cpp; a prior session's separate
            //  `NetConnRelease` free-function thunk at the same address
            //  (ostcp_thunks.cpp) shadowed it and is removed.
            ((NETCONN*)node)->ReleaseRaw();
        }

        ListUnlinkAllConn(&taken);
        taken.m_terminator.Unlink();

        state = (CONNSTATE*)((char*)state + 0x12C);
    }
    while (--remaining);

    for (int i = 0; i < 4; i++)
        ListenSlotsDrain(&m_connects[i]);

    ListUnlinkAllThread(&m_threads);
    m_threads.m_terminator.Unlink();

    if (m_idleEvent != 0)
        CloseHandle(m_idleEvent);

    EhVectorDestructorIterator(&m_connects[0], 0x30, 4,
                               (void (__thiscall*)(void*))ListenSlotsDestructEl);
    ListenSlotsDestruct(&m_listens);
    EhVectorDestructorIterator(&m_states[0], 0x12C, 4,
                               (void (__thiscall*)(void*))ConnStateDestruct);

    ListUnlinkAllConn6C(&m_listC);
    m_listC.m_terminator.Unlink();
    //  0x6F6DE160 for both calls in the shipped binary (identical-COMDAT
    //  folding collapsed TSExplicitList<NETCONN>::UnlinkAll and
    //  TSExplicitList<LOOPINPUT>::UnlinkAll into one body) - this build does
    //  not fold, so only the second is reconstructed as its real template
    //  instantiation (ostcp_thunks.cpp); the first keeps going through the
    //  address directly since NETCONN's own instantiation is out of scope.
    m_listB.UnlinkAll();
    m_listB.m_terminator.Unlink();
    m_inputs.UnlinkAll();
    m_inputs.m_terminator.Unlink();

    DeleteCriticalSection(&m_lock);

    if (m_pumpThreads.m_data != 0)
        SMemFree(m_pumpThreads.m_data, ".PAX", -2, 0);
}

}  // namespace OsNet
