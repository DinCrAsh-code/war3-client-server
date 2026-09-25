//============================================================================
//  See netconn.cpp/netconnctor.cpp for the class-level notes, and
//  ostcp.h for the TCPCONN/SLTCPCONN/IOTCPCONN hierarchy built on top.
//============================================================================
#ifndef NETCONN_H
#define NETCONN_H

#include <windows.h>
#include "storm.h"

namespace OsNet
{

class NETCONN;
class TCPMGR;       // ostcp.h - the owner every connection carries at +0x60
struct OUTPUT;      // ostcp.h - one queued outbound buffer
struct IOOP;        // ostcp.h - an OVERLAPPED plus the op kind

//----------------------------------------------------------------------------
//  The notification the owner of a connection registered with it.  Seven
//  __stdcall arguments, and all seven are now typed - which took all three
//  notifiers to work out, because each one only uses part of the prototype:
//
//    reason 0  connected     NotifyConnected     0x6F6DA240
//    reason 1  data arrived  NotifyData          0x6F6DA2C0
//    reason 2  disconnected  NotifyDisconnected  0x6F6DA280
//
//  `data`/`len` are the receive buffer and how much is in it, and `consumed`
//  is an out-slot the callback writes to say how many bytes it framed off the
//  front - which is what makes the whole receive path work, since
//  TCPCONN::OnRecvComplete compacts the remainder against it.
//
//  The connected/disconnected pair pass null/zero for `data`/`len` and hand
//  `consumed` the address of their own `ctx` local.  That looked redundant
//  when only those two were known ("it passes both the context value and the
//  address of the local holding it"); it is not.  It is one prototype reused,
//  with a throwaway address for the out-slot those two reasons never read.
//
//  `addrs` is normally &NETCONN::m_addrs - the {remote, local} sockaddr pair
//  the constructor copied in - but NotifyData takes an override, which is how
//  a datagram path could report a per-packet source instead.
//----------------------------------------------------------------------------
typedef int (__stdcall* CONNNOTIFYPROC)(NETCONN* conn, void* addrs, int reason,
                                        void* ctx, const void* data,
                                        unsigned int len,
                                        unsigned int* consumed);

//----------------------------------------------------------------------------
//  OsNet::NETCONN - the base of the LAN net provider's per-socket connection
//  objects (`??_7NETCONN@OsNet@@6B@` in the dump). Only the fields this call
//  tree actually touches are named; the rest are `m_reservedNN` gaps of the
//  exact size the constructor/destructor prove out (a 32-byte block copied
//  in whole from the constructor's fifth argument, a byte state machine at
//  +0x10/+0x11, and a CRITICAL_SECTION at +0x44).
//
//  The vtable is ten slots, and it is no longer inferred: the user dumped it
//  out of IDA into user_knowledge.json, along with TCPCONN's and IOTCPCONN's.
//  Four of the ten are `_purecall` in the shipped table and are given trivial
//  bodies here for the reason refcnt.h's TRefCnt gives - a concrete derived
//  object is placement-new'd and a class embedded by value cannot leave any
//  base virtual unresolved - and for the linking reason spelled out on the
//  slot list itself.
//----------------------------------------------------------------------------
class NETCONN
{
public:
    //  0x6F6DCEC0 - retn 14h (5 stack args), in this order.  The OsTcp call
    //  tree types every one of them: TCPCONN's constructor forwards its own
    //  first five arguments straight through, and CreateConnection builds
    //  those out of the accepting LISTENER's callback pair and the socket
    //  the accept produced.
    NETCONN(TCPMGR* owner, SOCKET s, CONNNOTIFYPROC proc, void* ctx,
            const void* addrs);

    //  0x6F6DCE00 - deletes m_critSec, then unlinks the embedded
    //  TSLink<NETCONN> (see storm.h's TSLink<T>::Unlink - the exact same
    //  shape).  That order is pinned down by ~TCPCONN, which has this body
    //  inlined; see ostcp_conndtor.cpp.
    //
    //  Defined in ostcp_conndtor.cpp rather than netconn.cpp, which is the
    //  arrangement the shipped module has: `~TCPCONN` ends with this body
    //  *inlined* (the vptr store, DeleteCriticalSection and the link Unlink
    //  all appear in 0x6F6E0520 with no call), which only happens if the two
    //  are compiled together, while 0x6F6DCE00 still exists as a standalone
    //  function for the other translation units that call it.
    ~NETCONN();

    //  The vtable, from the dump the user pulled out of IDA and recorded in
    //  user_knowledge.json (`OsNet::NETCONN` @ 0x6F975700) - ten slots, not
    //  the sixteen this header guessed before that arrived.  The seven extra
    //  slots the derived classes reach through belong to TCPCONN, which
    //  extends this to seventeen (@ 0x6F97587C); IOTCPCONN's is the same
    //  length (@ 0x6F975954).
    //
    //  Slots the shipped vtable fills with `_purecall` are given trivial
    //  bodies here rather than being declared `= 0`, for the reason this
    //  header already gives below and because matching_decomp_injection's
    //  .mix has to link: an undefined virtual is a link error, and nothing in
    //  this repo's own code ever executes one of these bodies anyway (a hook
    //  patches a function, never a vtable, so every virtual call at run time
    //  dispatches through the *game's* vptr on the game's own object).  The
    //  addresses in the comments are what the slot actually holds.
    //
    //  Slot 0 - `_purecall` in NETCONN and TCPCONN; IOTCPCONN nullsub_44.
    virtual void m_reservedSlot0() {}
    //  Slot 1 (+0x04) - 0x6F6D9A10, NETCONN's own, inherited unchanged by
    //  both derived classes.  Nothing in any dump here calls it.
    //
    //  Its body is `return m_socket == INVALID_SOCKET;` - the same
    //  comparison OnPendingDrained (slot 3) makes before closing, so this
    //  reads as a plain "is there still a live socket" query.  No caller in
    //  either dump, so the name is a best guess from the body alone.
    virtual BOOL IsSocketClosed();
    //  Slot 2 (+0x08) - `_purecall` in NETCONN and TCPCONN; IOTCPCONN
    //  nullsub_1736.
    virtual void m_reservedSlot2() {}
    //  Slot 3 (+0x0C) - `_purecall` in NETCONN, TCPCONN 0x6F6DA7D0.
    //  ReleasePending calls it on the transition to zero outstanding I/O,
    //  immediately before Shutdown(1).
    virtual void OnPendingDrained() {}
    //  Slot 4 (+0x10) - the hook ReleaseRaw() calls once the last reference
    //  drops.  NETCONN 0x6F6DCD90, TCPCONN 0x6F6E24C0, IOTCPCONN 0x6F6E25F0.
    //
    //  **Renamed from `OnZeroRefCount(int)`.**  All three bodies are MSVC's
    //  own scalar-deleting-destructor shape - `this->~X(); if (flags & 1)
    //  SMemFree(this, "delete", -1, 0); return this;` - exactly the pattern
    //  `refcnt.h`'s `TRefCnt::DeleteSelf` and the whole `CWidget`/`CItem`
    //  chain already carry this name for (see
    //  `docs/msvc-vc8-idioms.md`, "A deleting destructor's `this != 0` test
    //  tracks whether the base destructor inlined" - NETCONN's own body has
    //  no null test because its `~NETCONN()` call inlines, and TCPCONN's and
    //  IOTCPCONN's both do because theirs is a real `call`, exactly that
    //  rule).  `OnZeroRefCount` was a guess made before the vtable dump
    //  proved the body; `DeleteSelf` is what the rest of this codebase
    //  already calls the identical shape, so this keeps one name for one
    //  idiom instead of inventing a second.  ReleaseRaw()
    //  (netconnrefraw.cpp) tail-calls it with flags=1 ("run the destructor,
    //  then free").
    virtual NETCONN* DeleteSelf(int flags);
    //  Slot 5 (+0x14) - nullsub_1740 in NETCONN and TCPCONN, IOTCPCONN
    //  0x6F6DA8C0.  IOTCPCONN::PostRead calls it before arming the receive.
    //  NETCONN's own nullsub_1740 is 0x6F6DCD50 - resolved this session
    //  while dumping the OsNet connection-family siblings
    //  (docs/targets/OsNet_connection_family_siblings_vtable_sweep.md);
    //  NETCONNLESS, NETCONNFULL, LOOPCONN, UDPCONN and SLTCPCONN all
    //  inherit it unchanged (their own vtable slot resolves to this exact
    //  address, not a per-class override).
    //
    //  IOTCPCONN's override (ostcp.h) is `InterlockedIncrement(&m_pending)`
    //  - the exact mirror of slot 6's `ReleasePending`, which decrements the
    //  same field - so it is named `AddPending` here rather than left as a
    //  reserved slot, the same way `ReleasePending` already is.
    virtual void AddPending() {}
    //  Slot 6 (+0x18) - **ReleasePending**, and naming it was the one real
    //  correction the vtable dump forced: this header previously invented an
    //  `OnIoHandled` hook for the slot, when it is the function already
    //  reconstructed EXACT at 0x6F6E0670.  Both call sites read properly with
    //  the right name - TCPMGR::PumpIocp calls it after *every* completion to
    //  drop the outstanding-I/O reference, and IOTCPCONN::PostRead tail-jumps
    //  to it when the receive could not be armed at all.
    //  nullsub_1741 in NETCONN and TCPCONN; IOTCPCONN 0x6F6E0670.  NETCONN's
    //  own nullsub_1741 is 0x6F6DCD60 (resolved this session - see the
    //  slot 5 comment above for which siblings inherit it unchanged).
    virtual void ReleasePending() {}
    //  Slots 7 and 8 (+0x1C / +0x20) - the send and receive completion
    //  callbacks TCPMGR::PumpIocp dispatches an IOOP to by its m_type
    //  (1 = send, 0 = receive).  nullsub_1742/nullsub_1743 in NETCONN; both
    //  are TCPCONN's (0x6F6E05C0 and 0x6F6DA720) and inherited by IOTCPCONN.
    //  NETCONN's own nullsub_1742/nullsub_1743 are 0x6F6DCD70/0x6F6DCD80
    //  (resolved this session - same inheriting siblings as slot 5 above).
    virtual void OnSendComplete(IOOP* /*op*/, DWORD /*bytes*/,
                                BOOL /*ok*/) {}
    virtual void OnRecvComplete(IOOP* /*op*/, DWORD /*bytes*/,
                                BOOL /*ok*/) {}
    //  Slot 9 (+0x24) - close the socket.  0x6F6DA450 in all three vtables,
    //  so it is NETCONN's and is never overridden.  Reached from Send() on a
    //  hard socket error, from Queue() when the send backlog has gone stale,
    //  from OnRecvComplete() when framing stalls against a full buffer, from
    //  PostRead() when the receive could not be armed, and from Shutdown()
    //  unconditionally.
    //
    //  Its own body is three instructions: take the connection lock, then
    //  tail-jump into slot 3 (`OnPendingDrained`) with the lock still held -
    //  `EnterCriticalSection(&m_critSec); OnPendingDrained();` compiles to
    //  exactly that tail call, because the two share the same `retn 0`
    //  shape.  `OnPendingDrained`'s own body (TCPCONN's override,
    //  0x6F6DA7D0) is what actually closes the socket and releases the
    //  lock, so Close() is a lock-then-dispatch wrapper around it, not a
    //  second, independent socket teardown.
    virtual void Close();

    //  0x6F6DA3D0 - raw `InterlockedIncrement(&m_refcount)`, no null check.
    void AddRefRaw();
    //  0x6F6DACD0 - AddRefRaw() with a null-`this` diagnostic; returns
    //  `this` (or null).
    NETCONN* AddRef();

    //  0x6F6DA3E0 - raw `InterlockedDecrement(&m_refcount)`; on reaching
    //  zero, tail-calls DeleteSelf(1).
    void ReleaseRaw();
    //  0x6F6DACF0 - ReleaseRaw() with a null-`this` diagnostic.
    void Release();

    //  0x6F6DA110 - retn 8.  A plain seqlock reader over m_notifySeq and the
    //  callback pair it guards: spin while the counter is odd, copy both
    //  fields out, retry if the counter moved.  Its own TU
    //  (ostcp_notifyread.cpp) so the two notifiers below keep their call.
    void ReadNotify(CONNNOTIFYPROC* outProc, void** outCtx);

    //  0x6F6DA240 and 0x6F6DA280 (ostcp_notify.cpp) - the same seven-argument
    //  call to the registered callback, differing only in the reason code.
    //  They are separate functions in the shipped binary, not one with a
    //  parameter.  Both return 0 when no callback is registered.
    int  NotifyConnected();
    int  NotifyDisconnected();

    //  0x6F6DA2C0 - retn 10h, the third of the trio and the only one that
    //  uses the whole prototype.  Its own TU (ostcp_notifydata.cpp) so
    //  TCPCONN::OnRecvComplete keeps the call.  `addrs` null means "use
    //  m_addrs"; returns 0 when no callback is registered, which
    //  OnRecvComplete treats as fatal and closes on.
    int  NotifyData(const void* data, unsigned int len,
                    unsigned int* consumed, const void* addrs);

    //  +0x04 is the ctor's `a4`.  The OsTcp call tree pins it down: TCPCONN's
    //  own constructor hands it straight to setsockopt/ioctlsocket as `s` and
    //  IOTCPCONN's to CreateIoCompletionPort as `FileHandle`, so it is the
    //  connection's socket, not an opaque word.
    SOCKET      m_socket;          // +0x04 - ctor's a4
    //  Laid out raw (not as a `TSLink<NETCONN> m_link` member) so the
    //  constructor's plain zero-stores stay inline instead of routing
    //  through TSLink's own out-of-line default constructor - see
    //  ostcp_conndtor.cpp's ~NETCONN() for where the TSLink view comes back.
    void*       m_linkNext;        // +0x08 - TSLink<NETCONN>.m_next
    int         m_linkPrevlink;    // +0x0C - TSLink<NETCONN>.m_prevlink
    //  Which of TCPMGR's four state lists this connection is currently on,
    //  and which of that list's eight shards it went into.  4 means "on no
    //  list"; the constructor starts there and TCPMGR::MoveConn is the only
    //  thing that ever writes either byte.
    char        m_state;          // +0x10 - ctor sets 4
    char        m_stateBucket;    // +0x11 - ctor sets 0
    char        m_reserved12[2];
    int         m_refcount;       // +0x14 - ctor sets 1
    //  +0x18 is the ctor's 32-byte `rep movsd` block, and the accept path
    //  says what is in it: TCPMGR::OnAcceptComplete hands CreateConnection a
    //  32-byte local that LISTENER::FinishAccept filled with the two 16-byte
    //  sockaddr_in results of GetAcceptExSockaddrs, remote first.  Notify()
    //  passes its address on to the owner's callback.
    char        m_addrs[0x20];        // +0x18..+0x37 - {remote, local}
    //  A sequence counter guarding the notification pair below - see
    //  ostcp_notifyread.cpp, which is a plain seqlock reader over the three
    //  fields.  The ctor starts it even (unlocked).
    int         m_notifySeq;          // +0x38 - ctor zeroes it
    CONNNOTIFYPROC m_notifyProc;      // +0x3C - ctor's pInit8
    void*       m_notifyCtx;          // +0x40 - ctor's pInitC
    CRITICAL_SECTION m_critSec;       // +0x44 (24 bytes, ends at +0x5C)
    //  When this connection was created.  TCPCONN::Queue measures the send
    //  backlog's staleness against it.
    DWORD       m_createTick;         // +0x5C - ctor's GetTickCount()
    //  The TCPMGR that owns this connection - `a0`, and the `this` every
    //  MoveConn() call in the OsTcp tree is made on.
    TCPMGR*     m_owner;              // +0x60 - ctor's a0
};

}  // namespace OsNet

#endif
