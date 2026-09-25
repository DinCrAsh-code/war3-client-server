//============================================================================
//  Twelve siblings of the NETCONN/TCPCONN/IOTCPCONN hierarchy (netconn.h,
//  ostcp.h) - OsNet::NETCONNLESS, NETCONNFULL, TCPCONNECT, FILECONNECT,
//  TCPLISTEN, LOOPCONNECT, LOOPCONN, UDPCONN, FILECONN, IOFILECONN,
//  SLFILECONN (SLTCPCONN's own five new slots are declared in ostcp.h
//  instead, since it is already declared there).  Full vtable dumps for all
//  fourteen live in user_knowledge.json; see
//  docs/targets/OsNet_connection_family_siblings_vtable_sweep.md for the
//  session that reconstructed the 50 addresses this header's classes name.
//
//  Two shapes recur across the family, and the vtable data is what proves
//  them real inheritance rather than coincidence:
//
//   - NETCONNLESS and NETCONNFULL are `: public NETCONN` with **no added
//     data at all** - their own DeleteSelf overrides (0x6F6DE8E0,
//     0x6F6DE9B0) are byte-for-byte NETCONN::DeleteSelf's body (vftable
//     stamp, DeleteCriticalSection, link Unlink, conditional free - see
//     ostcp_conndtor.cpp), which only compiles from an implicit destructor
//     with nothing of its own to tear down.  Slots 5-8 (AddPending,
//     ReleasePending, OnSendComplete, OnRecvComplete) resolve to the exact
//     same addresses as NETCONN's own (nullsub_1740/41/42/43,
//     0x6F6DCD50/60/70/80 - resolved this session and now given to
//     NETCONN's own trivial bodies in netconn.h, see that file), i.e.
//     genuinely inherited unchanged rather than merely identical.  Slot 9
//     (Close, 0x6F6DA450) is the same story.  Only slot 4 (DeleteSelf) and
//     the tail (one new pure slot for NETCONNLESS, two pure plus three
//     trivial no-op ones for NETCONNFULL) are this class's own.
//
//   - LOOPCONN, UDPCONN and FILECONN/IOFILECONN/SLFILECONN each derive from
//     NETCONN too, adding their own outbound-queue-and-handle fields after
//     NETCONN's own 0x64 bytes the same way TCPCONN does - proven the same
//     way: FILECONN's own OnPendingDrained-position override
//     (0x6F6E3970/0x6F6E39A0/0x6F6E39D0) calls a real, out-of-line
//     destructor exactly like TCPCONN::DeleteSelf calls
//     `this->TCPCONN::~TCPCONN()`, and FILECONN's slot 7/8 pair
//     (0x6F6E0840/0x6F6E2740) reaches `OsNet::TCPCONN::Shutdown`
//     (0x6F6E00C0, ostcp_shutdown.cpp) through a `(TCPCONN*)this` reinterpret
//     - legal and correct because Shutdown's own body (Close(), then
//     m_owner->MoveConn/NotifyDisconnected/ReleaseRaw) only ever touches
//     NETCONN's base fields, never TCPCONN's own m_sendQueue/m_recvBuf, so
//     it works unmodified on any NETCONN-derived object regardless of what
//     that object added past +0x64.
//
//  TCPCONNECT, FILECONNECT, TCPLISTEN and LOOPCONNECT are the odd ones out:
//  small, standalone objects (not NETCONN-derived - their own fields start
//  well inside NETCONN's own layout, e.g. a ctx pointer at +0x10 where
//  NETCONN's is at +0x40) used only while a connection is being
//  established, not once one exists.  Their slot 1 (IsSocketClosed
//  position, three of the four) resolves to the *exact same* address as
//  NETCONN's own IsSocketClosed (0x6F6D9A10) - not inheritance (these
//  classes share no base with NETCONN) but the linker's own identical-code
//  folding merging two byte-identical one-line bodies
//  (`return m_socket == INVALID_SOCKET`) into one address, the same
//  mechanism that put NETCONNFULL's and LOOPCONN's own slot-12/14 no-ops at
//  the identical address (0x6F6DE980/0x6F6DE9A0) despite the two classes
//  sharing no base either.  Since that address already carries a funcmap
//  name (NETCONN::IsSocketClosed), these four classes' own slot 1 is given
//  the identical body here with no *second* funcmap entry - one address,
//  one name, per CLAUDE.md - and is not separately scored.
//============================================================================
#ifndef OSNETFAMILY_H
#define OSNETFAMILY_H

//  Ahead of netconn.h (which pulls in plain windows.h) for the same reason
//  ostcp.h does this - this header's own TCPCONNECT/FILECONNECT/TCPLISTEN/
//  LOOPCONNECT/UDPCONN bodies use sockaddr_in, htons and friends, which need
//  Winsock 2 rather than the older Winsock 1 windows.h would otherwise drag
//  in first.
#include <winsock2.h>
//  See ostcp.h's own copy of this pragma for why it is needed at all under
//  /Zl - several of the classes below call socket functions (closesocket,
//  accept, sendto/recvfrom, getsockname) directly rather than through an
//  OsNet method that already carries the directive.
#pragma comment(lib, "ws2_32.lib")
#include "netconn.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  OsNet::NETCONNLESS (0x6F97572C) - 11 slots.  No data of its own; see the
//  file header.
//----------------------------------------------------------------------------
class NETCONNLESS : public NETCONN
{
public:
    //  Slot 4 - DeleteSelf.  0x6F6DE8E0, retn 4h.  Body-identical to
    //  NETCONN::DeleteSelf (ostcp_conndtor.cpp) - a fresh compiled instance
    //  of the same implicit destructor, at its own address because it is a
    //  distinct derived class, not because it does anything different.
    virtual NETCONNLESS* DeleteSelf(int flags);
    //  Slot 10 - `_purecall` in the shipped table (this class adds one new
    //  pure slot past NETCONN's own ten and never resolves it).  Given a
    //  trivial body for the reason netconn.h's own reserved slots are - a
    //  concrete object cannot leave a base virtual unresolved, and nothing
    //  live ever calls it.
    virtual void m_reservedSlot10() {}
};

//----------------------------------------------------------------------------
//  OsNet::NETCONNFULL (0x6F97575C) - 15 slots.  No data of its own; see the
//  file header.
//----------------------------------------------------------------------------
class NETCONNFULL : public NETCONN
{
public:
    //  Slot 4 - DeleteSelf.  0x6F6DE9B0, retn 4h.  Same shape as
    //  NETCONNLESS::DeleteSelf above.
    virtual NETCONNFULL* DeleteSelf(int flags);
    //  Slots 10/11 - `_purecall` in the shipped table, same reserved-slot
    //  convention as NETCONNLESS::m_reservedSlot10 above.
    virtual void m_reservedSlot10() {}
    virtual void m_reservedSlot11() {}
    //  Slots 12-14 - the same three-method shape TCPCONN's own slots 12-14
    //  are (SetCoalesce/SetBufferSizes/SetRecvTimeout), all three trivial
    //  no-ops here: a connectionless socket has nothing for Nagle,
    //  SO_SNDBUF/SO_RCVBUF or SO_RCVTIMEO to mean.
    //
    //  Slot 12 - 0x6F6DE980, retn 4h.  `retn 4` alone - the argument is
    //  read by nothing.  Defined out-of-line (netconnfull_trivials.cpp) so
    //  it is emitted regardless of whether anything in this build's own
    //  callee closure ever reaches it - an inline body with no caller and
    //  no emitted vtable (this class has no constructor in scope this
    //  session) never becomes a symbol verify.py can score at all.
    virtual void SetCoalesce(int coalesce);
    //  Slot 13 - 0x6F6DE990, retn 4h.  `xor eax,eax; retn 4` - always false.
    virtual BOOL SetBufferSizes(int size);
    //  Slot 14 - 0x6F6DE9A0, retn 4h.  `retn 4` alone, same as slot 12.
    virtual int SetRecvTimeout(int ms);
};

//----------------------------------------------------------------------------
//  OsNet::TCPCONNECT (0x6F9757B4) - 5 slots.  A small standalone object used
//  while an outbound TCP connect is in flight - not NETCONN-derived (see
//  the file header).  Layout recovered from the three functions below:
//  m_socket at +0x04 (the position slot 1's shared-address IsSocketClosed
//  body implies), an unread gap, m_ctx at +0x10 (read internally by the
//  thunked Announce helper, 0x6F6D9BC0), another gap, m_addr/m_port at
//  +0x1C/+0x20 (a raw in_addr and a host-order port, assembled into a
//  sockaddr_in by slot 3) and m_notifyProc at +0x24.
//----------------------------------------------------------------------------
class TCPCONNECT
{
public:
    //  Slot 0 - 0x6F6D9CF0, retn 8h.  A two-way dispatch on `reason`:  1
    //  calls slot 4 (DeleteSelf-shaped tail) with the connection's socket
    //  handle, 2 calls slot 3 (the abort/notify path) with no argument.
    //  Named from the shape - nothing in either dump calls this slot
    //  directly, so it is reached only through a caller outside both call
    //  trees (the outbound-connect state machine that owns these objects).
    virtual void OnConnectEvent(int reason, void* arg);
    //  Slot 1 - identical body to NETCONN::IsSocketClosed
    //  (`return m_socket == INVALID_SOCKET`), linker-folded onto the exact
    //  same address (0x6F6D9A10) - see the file header.  Not separately
    //  funcmapped.
    virtual BOOL IsSocketClosed() { return m_socket == INVALID_SOCKET; }
    //  Slot 2 - 0x6F6E6C10, retn 4h.  Registers this connect attempt into
    //  its owner's select-bucket table under both the read and write
    //  reasons.  See osnet_family_thunks-shaped helpers below.
    virtual void RegisterForSelect(void* mgr);
    //  Slot 3 - 0x6F6D9C60, retn 0.  Assemble {AF_INET, m_port, m_addr}
    //  into a local sockaddr_in, announce reason 3 through m_notifyProc,
    //  then close the socket if still open.  The abort path: something
    //  told this attempt to give up before it connected.
    virtual void Abort();
    //  Slot 4 - 0x6F6E3EC0, retn 4h.  Tail-calls a shared "finish or fail
    //  the connect" helper (0x6F6E3880, out of this session's scope,
    //  thunked) with `this` reinterpreted as its own first argument and the
    //  real `this` passed through as a stack argument - the same
    //  "handed a differently-typed pointer, calls into a shared finisher"
    //  shape LOOPCONNECT's and FILECONNECT's own slot 4 use.
    virtual void OnConnectResult(int arg);

    SOCKET      m_socket;              // +0x04
    char        m_reserved08[8];       // +0x08 .. +0x0F
    void*       m_ctx;                 // +0x10
    char        m_reserved14[8];       // +0x14 .. +0x1B
    unsigned int m_addr;                // +0x1C - raw in_addr (network order)
    unsigned short m_port;             // +0x20 - host order, htons'd on use
    char        m_reserved22[2];
    CONNNOTIFYPROC m_notifyProc;       // +0x24
};

//----------------------------------------------------------------------------
//  OsNet::FILECONNECT (0x6F9757CC) - 5 slots.  The file-connect twin of
//  TCPCONNECT above; same standalone shape, m_addr/m_port replaced by
//  whatever a file-open attempt tracks (not read by anything in scope this
//  session).
//----------------------------------------------------------------------------
class FILECONNECT
{
public:
    //  Slot 0 - `nullsub_42` in the shipped table (not this session's - no
    //  dump, not separately funcmapped).  Trivial, same reserved-slot
    //  convention netconn.h's own purecall slots use.
    virtual void m_reservedSlot0() {}
    //  Slot 1 - 0x6F6DEE00, retn 0.  `return this->+0x1C == -1;` - an
    //  "attempt still pending" test (the file-handle-or-sentinel position
    //  TCPCONNECT's own m_addr occupies at the same offset; here it holds a
    //  handle-shaped value instead).  Defined out-of-line (fileconnect.cpp)
    //  for the same emission reason NETCONNFULL::SetCoalesce's own comment
    //  gives.
    virtual BOOL IsPending();
    //  Slot 2 - `nullsub_1732` in the shipped table (not this session's -
    //  trivial, same reserved-slot convention as slot 0 above).
    virtual void m_reservedSlot2() {}
    //  Slot 3 - 0x6F6D9D40, retn 0.  Same "assemble a zeroed local buffer,
    //  announce reason 3 through the field at +0x20, close" shape as
    //  TCPCONNECT::Abort, reading the notify field from +0x20 rather than
    //  +0x24 (FILECONNECT's own fields sit four bytes earlier than
    //  TCPCONNECT's).
    virtual void Abort();
    //  Slot 4 - 0x6F6E3ED0, retn 4h.  Same shared-finisher tail-call shape
    //  as TCPCONNECT::OnConnectResult, into 0x6F6E38E0.
    virtual void OnConnectResult(int arg);

    //  Only 0x18 bytes here, not 0x1C - the implicit vtable pointer this
    //  class's own virtuals give it already occupies the first four
    //  (+0x00..+0x03), so this array only needs to cover +0x04..+0x1B.
    char        m_reserved04[0x18];    // +0x04 .. +0x1B
    int         m_handleOrSentinel;    // +0x1C
    CONNNOTIFYPROC m_notifyProc;       // +0x20
};

//----------------------------------------------------------------------------
//  OsNet::TCPLISTEN (0x6F9757E4) - 3 slots.  vtable_classes.json's own name
//  for this class collides with an *already-reconstructed*, non-virtual
//  `struct TCPLISTEN` in ostcp.h (the slotted-list element TCPMGR::m_listens
//  holds, with its own Close()/CloseSocket() at 0x6F6E1370/0x6F6E14A0 - see
//  that struct's own comment) - same RTTI-shaped SMemFree tag
//  (`.?AUTCPLISTEN@OsNet@@`), and this class's own m_socket-at-+0x04 layout
//  is consistent with that struct being the same object with an unaccounted
//  vtable pointer at +0x00 pushing every field down by four.  Reshaping the
//  existing struct to prove that is real, cross-file work touching several
//  already-scored call sites (ostcp_listenclosesocket.cpp,
//  ostcp_listenslotsreleaseall.cpp, ostcp_slotiternext.cpp,
//  ostcp_stopselectthreads.cpp, ostcp_thunks.cpp) that is out of this
//  session's scope and too risky to do blind under a "no regressions" rule -
//  so this class is named TCPLISTENSELECT here instead of colliding, with
//  this note left for whoever verifies (or disproves) the vptr-shift theory
//  next.  The two addresses below are real either way.
//----------------------------------------------------------------------------
class TCPLISTENSELECT
{
public:
    //  Slot 0 - 0x6F6E3DD0, retn 8h.  `reason` gates whether this is a real
    //  accept-ready event: 0 means "finish it" (accept(); if the listener
    //  is still live, getsockname() and the shared AcceptFinisher tail
    //  (0x6F6E3830, out of scope and thunked) - otherwise the accepted
    //  socket is simply closed).  Any other reason is a no-op - a listening
    //  socket has nothing else the select pump would report.
    virtual void OnSelectEvent(int reason, void* owner);
    //  Slot 1 - identical body to NETCONN::IsSocketClosed, linker-folded
    //  onto the exact same address (0x6F6D9A10) - see the file header.
    //  Not separately funcmapped.
    virtual BOOL IsSocketClosed() { return m_listenSocket == INVALID_SOCKET; }
    //  Slot 2 - 0x6F6E6BF0, retn 4h.  Registers this listener into its
    //  owner's select read-bucket only (no write bucket - a listening
    //  socket is never "writable").  Same shared helper shape as
    //  TCPCONNECT::RegisterForSelect.
    virtual void RegisterForSelect(void* mgr);

    //  No leading reserved field here - the implicit vtable pointer this
    //  class's own virtuals give it already occupies +0x00..+0x03, so
    //  m_listenSocket (the real object's +0x04) is this class's first
    //  declared member.
    SOCKET      m_listenSocket;         // +0x04
    char        m_reserved08[0x14];     // +0x08 .. +0x1B
    int         m_stillListening;       // +0x1C
};

//----------------------------------------------------------------------------
//  OsNet::LOOPCONNECT (0x6F9757F4) - 5 slots.  The loopback-pair twin of
//  TCPCONNECT/FILECONNECT above.
//----------------------------------------------------------------------------
class LOOPCONNECT
{
public:
    //  Slot 0 - `nullsub_43` in the shipped table (not this session's -
    //  trivial, same reserved-slot convention FILECONNECT's own slot 0
    //  uses).
    virtual void m_reservedSlot0() {}
    //  Slot 1 - 0x6F6DFF60, retn 0.  `return FALSE;` unconditionally - a
    //  loopback attempt never has a real socket to have closed.  Defined
    //  out-of-line (loopconnect.cpp) for the same emission reason
    //  NETCONNFULL::SetCoalesce's own comment gives.
    virtual BOOL IsSocketClosed();
    //  Slot 2 - `nullsub_1731` in the shipped table (not this session's -
    //  trivial, same reserved-slot convention as slot 0 above).
    virtual void m_reservedSlot2() {}
    //  Slot 3 - 0x6F6D9C10, retn 0.  Same "zeroed local buffer, announce
    //  reason 3 through the stored proc, no socket to close" shape as
    //  TCPCONNECT::Abort - the proc lives at +0x1C here (one field earlier
    //  again, matching FILECONNECT's own +0x20 vs TCPCONNECT's +0x24).
    virtual void Abort();
    //  Slot 4 - 0x6F6E3EA0, retn 4h.  Same shared-finisher tail-call shape,
    //  into 0x6F6E3860 - except this one has a real branch first: if
    //  `this+0x20` is already zero it skips the finisher and calls slot 3
    //  (Abort) directly instead.
    virtual void OnConnectResult(int arg);

    //  Only 0x18 bytes here - the implicit vtable pointer already occupies
    //  +0x00..+0x03, same as FILECONNECT's own m_reserved04 above.
    char        m_reserved04[0x18];    // +0x04 .. +0x1B
    CONNNOTIFYPROC m_notifyProc;       // +0x1C
    int         m_pending;             // +0x20
};

//----------------------------------------------------------------------------
//  OsNet::LOOPCONN (0x6F97580C) - 15 slots.  `: public NETCONN`, adding a
//  peer pointer (+0x68 - the other LOOPCONN half of the pair) plus its own
//  outbound queue (+0x78, TSList-shaped like TCPCONN's own m_sendQueue).
//  "LOOP" is a same-process loopback pair, not a real socket past
//  construction - Close() (slot 9) releases *both* halves' references.
//----------------------------------------------------------------------------
class LOOPCONN : public NETCONN
{
public:
    //  Slot 1 - IsSocketClosed override.  0x6F6DBF90, retn 0.  Takes the
    //  connection lock, then returns whether the peer pointer (+0x68) is
    //  null - "closed" means "unpaired", not "no live socket", since a
    //  loopback connection never has a real one.
    virtual BOOL IsSocketClosed();
    //  Slot 3 - OnPendingDrained override.  0x6F6E2400, retn 0.  Unlinks
    //  both halves' peer pointers, releases each half's reference, sets the
    //  paired event and releases this half's own refcount too - the whole
    //  pair tears down together.
    virtual void OnPendingDrained();
    //  Slot 4 - DeleteSelf override.  0x6F6E23D0, retn 4h.  Calls a real,
    //  out-of-line destructor (0x6F6E0130, out of scope, thunked) rather
    //  than inlining one - LOOPCONN has its own fields to tear down (the
    //  send queue), the same reason TCPCONN::DeleteSelf is a real call
    //  rather than NETCONN::DeleteSelf's inlined one.
    virtual LOOPCONN* DeleteSelf(int flags);
    //  Slot 9 - Close override (NETCONN's own Close, 0x6F6DA450, is *not*
    //  reused here).  0x6F6DBF70, retn 0.  Takes the peer-list lock and
    //  tail-jumps into slot 3 (OnPendingDrained) - the same lock-then-
    //  dispatch shape NETCONN::Close itself has, just against LOOPCONN's
    //  own lock field rather than the connection lock NETCONN::Close uses.
    virtual void Close();
    //  Slot 10 - CanAppendToHead-position.  0x6F6DCF30, retn 0.  Same shape
    //  as TCPCONN::CanAppendToHead: true when the peer exists and its head
    //  queue buffer is still open for appending.
    virtual BOOL CanAppendToHead();
    //  Slot 11 - Send-position.  0x6F6E0340, retn 8h.  When `len` is
    //  nonzero, forwards the write straight into the peer's own queue
    //  (0x6F6E0270, out of scope, thunked) under the peer-list lock, then
    //  signals the peer's wake event if the peer is present.
    virtual void Send(const void* buf, int len);
    //  Slot 12 - SetCoalesce-position, same address NETCONNFULL's own
    //  slot 12 resolves to (0x6F6DE980, linker-folded - see the file
    //  header, not this session's - not separately funcmapped).
    //  `retn 4` alone - a no-op.
    virtual void SetCoalesce(int coalesce) { (void)coalesce; }
    //  Slot 13 - SetBufferSizes-position, same address NETCONNFULL's own
    //  slot 13 resolves to (0x6F6DE990, linker-folded - see the file
    //  header).  `xor eax,eax; retn 4` - always false, same no-op shape.
    virtual BOOL SetBufferSizes(int size) { (void)size; return FALSE; }
    //  Slot 14 - SetRecvTimeout-position, same address NETCONNFULL's own
    //  slot 14 resolves to (0x6F6DE9A0, linker-folded - not separately
    //  funcmapped).  `retn 4` alone - a no-op.
    virtual int SetRecvTimeout(int ms) { (void)ms; return 0; }

    LOOPCONN*   m_peer;                 // +0x68
    char        m_reserved6C[8];        // +0x6C .. +0x73
    void*       m_queueLink;            // +0x74 (TSList-shaped, unread here)
    void*       m_queueHeadNext;        // +0x78
    int         m_queueHeadPrevlink;    // +0x7C
};

//----------------------------------------------------------------------------
//  OsNet::UDPCONN (0x6F97584C) - 11 slots.  `: public NETCONN`.  A
//  connectionless (SOCK_DGRAM) socket wrapper.
//----------------------------------------------------------------------------
class UDPCONN : public NETCONN
{
public:
    //  Slot 0 - 0x6F6DA680, retn 8h.  Two stack arguments, only the second
    //  (`reason`) is ever read - `reason != 0` is a no-op, same gate
    //  TCPLISTEN::OnSelectEvent's own slot 0 uses.  `reason == 0`:
    //  recvfrom() into a 0x5B4-byte local buffer (TCPCONN's own
    //  kTcpSegment), then NotifyData with the sender's address as the
    //  override addrs pointer (matching netconn.h's own note that
    //  NotifyData's `addrs` parameter exists for exactly a datagram path to
    //  report a per-packet source).  Compiled with a /GS stack cookie this
    //  build's fixed /GS- can never reproduce - see
    //  docs/msvc-vc8-idioms.md.
    virtual void OnReadable(int unused0, int reason);
    //  Slot 2 - 0x6F6E6DE0, retn 4h.  Registers this socket into its
    //  owner's select read-bucket only, same shape as TCPLISTEN's own
    //  slot 2.
    virtual void RegisterForSelect(void* mgr);
    //  Slot 3 - 0x6F6DA650, retn 0.  Close the socket if still open, then
    //  release the connection lock - UDPCONN's own OnPendingDrained
    //  override (matches TCPCONN::OnPendingDrained's position and shape,
    //  minus the notify).
    virtual void OnPendingDrained();
    //  Slot 4 - DeleteSelf override.  0x6F6E2490, retn 4h.  Calls a real
    //  out-of-line destructor (0x6F6E0440, out of scope, thunked).
    virtual UDPCONN* DeleteSelf(int flags);
    //  Slot 10 - Send-position.  0x6F6DA5F0, retn 10h.  `destAddrs == 0` is
    //  a Storm assert (SErrSetLastError(0x57)) and an immediate return -
    //  every caller is expected to supply a real destination array.
    //  `count == 0` is a quiet no-op.  Otherwise, one sendto() per entry of
    //  `destAddrs` (each 0x10 bytes, sockaddr_in-sized), walked from the
    //  last entry back to the first.
    virtual void Send(const void* buf, int len, const void* destAddrs,
                      int count);
};

//----------------------------------------------------------------------------
//  OsNet::FILECONN (0x6F97590C) - 12 slots.  `: public NETCONN`, adding a
//  pending-I/O count (+0x68, matching IOTCPCONN's own m_pending shape) and
//  a file handle (+0x6C) in place of TCPCONN's socket-flavoured extras.
//  IOFILECONN and SLFILECONN (below) both derive from this rather than
//  duplicating its slots 1/5/6/7/8/9, the same pattern SLTCPCONN already
//  uses over TCPCONN.
//----------------------------------------------------------------------------
class FILECONN : public NETCONN
{
public:
    //  Slot 1 - IsSocketClosed-position, FILECONN's own (not NETCONN's
    //  0x6F6D9A10 - the field tested is the file handle at +0x6C, not the
    //  socket at +0x04).  0x6F6E07F0, retn 0.  Defined out-of-line
    //  (fileconn_deleteself.cpp) for the same emission reason
    //  NETCONNFULL::SetCoalesce's own comment gives.
    virtual BOOL IsSocketClosed();
    //  Slot 4 - DeleteSelf.  0x6F6E3970, retn 4h.  Calls a real, out-of-line
    //  destructor (0x6F6E2650, out of this session's scope, thunked) rather
    //  than inlining one, the same reason TCPCONN::DeleteSelf is a real
    //  call rather than NETCONN::DeleteSelf's inlined one.
    virtual FILECONN* DeleteSelf(int flags);
    //  Slot 5 - AddPending.  0x6F6DAB30, retn 0.
    //  `InterlockedIncrement(&this->+0x68)`.  Defined out-of-line
    //  (fileconn_deleteself.cpp) for the same emission reason
    //  NETCONNFULL::SetCoalesce's own comment gives.
    virtual void AddPending();
    //  Slot 6 - ReleasePending-position.  0x6F6E0800, retn 0.  Drop one
    //  pending-I/O reference; on reaching zero, call slot 3
    //  (OnPendingDrained) then Shutdown(1) through a (TCPCONN*)this
    //  reinterpret - see the file header for why that reinterpret is sound.
    virtual void ReleasePending();
    //  Slot 7 - OnSendComplete-position (a completed write).  0x6F6E0840,
    //  retn 0Ch.  Unlink the completed node from the write queue, notify
    //  through the shared FileConnNotify helper (reason 4, out of scope,
    //  thunked), then restart the queue drain if that notify said to.
    virtual void OnWriteComplete(void* node, void* unused, DWORD bytes);
    //  Slot 8 - OnRecvComplete-position (a completed read).  0x6F6E2740,
    //  retn 0Ch.  Same unlink-then-notify shape as slot 7, reason 5.
    virtual void OnReadComplete(void* node, DWORD bytes, BOOL ok);
    //  Slots 10/11 - `_purecall` in FILECONN itself; IOFILECONN and
    //  SLFILECONN each give them real, differently-shaped bodies below.
    virtual void m_reservedSlot10() {}
    virtual void m_reservedSlot11() {}

    //  Four bytes between NETCONN's last field (m_owner at +0x60, ending at
    //  +0x64) and FILECONN's own first one - the same gap TCPCONN's own
    //  m_reserved64 is (ostcp.h), and for the same reason: nothing in
    //  either dump reads +0x64..+0x67 directly, so which side of the
    //  boundary it belongs to is not recoverable.
    char        m_reserved64[4];        // +0x64
    int         m_pending;              // +0x68
    HANDLE      m_hFile;                 // +0x6C
    char        m_reserved70[8];        // +0x70 .. +0x77
    void*       m_queueHeadNext;        // +0x78
    int         m_queueHeadPrevlink;    // +0x7C
    void*       m_queueSentinel;        // +0x80
};

//----------------------------------------------------------------------------
//  OsNet::IOFILECONN (0x6F97599C) - 12 slots, `: public FILECONN`.  The
//  overlapped-I/O completion-port variant, the file-connection twin of
//  IOTCPCONN.
//----------------------------------------------------------------------------
class IOFILECONN : public FILECONN
{
public:
    //  Slot 3 - OnPendingDrained override.  0x6F6DAC00, retn 0.
    //  CloseHandle the file if open, release the connection lock, then post
    //  a zero-byte completion to the I/O completion port so the pump wakes
    //  up and finishes tearing this connection down.
    virtual void OnPendingDrained();
    //  Slot 4 - DeleteSelf override.  0x6F6E39A0, retn 4h.  Same shared
    //  destructor as FILECONN::DeleteSelf (0x6F6E2650) - IOFILECONN adds no
    //  fields of its own past FILECONN's.
    virtual IOFILECONN* DeleteSelf(int flags);
    //  Slot 10 - the write-side PostRead equivalent.  0x6F6DAB40, retn 4h.
    //  AddPending() through the vtable (slot 5), then an overlapped
    //  WriteFile using the queue node handed in; on a synchronous failure
    //  that is not ERROR_IO_PENDING, posts a zero-byte completion so the
    //  pump notices.
    virtual void PostWrite(void* node);
    //  Slot 11 - the read-side arm.  0x6F6DABB0, retn 4h.  Same shape as
    //  PostWrite, an overlapped ReadFile.
    virtual void PostRead(void* node);
};

//----------------------------------------------------------------------------
//  OsNet::SLFILECONN (0x6F9759D0) - 12 slots, `: public FILECONN`.  The
//  event-driven (select-style) variant - adds a wait event handle at +0x8C
//  in place of IOFILECONN's overlapped machinery.
//----------------------------------------------------------------------------
class SLFILECONN : public FILECONN
{
public:
    //  Slot 3 - OnPendingDrained override.  0x6F6DAC90, retn 0.  Same
    //  CloseHandle-then-lock-release shape as IOFILECONN's own, signalling
    //  the wait event (SetEvent) instead of posting a completion.
    virtual void OnPendingDrained();
    //  Slot 4 - DeleteSelf override.  0x6F6E39D0, retn 4h.  Its own
    //  destructor (0x6F6E2A10, out of scope) tears down the wait-event
    //  handle first, then tail-jumps into FILECONN's shared 0x6F6E2650.
    virtual SLFILECONN* DeleteSelf(int flags);
    //  Slot 10 - the write-side "go" signal.  0x6F6DAC50, retn 4h.
    //  AddPending() through the vtable, release the connection lock, then
    //  SetEvent on the wait event - the actual write happens on whatever
    //  thread the event wakes, not here.
    virtual void PostWrite(void* node);
    //  Slot 11 - the read-side "go" signal.  0x6F6DAC80, retn 0.  Just
    //  SetEvent on the wait event - three instructions, no lock, no
    //  AddPending.
    virtual void PostRead();

    char        m_reserved84[8];        // +0x84 .. +0x8B
    HANDLE      m_waitEvent;             // +0x8C
};

}  // namespace OsNet

#endif
