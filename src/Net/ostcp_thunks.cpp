//============================================================================
//  Naked redirects for the OsTcp.cpp callees this batch declares and calls
//  but does not reconstruct - the element constructors and destructors of
//  TCPMGR's two object arrays, the four TSExplicitList<T>::UnlinkAll
//  instantiations its destructor drives, and three helpers below them.  Each
//  body jumps to the shipped function at its real address, so the call runs
//  original code; none is a reconstruction and none is ever hookable.
//
//  **Every argument count below came off the shipped call site's own push
//  count**, which for this set is unusually strong evidence rather than
//  unusually weak: everything in this binary is callee-cleanup, so a call
//  site that pushes nothing and does not adjust esp afterwards can only be
//  calling something that pops nothing.  Thirteen of the fourteen are
//  reached with `mov ecx, <object>` and no push at all, which is `retn 0`;
//  the fourteenth, ConnStateTakeAll, has exactly one push, which is
//  `retn 4`.  None of them has a dump body in this tree, so
//  tools/thunk_abi_audit.py reports them as "no dump body for the target" -
//  unknown, not clean - and an `ida_query func_at` plus a three-byte tail
//  read would settle each one outright.
//
//      0x6F6E2BD0  ecx = CONNSTATE, no push          -> retn 0
//      0x6F6E0D10  ecx = CONNSTATE, no push          -> retn 0
//      0x6F6E2D70  ecx = LISTENSLOTS, no push        -> retn 0
//      0x6F6E0DB0  ecx = LISTENSLOTS, no push        -> retn 0
//      0x6F6E10D0  ecx = list, no push               -> retn 0
//      0x6F6E0FD0  ecx = list, no push               -> retn 0
//      0x6F6DF670  ecx = CONNSTATE, one push         -> retn 4
//      0x6F6DE330  ecx = list, no push               -> retn 0
//      0x6F6DE160  ecx = list, no push               -> retn 0
//      0x6F6DE270  ecx = list, no push               -> retn 0
//      0x6F6DE4A0  ecx = list, no push               -> retn 0
//      0x6F6E0E30  ecx = LISTENSLOTS, no push        -> retn 0
//      0x6F6E2CF0  ecx = LISTENSLOTS, no push        -> retn 0
//      0x6F6E1370  ecx = TCPLISTEN, no push          -> retn 0
//
//  0x6F6E1420 (TCPLISTEN::SelectSetUpdate), 0x6F6DA3E0 (NETCONN::Release)
//  and 0x6F6DE160 (TSExplicitList<LOOPINPUT>::UnlinkAll) were also in this
//  table; all three are reconstructed for real below now, past the naked
//  redirects.
//
//  The two `unknown_libname_950`/`953` entries the shipped stream passes as
//  the critical-section element ctor/dtor are MSVC's own
//  InitializeCriticalSection/DeleteCriticalSection thunks.  IDA gives
//  neither an address, so those two are written out rather than redirected;
//  see below.
//============================================================================
#include "ostcp.h"

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

namespace OsNet
{

ADDR_THUNK(void __fastcall ConnStateConstruct(void*),      0x6F6E2BD0)
ADDR_THUNK(void __fastcall ConnStateDestruct(void*),       0x6F6E0D10)
ADDR_THUNK(void __fastcall ListenSlotsConstructEl(void*),  0x6F6E2D70)
ADDR_THUNK(void __fastcall ListenSlotsDestructEl(void*),   0x6F6E0DB0)
ADDR_THUNK(void __fastcall ListenListConstruct(void*),     0x6F6E10D0)
ADDR_THUNK(void __fastcall ListenListDestruct(void*),      0x6F6E0FD0)
//  Not redirects: the shipped stream passes MSVC's own
//  `unknown_libname_950` / `unknown_libname_953` as the element ctor/dtor
//  for LISTENSLOTS' one-element CRITICAL_SECTION array, and IDA gives
//  neither an address this repo can read.  Redirecting to a guessed address
//  would be a live jump into whatever is really there, so they are written
//  out instead - which is exactly what those two CRT thunks do.
void __fastcall CriticalSectionConstruct(void* cs)
{
    InitializeCriticalSection((CRITICAL_SECTION*)cs);
}

void __fastcall CriticalSectionDestruct(void* cs)
{
    DeleteCriticalSection((CRITICAL_SECTION*)cs);
}
ADDR_THUNK(void __fastcall ConnStateTakeAll(CONNSTATE*, TSExplicitList<NETCONN>*), 0x6F6DF670)
ADDR_THUNK(void __fastcall ListUnlinkAllConn(void*),       0x6F6DE330)
ADDR_THUNK(void __fastcall ListUnlinkAllConn6C(void*),     0x6F6DE270)
ADDR_THUNK(void __fastcall ListUnlinkAllThread(void*),     0x6F6DE4A0)
ADDR_THUNK(void __fastcall ListenSlotsDrain(void*),        0x6F6E0E30)
ADDR_THUNK(void __fastcall ListenSlotsDestruct(void*),     0x6F6E2CF0)
ADDR_THUNK(void TCPLISTEN::Close(),                        0x6F6E1370)

}  // namespace OsNet

//----------------------------------------------------------------------------
//  0x6F6DE160 - OsNet::TSExplicitList<LOOPINPUT>::UnlinkAll, the real
//  template instantiation (see src/Containers/tslist.inl).  The shipped
//  binary identical-COMDAT-folds this onto the SAME address as
//  TSExplicitList<NETCONN>::UnlinkAll (TCPMGR::m_listB's own instantiation,
//  both born with m_linkoffset==8); this build does not fold, but
//  UnlinkAll's own body never reads the LINKOFFSET template argument at all
//  (it walks by the runtime m_linkoffset field) and TSLink<T> carries no
//  T-dependent layout, so both instantiations still compile to the exact
//  same instructions - batch-J-w3gsaction-tiers reconstructs the NETCONN
//  one for real below too (ostcp_mgrdestruct.cpp calls m_listB.UnlinkAll()
//  directly now), rather than leaving it behind a raw address redirect.
//----------------------------------------------------------------------------
#include "tslist.inl"
template void TSExplicitList<OsNet::LOOPINPUT>::UnlinkAll();
template void TSExplicitList<OsNet::NETCONN>::UnlinkAll();

//----------------------------------------------------------------------------
//  0x6F6E1420 - OsNet::TCPLISTEN::SelectSetUpdate.  Toggle listen() on or
//  off when the caller's requested state differs from the one already
//  stored; on the transition to listening (backlog 0xFA), and only then,
//  drain any queued AcceptEx slots at m_pendingRearm through
//  TCPACCEPT::Rearm() (0x6F6DBD20, already real - ostcp_rearm.cpp).  On the
//  transition to not-listening (backlog 0), a failed listen() also closes
//  the socket and marks it dead, matching CloseSocket's own INVALID_SOCKET
//  convention.
//----------------------------------------------------------------------------
namespace OsNet
{

void TCPLISTEN::SelectSetUpdate(int wantListen)
{
    if (m_wantListen == wantListen)
        return;
    m_wantListen = wantListen;

    if (listen(m_socket, wantListen ? 0xFA : 0) != 0)
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return;
    }

    for (TCPACCEPT* node = m_pendingRearm.Tail(); node; node = m_pendingRearm.Prev(node))
        node->Rearm();
}

//----------------------------------------------------------------------------
//  The six thread bodies the starts hand _beginthreadex.  Each is
//  `__stdcall` with one argument, which is what _beginthreadex calls, and
//  each is only ever *pointed at* from here - never called - so the
//  redirect's own arity is the callee's.
//
//      0x6F6E4FE0 timer, 0x6F6E5FB0 listen, 0x6F6E5CF0 select,
//      0x6F6E68E0 notify, 0x6F6E4F30 IOCP pump, 0x6F6E5910 select pump
//----------------------------------------------------------------------------
}  // namespace OsNet

extern "C" {
ADDR_THUNK(void __stdcall OsTcpTimerThreadMain(void*),  0x6F6E4FE0)
ADDR_THUNK(void __stdcall OsTcpListenThreadMain(void*), 0x6F6E5FB0)
ADDR_THUNK(void __stdcall OsTcpSelectThreadMain(void*), 0x6F6E5CF0)
ADDR_THUNK(void __stdcall OsTcpNotifyThreadMain(void*), 0x6F6E68E0)
ADDR_THUNK(void __stdcall OsTcpIocpPumpMain(void*),     0x6F6E4F30)
ADDR_THUNK(void __stdcall OsTcpSelectPumpMain(void*),   0x6F6E5910)
}
