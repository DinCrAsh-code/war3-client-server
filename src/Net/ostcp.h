//============================================================================
//  Storm's Winsock layer - `.\W32\OsTcp.cpp`, which is not a guess: every
//  allocation in this module tags itself with that literal filename, and the
//  four vftable stores name the classes outright
//  (`??_7NETCONN@OsNet@@6B@`, `??_7TCPCONN@OsNet@@6B@`,
//  `??_7SLTCPCONN@OsNet@@6B@`, `??_7IOTCPCONN@OsNet@@6B@`).  The send buffer's
//  own name comes from the free that releases it, which passes Storm the
//  element's RTTI descriptor instead of a filename: `.?AUOUTPUT@OsNet@@`,
//  i.e. `struct OsNet::OUTPUT`.
//
//  The shape of the module:
//
//    NETCONN  (netconn.h) is the socket-agnostic base - refcount, state,
//             address pair, notification callback, lock.
//    TCPCONN  adds the outbound queue and the receive buffer.
//    SLTCPCONN ("select") is TCPCONN with nothing added: it is what
//             CreateConnection builds when the manager has no completion
//             port, and it is 0x630 bytes, exactly sizeof(TCPCONN).
//    IOTCPCONN ("I/O completion") adds an outstanding-I/O count and the
//             OVERLAPPED it arms receives with, and is 0x650 bytes.
//    TCPMGR   owns the completion port, the four connection state lists and
//             the accept path.
//
//  Two things about the layout are worth stating up front because they pin
//  down every size in the file.  `TCPCONN::m_recvBuf` is 0x5B4 bytes, which is
//  the same number `OUTPUT`'s constructor uses as a floor for a send buffer's
//  capacity - 1460, the Ethernet TCP MSS.  And the four bytes at +0x62C exist
//  only because IOTCPCONN's first own member has to land at +0x630: without
//  them sizeof(TCPCONN) would be 0x62C and both derived sizes would be four
//  short of the two SMemAlloc constants the call tree hands us.
//============================================================================
#ifndef OSTCP_H
#define OSTCP_H

//  Ahead of <windows.h> (which netconn.h pulls in) because the default
//  windows.h drags in Winsock 1, and this module is Winsock 2 throughout:
//  WSABUF, WSARecv's signature and WSAEWOULDBLOCK's spelling all come from
//  here.  winsock2.h defines _WINSOCKAPI_ itself, which is what stops the
//  older header being included behind it.
#include <winsock2.h>
#include <windows.h>

//  This module is the first thing in src/ to reference ws2_32 (socket, send,
//  WSAGetLastError).  The pipeline build only ever compiles, so it never
//  noticed; matching_decomp_injection's .mix link does, and its link line
//  names Kernel32.Lib explicitly and nothing else.  /Zl suppresses the
//  *default* library record, not an explicit one, so this directive is what
//  gets ws2_32 onto that link (its directory is already on the /libpath).
//  It emits no code and changes no listing.
#pragma comment(lib, "ws2_32.lib")

#include "netconn.h"
#include "storm.h"
#include "os.h"      // OsInterlockedIncrement - the AddRef every start here takes

namespace OsNet
{

//----------------------------------------------------------------------------
//  An overlapped operation plus the tag TCPMGR::PumpIocp dispatches on.
//
//  The tag sits immediately past the five dwords of OVERLAPPED, which is how
//  the pump can read it off a bare `LPOVERLAPPED` (`[edx+14h]`) without
//  knowing which object the operation belongs to.  Only three values occur:
//  0 for a receive, 1 for a send and 2 for an accept.
//----------------------------------------------------------------------------
struct IOOP
{
    OVERLAPPED  m_ov;       // +0x00
    int         m_type;     // +0x14
};                          // 0x18

//----------------------------------------------------------------------------
//  What a *send* operation really is.
//
//  TCPCONN::OnSendComplete stamps a 2 four bytes past the end of the IOOP it
//  was handed, so the send path's operations live at the front of something
//  bigger.  That the extra word is outside IOOP and not part of it is fixed by
//  TCPACCEPT, which embeds an IOOP at +0x08 and puts its CRITICAL_SECTION at
//  +0x20: 0x20 - 0x08 = 0x18, so IOOP is exactly the OVERLAPPED and the tag.
//  (Modelling it as 0x1C instead pushed every TCPACCEPT field past the lock
//  down by four and took Rearm from 54/54 to 46/54 - the check that caught it.)
//
//  Nothing in these call trees constructs one of these, so only the field the
//  send completion writes is named.
//----------------------------------------------------------------------------
struct SENDOP
{
    IOOP        m_op;       // +0x00
    int         m_state;    // +0x18 - OnSendComplete sets 2
};

const int kIoOpRecv   = 0;
const int kIoOpSend   = 1;
const int kIoOpAccept = 2;

//: The floor on a send buffer's capacity and the exact size of the receive
//: buffer - 1460 bytes, the TCP maximum segment size over Ethernet.
const unsigned int kTcpSegment = 0x5B4;

//: NETCONN::m_state when the connection is on none of TCPMGR's four lists.
//: The constructor starts there and MoveConn is the only writer.
const int kConnStateNone = 4;

//----------------------------------------------------------------------------
//  One queued outbound buffer.
//
//  The constructor zeroes seven dwords and sets +0x1C to 1 - the same
//  "born with one reference" shape NETCONN itself has.  The five dwords
//  between the link and that refcount keep reserved names because nothing
//  ever reads them *as* five separate dwords - but IOTCPCONN::OnQueued
//  (ostcp.h, 0x6F6DA8D0) takes `&buf->m_reserved08` and hands the whole
//  0x14-byte run to WSASend/WriteFile as its `LPOVERLAPPED`, i.e. it is a
//  per-buffer scratch OVERLAPPED, not dead space; the block from +0x20 on
//  is entirely accounted for by TCPCONN::Queue, which is the only thing
//  that ever fills one in.
//----------------------------------------------------------------------------
struct OUTPUT
{
    OUTPUT()
    {
        m_reserved08 = 0;
        m_reserved0C = 0;
        m_reserved10 = 0;
        m_reserved14 = 0;
        m_reserved18 = 0;
        m_refcount = 1;
    }

    TSLink<OUTPUT>  m_link;         // +0x00 - first member, hence LINKOFFSET 0
    //  +0x08 .. +0x1C - scratch OVERLAPPED for IOTCPCONN::OnQueued's own
    //  overlapped send.  See the struct comment above.
    int             m_reserved08;   // +0x08
    int             m_reserved0C;   // +0x0C
    int             m_reserved10;   // +0x10
    int             m_reserved14;   // +0x14
    int             m_reserved18;   // +0x18
    int             m_refcount;     // +0x1C - ctor sets 1
    //  Not a flag - a state, and the value that matters is 2.  Queue() is
    //  born at 0 and only ever tops up a head buffer that is still 0;
    //  OnSendComplete drains and frees every tail buffer that has reached 2
    //  and stops at the first one that has not.  IOTCPCONN::OnQueued is the
    //  in-flight setter: it stamps 1 right before the overlapped send goes
    //  out, so 0 is "still open for appending", 1 is "in flight" and 2 is
    //  "fully sent, may be released".
    int             m_sendState;    // +0x20
    DWORD           m_tick;         // +0x24 - when it was queued
    unsigned int    m_used;         // +0x28
    unsigned int    m_capacity;     // +0x2C
    void*           m_data;         // +0x30
};

//----------------------------------------------------------------------------
//  The send queue itself.
//
//  A plain TSList would do for the linking, but the queue also has to be able
//  to *release* what is on it, and that needs the element type: FreeAll()
//  frees each buffer's payload with the module's own filename tag and then
//  the buffer with the element's RTTI descriptor name, which is a
//  T-dependent pair a generic list has no way to produce.  Hence the derived
//  class, which adds no data and inherits the destructor unchanged.
//----------------------------------------------------------------------------
class OUTPUTLIST : public TSList<OUTPUT, 0>
{
public:
    //  0x6F6DF340
    void FreeAll();
};

//----------------------------------------------------------------------------
//  A TCP connection with an outbound queue.
//
//  m_sendQueue is a TSList (compile-time link offset), not a TSExplicitList:
//  the constructor stores 0 into m_linkoffset and every walk in the module
//  dereferences a tail value directly as a node, with no add - see storm.h.
//----------------------------------------------------------------------------
class TCPCONN : public NETCONN
{
public:
    //  0x6F6E04A0 - retn 14h.  The five arguments are NETCONN's own, passed
    //  straight through; everything this constructor adds on top is the queue,
    //  the receive cursor, SO_KEEPALIVE and FIONBIO.
    TCPCONN(TCPMGR* owner, SOCKET s, CONNNOTIFYPROC proc, void* ctx,
            const void* addrs);

    //  0x6F6E0520
    ~TCPCONN();

    //  TCPCONN's own vtable (@ 0x6F97587C) extends NETCONN's ten slots to
    //  seventeen.  It overrides five of the inherited ones - slot 3
    //  (OnPendingDrained, 0x6F6DA7D0), slot 4 (DeleteSelf, 0x6F6E24C0) and
    //  the two completion callbacks, slot 7 (OnSendComplete, 0x6F6E05C0) and
    //  slot 8 (OnRecvComplete, 0x6F6DA720) - and adds slots 10..16 below.
    //  IOTCPCONN's table (@ 0x6F975954) is the same shape.
    //
    //  Slot 3 (+0x0C) - close the socket if it is still open, then release
    //  the lock Close() (NETCONN, slot 9) took before tail-jumping here.
    //  `_purecall` in the base; this is TCPCONN's own body, inherited
    //  unchanged by IOTCPCONN.
    virtual void OnPendingDrained();
    //  Slot 4 (+0x10) - the scalar deleting destructor for a `virtual
    //  ~TCPCONN()` (see netconn.h's slot 4 comment for the shape and the
    //  `DeleteSelf` rename).  `this->TCPCONN::~TCPCONN()` is a real `call`
    //  here (0x6F6E0520, defined in ostcp_conndtor.cpp), which is why this
    //  override carries the `this != 0` test the inlined NETCONN one does
    //  not - see docs/msvc-vc8-idioms.md.  Defined in its own translation
    //  unit (tcpconn_deleteself.cpp) so that call cannot inline.
    virtual TCPCONN* DeleteSelf(int flags);
    //  Slot 10 (+0x28) - 0x6F6DCF80, TCPCONN's, inherited by IOTCPCONN.
    //
    //  Under the connection lock: if the socket is still open and the send
    //  queue's head buffer hasn't been handed to a real send yet
    //  (`OUTPUT::m_sendState == 0`, "still open for appending" per
    //  ostcp.h's own comment on that field), returns true.  Reads
    //  exactly like the guard `Queue()` would want before appending to an
    //  existing head rather than allocating a fresh buffer, but nothing in
    //  either dump actually calls this slot, so the name is inferred from
    //  the body alone.
    virtual BOOL CanAppendToHead();
    //  Slot 11 (+0x2C) - Send.  It is a *virtual*, which the vtable dump is
    //  what revealed; every direct `call` to it in the dumps is from a
    //  constructor, and a virtual call inside a constructor is exactly what
    //  compiles to a direct call.
    //
    //  0x6F6DF040 - retn 8.  Try the socket first, queue whatever is left.
    virtual void Send(const void* buf, int len);
    //  Slots 12..14 (+0x30/+0x34/+0x38) - 0x6F6DA800, 0x6F6DA830, 0x6F6DA890.
    //  TCPCONN's, inherited by IOTCPCONN, called by nothing dumped so far -
    //  three setsockopt wrappers, named for what each sets.  See
    //  tcpconn_sockopts.cpp.
    //
    //  Slot 12 - `setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, ...)` with
    //  the flag inverted (`!coalesce`): a nonzero `coalesce` leaves Nagle's
    //  algorithm on, zero disables it (TCP_NODELAY set).
    virtual void SetCoalesce(int coalesce);
    //  Slot 13 - sets SO_SNDBUF to `size`, and only on success sets
    //  SO_RCVBUF to the same value; returns whether the second call
    //  succeeded (false on either failure).
    virtual BOOL SetBufferSizes(int size);
    //  Slot 14 - `setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, ...)`,
    //  returning the raw `setsockopt` result.
    virtual int SetRecvTimeout(int ms);
    //  Slot 15 (+0x3C) - "start draining the queue".  `_purecall` in TCPCONN,
    //  IOTCPCONN 0x6F6DA8D0.
    //
    //  It takes ownership of the connection lock.  Both callers - Send() and
    //  OnSendComplete() - return straight out of the branch that reaches it
    //  with no LeaveCriticalSection in front of them, while every other exit
    //  in both goes through one.  Getting that wrong is a double unlock, and
    //  it is invisible to the score: see ostcp_send.cpp.
    virtual void OnQueued(OUTPUT* /*buf*/) {}
    //  Slot 16 (+0x40) - arm the next receive.  `_purecall` in TCPCONN,
    //  IOTCPCONN 0x6F6DA990.  Also virtual, same evidence as Send.
    virtual void PostRead() {}

    //  0x6F6DEEC0 - retn 8.  Append to the queue, returning the buffer the
    //  data ended up in (the head one if it had room, otherwise a fresh one).
    OUTPUT* Queue(const void* src, unsigned int size);

    //  0x6F6E00C0 - retn 4.  Close, unlist, optionally notify, release.
    void    Shutdown(int notify);

    //  0x6F6DA720 - retn 0Ch, slot 8.  Fold the transferred bytes into the
    //  receive buffer, hand complete messages up, compact and re-arm.
    virtual void OnRecvComplete(IOOP* op, DWORD bytes, BOOL ok);

    //  0x6F6E05C0 - retn 0Ch, slot 7.  Release every fully-sent buffer off
    //  the tail of the queue, then restart the drain.
    virtual void OnSendComplete(IOOP* op, DWORD bytes, BOOL ok);

    //  Four bytes between the last field NETCONN's constructor writes
    //  (m_owner at +0x60) and the first one this class's writes (+0x68).
    //  Which side of the boundary they belong to is not recoverable - no
    //  function in either call tree reads or writes +0x64 - so they are
    //  parked here rather than grown onto NETCONN, whose own size is already
    //  fixed by two matching functions and by matching_decomp_injection's
    //  hook set.
    char                m_reserved64[4];        // +0x64
    OUTPUTLIST          m_sendQueue;            // +0x68 (12 bytes)
    unsigned int        m_recvUsed;             // +0x74
    char                m_recvBuf[kTcpSegment]; // +0x78 .. +0x62B
    //  Not a real field: padding that has to be here for IOTCPCONN's own
    //  members to start at +0x630.  See the header comment.
    char                m_reserved62C[4];       // +0x62C
};

//----------------------------------------------------------------------------
//  0x6F6E06B0 - the select-driven connection.  Adds no data at all; the whole
//  derived class is one constructor and a vftable.
//
//  Its own vtable (@ 0x6F9758C4, user_knowledge.json) turned out to override
//  five of TCPCONN's slots rather than none - slots 0, 2, 4 and 15 (the
//  fifth, slot 16 PostRead, is `nullsub_1737` in the shipped table and is
//  not part of this session's worklist; TCPCONN's own trivial `{}` PostRead
//  is behaviourally identical - "arm nothing" either way - so it is left
//  inherited rather than re-declared for an address nobody has dumped).
//  Slots 3 (OnPendingDrained), 5-14 all still resolve to TCPCONN's own
//  addresses (0x6F6DA7D0, nullsub_1740/1741, 0x6F6E05C0, 0x6F6DA720,
//  0x6F6DA450, 0x6F6DCF80, TCPCONN::Send @ 0x6F6DF040, the three sockopt
//  setters), i.e. genuinely inherited unchanged, not merely coincidentally
//  identical - so this class still adds no data, only new vtable slots.
//----------------------------------------------------------------------------
class SLTCPCONN : public TCPCONN
{
public:
    SLTCPCONN(TCPMGR* owner, SOCKET s, CONNNOTIFYPROC proc, void* ctx,
              const void* addrs, const void* buf, int len);

    //  Slot 0 (+0x00) - `_purecall` in TCPCONN (m_reservedSlot0, netconn.h).
    //  0x6F6DD090, retn 8h.  A two-way dispatch on the reason code the
    //  select pump hands it: 0 tail-calls slot 9 (Close) through
    //  sub_6F6DAAA0 (not this session's - out of scope, thunked), 1 calls
    //  sub_6F6DCFC0 (also thunked).  Named from the shape alone - a select
    //  loop's per-socket event callback, `(fd, reason)` - since nothing in
    //  either dump calls it directly (it is reached only through the vtable
    //  the select pump itself owns, which this call tree does not include).
    virtual void OnSelectEvent(int unused0, int reason);
    //  Slot 2 (+0x08) - `_purecall` in TCPCONN (m_reservedSlot2).
    //  0x6F6E6E00, retn 4h.  Registers this connection's socket into the
    //  select pump's own read-bucket (reason 0) unconditionally, then the
    //  write-bucket (reason 1) when `this+0x70` is positive - a pending
    //  send-backlog count position matching TCPLISTEN's and UDPCONN's own
    //  slot 2 overrides at the same two addresses (sub_6F6E6BC0/
    //  sub_6F6D9B60), both out of this session's scope and thunked.
    virtual void RegisterForSelect(void* mgr);
    //  Slot 4 (+0x10) - DeleteSelf (netconn.h's slot 4 comment).  0x6F6E2620,
    //  retn 4h.  Calls `this->TCPCONN::~TCPCONN()` as a real, out-of-line
    //  call (0x6F6E0520 - the same destructor IOTCPCONN::DeleteSelf calls),
    //  so this override carries the `this != 0` test the same way
    //  IOTCPCONN's does.
    virtual SLTCPCONN* DeleteSelf(int flags);
    //  Slot 15 (+0x3C) - TCPCONN::OnQueued's override (`_purecall` in
    //  TCPCONN).  0x6F6DAA80, retn 4h.  Three instructions: release the
    //  connection lock and ignore `buf` entirely - SLTCPCONN has no
    //  overlapped send path to arm, so all this override has to do is give
    //  back the lock Send()/OnSendComplete() took before reaching here.
    virtual void OnQueued(OUTPUT* buf);
};

//----------------------------------------------------------------------------
//  0x6F6E24F0 - the completion-port connection.
//
//  m_pending starts at 1 and is what keeps the object alive while the port
//  owns it: the constructor's own reference is dropped by ReleasePending() on
//  the way out, so a connection whose registration failed tears itself down
//  before the constructor returns.
//----------------------------------------------------------------------------
class IOTCPCONN : public TCPCONN
{
public:
    IOTCPCONN(TCPMGR* owner, HANDLE iocp, SOCKET s, CONNNOTIFYPROC proc,
              void* ctx, const void* addrs, const void* buf, int len);

    //  0x6F6DA990 - slot 16.  Arm the next receive into m_recvBuf +
    //  m_recvUsed.
    virtual void PostRead();

    //  0x6F6E0670 - slot 6.  Drop one outstanding-I/O reference.
    virtual void ReleasePending();

    //  0x6F6DA8C0 - slot 5.  `InterlockedIncrement(&m_pending)`, the exact
    //  mirror of ReleasePending's decrement.  PostRead calls it before
    //  arming the receive (see netconn.h's slot 5 comment).
    virtual void AddPending();

    //  0x6F6E25F0 - slot 4, the scalar deleting destructor (see netconn.h's
    //  `DeleteSelf` and TCPCONN's own override just above).  IOTCPCONN
    //  declares no destructor of its own - m_pending and m_readOp are plain
    //  data, nothing to tear down - so its implicit `~IOTCPCONN()` is a
    //  bare call into `TCPCONN::~TCPCONN()`, and that call has to stay real
    //  rather than inline the same way TCPCONN's own override does:
    //  defined in its own translation unit (iotcpconn_deleteself.cpp),
    //  which only declares `~TCPCONN()` (ostcp.h) rather than defining it.
    virtual IOTCPCONN* DeleteSelf(int flags);

    //  0x6F6DA8D0 - slot 15, TCPCONN::OnQueued's override.  Marks the
    //  buffer in-flight (`m_sendState = 1`), bumps the pending-I/O count
    //  (AddPending), releases the connection lock Send()/OnSendComplete()
    //  are holding on the way in, then tries an overlapped send: WSASend
    //  when the extension pointer resolved (g_pfnWSASend, ostcp.h's
    //  globals - the send-path twin of PostRead's WSARecv fallback), or a
    //  plain overlapped WriteFile on the socket handle otherwise.  Either
    //  way, `OUTPUT`'s own five "never read" reserved dwords (+0x08..+0x1C)
    //  are what gets passed as the OVERLAPPED - they are read here, just
    //  not by either call tree that named them that.  A synchronous failure
    //  that isn't "I/O pending" closes the connection and drops the pending
    //  reference this function just took.
    virtual void OnQueued(OUTPUT* buf);

    long    m_pending;              // +0x630
    IOOP    m_readOp;               // +0x634 .. +0x64B
    char    m_reserved64C[4];       // +0x64C
};

//----------------------------------------------------------------------------
//  The listening side's configuration, reached only through TCPACCEPT.
//
//  m_notifyProc/m_notifyCtx are the pair every connection accepted through
//  this listener is born with: TCPMGR::OnAcceptComplete reads them straight
//  out of here and hands them to CreateConnection.
//----------------------------------------------------------------------------
class TCPLISTENER
{
public:
    int             m_reserved0;        // +0x00
    SOCKET          m_listenSocket;     // +0x04
    char            m_reserved8[0x0C];  // +0x08 .. +0x13
    CONNNOTIFYPROC  m_notifyProc;       // +0x14
    void*           m_notifyCtx;        // +0x18
    //  Cleared when the listener is being shut down.  Both methods below test
    //  it first and, when it is clear, close the socket instead of re-arming.
    int             m_running;          // +0x1C
};

//----------------------------------------------------------------------------
//  One in-flight AcceptEx.
//
//  m_op is at +0x08 rather than +0x00, and that offset is visible from the
//  outside: the pump only ever gets the OVERLAPPED back from
//  GetQueuedCompletionStatus, and recovers the slot by subtracting 8 from it.
//  m_addrBuf is AcceptEx's output buffer, two sockaddr-plus-16 slots of 0x20
//  bytes each, which is why the pair of 0x20s appears in both calls below.
//----------------------------------------------------------------------------
class TCPACCEPT
{
public:
    //  0x6F6DBD20 - create a fresh socket and post an AcceptEx on it.
    void   Rearm();

    //  0x6F6DBDB0 - retn 8.  Finish (or abandon) the accept that just
    //  completed, fill `outAddrs` with its {remote, local} sockaddr pair, and
    //  re-arm.  Returns the accepted socket, or INVALID_SOCKET.
    SOCKET FinishAccept(void* outAddrs, int accepted);

    char             m_reserved0[8];    // +0x00
    IOOP             m_op;              // +0x08 .. +0x1F
    CRITICAL_SECTION m_critSec;         // +0x20 .. +0x37
    char             m_addrBuf[0x40];   // +0x38 .. +0x77
    TCPLISTENER*     m_listener;        // +0x78
    SOCKET           m_socket;          // +0x7C
};

//----------------------------------------------------------------------------
//  One of TCPMGR's four connection states, sharded eight ways.
//
//  Every insertion bumps m_cursor and takes the low three bits of it as the
//  shard, so connections spread round-robin across eight lists and eight
//  locks instead of contending on one.  The shard a connection landed in is
//  remembered in NETCONN::m_stateBucket, because removal has no other way to
//  find which lock to take.
//
//  The lists are TSExplicitList, not TSList: Insert()/Remove() load the link
//  offset out of the list object before every Link(), which is what a
//  connection whose TSLink sits at +0x08 rather than +0x00 needs.
//----------------------------------------------------------------------------
class CONNSTATE
{
public:
    //  0x6F6DF540 - retn 4.  Returns the shard it went into.
    unsigned char Insert(NETCONN* conn);

    //  0x6F6DF5F0 - retn 8.
    void          Remove(NETCONN* conn, unsigned char bucket);

    //  0x6F6DF4D0 - retn 0.  Under each shard's own lock in turn, destroy
    //  and free every connection still on that shard's list.
    void          FreeAll();

    //  A vftable pointer, confirmed live: the debugger found 0x6F975940 in
    //  this slot, which sits in the vtable block exactly five slots below
    //  IOTCPCONN's own table at 0x6F975954.  So CONNSTATE is polymorphic with
    //  a small (~5-slot) vtable - this was an `int m_reserved0` until that
    //  read.  Nothing in these call trees dispatches through it, so the
    //  virtual below exists only to put the pointer here.
    virtual void m_reservedSlot0() {}
    TSExplicitList<NETCONN> m_lists[8];     // +0x04 .. +0x63
    CRITICAL_SECTION        m_locks[8];     // +0x64 .. +0x123
    long                    m_cursor;       // +0x124
    long                    m_count;        // +0x128
};

//----------------------------------------------------------------------------
//  OsNet::LOOPCONN::INPUT - one queued loopback datagram.  The name is the
//  RTTI descriptor the free hands Storm, `.?AUINPUT@LOOPCONN@OsNet@@`.  Two
//  links, because the record is on the manager's list *and* on its own
//  connection's.
//----------------------------------------------------------------------------
struct LOOPINPUT
{
    TSLink<LOOPINPUT> m_link;    // +0x00 - TCPMGR::m_inputs
    TSLink<LOOPINPUT> m_link2;   // +0x08 - the owning LOOPCONN's
};

//----------------------------------------------------------------------------
//  OsNet::TCPLISTEN - what a listening socket is here.  The name is stamped
//  twice: the slotted list that holds them carries
//  `??_7?$TSSlottedListEx@UTCPLISTEN@OsNet@@$07$00@OsNet@@6B@` as its
//  vftable, and the free that releases one hands Storm the element's RTTI
//  descriptor `.?AUTCPLISTEN@OsNet@@` instead of a filename.
//----------------------------------------------------------------------------
struct TCPLISTEN
{
    //  0x6F6E1370 - retn 0.  Close the socket and unlink.
    void Close();

    //  0x6F6E14A0 - retn 0.  closesocket() the one at +0x04 and mark it
    //  INVALID_SOCKET again, having first told the select set to forget it.
    void CloseSocket();

    //  0x6F6E1420 - retn 4.  Toggle listen() on the socket at +0x04 on or
    //  off when `wantListen` differs from the stored +0x1C flag; closesocket
    //  and mark the slot dead if listen() itself fails.  On the transition
    //  to listening, drains m_pendingRearm through TCPACCEPT::Rearm()
    //  (0x6F6DBD20, already reconstructed in ostcp_rearm.cpp) - `this` in
    //  ecx, no edx use (a plain thiscall, not the `__fastcall` a prior
    //  session's caller-side guess at ostcp_listenclosesocket.cpp assumed;
    //  see that file's own note).
    void SelectSetUpdate(int wantListen);

    char   m_reserved0[0x04];   // +0x00
    SOCKET m_socket;            // +0x04
    char   m_reserved8[0x14];   // +0x08 .. +0x1B
    int    m_wantListen;        // +0x1C - SelectSetUpdate's own stored flag
    char   m_reserved20[0x04];  // +0x20
    //  A list of accept slots SelectSetUpdate walks through Rearm() on the
    //  0->1 (start listening) edge.  Link offset 0 (matching TCPACCEPT's own
    //  leading m_reserved0[8]) is folded at compile time here - the real
    //  disassembly reads the "next" field straight off the node with no
    //  `m_linkoffset` load first, which is TSList<T,LINKOFFSET>, not
    //  TSExplicitList<T> (see storm.h's own note on the pair).
    TSList<TCPACCEPT, 0> m_pendingRearm;   // +0x24 .. +0x2F
};

//----------------------------------------------------------------------------
//  One entry of TCPMGR::m_threads: a thread handle at +0x14 threaded on
//  a list whose link offset is 0.
//----------------------------------------------------------------------------
struct TCPTHREADREC
{
    //  0x6F6DEA80 - retn 0.  Release the two OsTcp.cpp buffers, the address
    //  and key blocks, close the thread handle and unlink.
    void Destruct();

    TSLink<TCPTHREADREC> m_link;       // +0x00
    void*   m_buffer;                   // +0x08 (freed with OsTcp.cpp:0x3E7)
    char    m_reservedC[0x08];          // +0x0C
    HANDLE  m_thread;                   // +0x14
    char    m_reserved18[0x0C];         // +0x18
    void*   m_keyBlock;                 // +0x24 (freed with OsTcp.cpp:0x3E8)
    char    m_reserved28[0x08];         // +0x28
    void*   m_key;                      // +0x30 (freed with tag "K")
    char    m_reserved34[0x0C];         // +0x34
    void*   m_addr;                     // +0x40 (freed with ".?ATNETADDR@@")
};

//----------------------------------------------------------------------------
//  OsNet::TSSlottedListEx<T,7,0> - CONNSTATE's one-shard sibling, and the
//  same layout with the arrays one element long: a vftable, one list, one
//  lock, a round-robin cursor and a count.  Two instantiations are reached
//  here, `TSSlottedListEx<TCPLISTEN,7,0>` (TCPMGR::m_listens) and
//  `TSSlottedListEx<NETCONNECT,7,0>` (the four TCPMGR::m_connects); their
//  bodies are identical because nothing in either touches T beyond the list.
//----------------------------------------------------------------------------
class LISTENSLOTS
{
public:
    //  0x6F6E2C60 - retn 0.
    LISTENSLOTS* Construct();

    //  0x6F6E3A00 - retn 0.  Close and free every entry still on the list.
    void ReleaseAll();

    virtual void m_reservedSlot0() {}
    TSExplicitList<TCPLISTEN> m_list;   // +0x04 .. +0x0F
    CRITICAL_SECTION          m_lock;   // +0x10 .. +0x27
    long                      m_cursor; // +0x28
    long                      m_count;  // +0x2C
};

//----------------------------------------------------------------------------
//  A walk over one of those, as the callers build it on the stack: the
//  container, the node the walk is on, the node it will step to next, the
//  node it started from, and the shard index.
//----------------------------------------------------------------------------
struct SLOTITER
{
    //  0x6F6DE4E0 - retn 0.  Step to the next node, moving the lock from one
    //  shard to the next when the current shard runs out.
    void Next();

    LISTENSLOTS* m_owner;      // +0x00
    int          m_current;    // +0x04
    int          m_next;       // +0x08
    int          m_start;      // +0x0C
    int          m_shard;      // +0x10
};

//----------------------------------------------------------------------------
//  The module's per-provider manager.
//
//  Only three of its fields are reachable from this call tree, but the two
//  offsets between them are exact: m_states is four CONNSTATEs of 0x12C each
//  starting at +0x64 (0x64 + 4 * 0x12C = 0x514), and the completion port is at
//  +0x618.
//----------------------------------------------------------------------------
class TCPMGR
{
public:
    //  0x6F6E4070 - retn 4.  Pump the port for up to `ms`, or just sleep.
    void Pump(DWORD ms);

    //  0x6F6E3EE0 - retn 4.  The completion-port loop itself.
    void PumpIocp(DWORD ms);

    //  0x6F6E37D0 - retn 8.  An accept completed; turn it into a connection.
    void OnAcceptComplete(TCPACCEPT* slot, int accepted);

    //  0x6F6E34C0 - retn 18h.  Build the connection kind this manager wants.
    TCPCONN* CreateConnection(SOCKET s, CONNNOTIFYPROC proc, void* ctx,
                              const void* addrs, const void* buf, int len);

    //  0x6F6DFFB0 - retn 8.  Move `conn` from the state list it is on to
    //  `newState`, under the connection's own lock.
    void MoveConn(NETCONN* conn, int newState);

    //  0x6F6E30D0 - retn 0.  Zero everything, initialise the lock, the three
    //  lists, the four CONNSTATE shards, the listen slots and the four
    //  connect slots, and create the "idle" event.
    TCPMGR* Construct();

    //  0x6F6E3220 - retn 0.  The mirror image, plus draining every list.
    void    Destruct();

    //  0x6F6E4090 - retn 0.  InterlockedDecrement(&m_refcount); at zero,
    //  destruct and hand the block back to Storm under the tag "delete".
    void    Release();

    //  0x6F6E6CC0 / 0x6F6E6D30 / 0x6F6E6C40 / 0x6F6E4F60 - retn 4 each.
    //  The four subsystem starts OsNetInitialize drives.
    int     StartWinsock(int flags);
    int     StartIocp(int flags);
    int     StartSelectThreads(int flags);
    int     StartListenThread(int flags);
    int     StartTimerThread(int flags);

    //  0x6F6E5ED0 - retn 0.  One pump thread per configured worker.
    int     StartPumpThreads();

    //  0x6F6E1500 / 0x6F6E1570 / 0x6F6DA090 / 0x6F6E4190 - retn 0 each.
    //  The four stops OsNetShutdown drives.
    void    StopTimerThread();
    void    StopSelectThreads();
    void    StopListenThread();
    void    StopIocp();

    //  0x6F6E0B60 - retn 0.  Drain and free the LOOPCONN::INPUT list.
    void    FreeInputList();

    //  0x6F6DECF0 - retn 0.  Wait for every pump thread, then close its
    //  handle.
    void    JoinPumpThreads();

    //  0x6F6DBF50 - retn 0.  Wake the completion port with a null packet so
    //  a blocked pump notices the shutdown flag.
    void    WakeIocp();

    long        m_refcount;             // +0x00
    int         m_workerCount;          // +0x04 - how many pump threads to run
    //  +0x08 .. +0x17.  The pump threads' handles.  This is what TCPMGR::Pump
    //  (0x6F6E4070) tests at +0x0C before deciding to sleep instead of
    //  pumping: a non-zero *count* means somebody else is already draining
    //  the port.  It was `int m_pumpDisabled` until StartPumpThreads
    //  (0x6F6E5ED0) and JoinPumpThreads (0x6F6DECF0) showed the four
    //  consecutive words being driven as one TSGrowableArray - alloc, count,
    //  data, chunk - through the same ComputeChunk/SetAlloc pair every other
    //  one in this repo uses.  The generated code for Pump is unchanged.
    TSGrowableArray<HANDLE> m_pumpThreads;   // +0x08 .. +0x17
    HANDLE      m_pollThread;           // +0x18
    HANDLE      m_pollEvent;            // +0x1C
    HANDLE      m_timerThread;          // +0x20
    HANDLE      m_timerEvent;           // +0x24
    CRITICAL_SECTION m_lock;            // +0x28 .. +0x3F
    TSExplicitList<LOOPINPUT> m_inputs; // +0x40 .. +0x4B (link offset 8)
    TSExplicitList<NETCONN>  m_listB;   // +0x4C .. +0x57 (link offset 8)
    TSExplicitList<NETCONN>  m_listC;   // +0x58 .. +0x63 (link offset 0x6C)
    CONNSTATE   m_states[4];            // +0x64 .. +0x513
    HANDLE      m_selectThread;         // +0x514
    LISTENSLOTS m_listens;              // +0x518 .. +0x547
    HANDLE      m_listenThread;         // +0x548
    HANDLE      m_listenWakeEvent;      // +0x54C
    int         m_listenStopping;       // +0x550
    HANDLE      m_listenDoneEvent;      // +0x554
    LISTENSLOTS m_connects[4];          // +0x558 .. +0x617
    HANDLE      m_iocp;                 // +0x618
    char        m_reserved61C[0x04];    // +0x61C
    HANDLE      m_idleEvent;            // +0x620
    char        m_reserved624[0x04];    // +0x624
    TSExplicitList<TCPTHREADREC> m_threads;   // +0x628 .. +0x633 (link offset 0)
};


//----------------------------------------------------------------------------
//  The module's own entry points.  Both take a subsystem mask; the `arg` of
//  OsNetInitialize is passed through to every start unchanged and is a bag of
//  option bits the individual starts read (StartIocp reads three of them).
//----------------------------------------------------------------------------
int  __fastcall OsNetInitialize(int arg, unsigned int mask);   // 0x6F6E6E30
void __fastcall OsNetShutdown(unsigned int mask);              // 0x6F6E4E30
void __fastcall OsNetWinsockShutdown();                        // 0x6F6E40C0
HANDLE __fastcall OsNetCreateIocp(unsigned int* workerCount);  // 0x6F6D9ED0
void __fastcall OsNetExpandMask(unsigned int* mask);           // 0x6F6D9F40

}  // namespace OsNet

//----------------------------------------------------------------------------
//  0x6F6E49A0 - `__fastcall`, so the millisecond budget arrives in ecx and
//  nothing is cleaned off the stack.  The one entry point the rest of the game
//  drives this module through: EvtWorkerThreadMain calls it and nothing else
//  in either call tree does.
//----------------------------------------------------------------------------
void __fastcall OsTcpPump(DWORD ms);

//----------------------------------------------------------------------------
//  The module's globals.  Addresses are in tools/funcmap.py's DATA table.
//----------------------------------------------------------------------------

//: 0x6FADA61C-ish - IDA calls it `lpAddend` because other call trees pass it
//: to the Interlocked* pair; here it is dereferenced as the manager itself.
//: Null before the module is started, which is why OsTcpPump has a fallback.
extern OsNet::TCPMGR* g_pTcpManager;

//: Set once the module is shutting down.  PumpIocp tests it after every
//: completion and abandons the loop rather than dispatching.  Raised and
//: lowered again by TCPMGR::StopIocp.
extern int g_osTcpShutdown;

//: The other three "a teardown is in progress" flags, one per subsystem, set
//: and cleared around the corresponding stop.
extern int g_osTcpSelectStopping;   // 0x6FADA7D0 - StopSelectThread
extern int g_osTcpTimerStopping;    // 0x6FADA7D8 - StopTimerThread
extern int g_osTcpListenStopping;   // 0x6FADA7DC - StopListenThread

//: OsNetInitialize/OsNetShutdown's own reference counts, one per subsystem
//: bit: each start runs only on the 0 -> 1 edge and each stop only on the
//: 1 -> 0 edge, which is what makes the two idempotent.
extern int g_osNetRefCore;          // 0x6FADA7E4 - bit 8 (the manager itself)
extern int g_osNetRefTimer;         // 0x6FADA7E8 - bit 1
extern int g_osNetRefSelect;        // 0x6FADA7EC - bit 2
extern int g_osNetRefWinsock;       // 0x6FADA7F0 - bit 0x10
extern int g_osNetRefIocp;          // 0x6FADA7F4 - bit 0x20
extern int g_osNetRefMisc;          // 0x6FADA7F8 - bit 4

//: The module's own lock and the "has it been initialised" target the lock
//: helper at 0x6F6D8650 takes.
extern int  g_osNetLock;            // 0x6FADA7FC - the spin flag
extern CRITICAL_SECTION* g_pOsNetLock;   // 0x6FADA800 - and the section itself

//: Non-zero once GetVersionEx reported a major version below 5, i.e. this is
//: a Windows 9x kernel and the completion-port paths must not be used.
extern int  g_osTcpLegacyOs;        // 0x6FADA63C

//: Storm ordinal 541's handle - released once, on the last shutdown.
extern void* g_osNetStormHandle;    // 0x6FADA61C

//: The two libraries the Winsock extension entry points are looked up in.
//: IDA names the storage `hModule` / `hLibModule` with no numeric address, so
//: like g_pTcpManager these have nothing for the injection binder to resolve
//: and are defined in ostcp_netglobals.cpp - see the note there.
extern HMODULE g_hMswsock;
extern HMODULE g_hWs2_32;
extern WSADATA g_wsaData;

//: 0x6F6D8650 - take the module lock, initialising it on first use.  Shared
//: with src/Sync/debuglock.cpp, which owns the body; the "record" is a bare
//: pair of globals here rather than an object.
void __fastcall LazyInitAndEnterDebugLock(void*);

//: Storm ordinal 541 - release the handle OsNetInitialize took.
void __stdcall Storm_541(void* handle);

//: Non-zero once QueryPerformanceCounter has been found usable; when it is
//: clear the pump falls back to GetTickCount.  g_qpcMsPerCount is the `float`
//: scale from counter ticks to milliseconds.
extern int   g_osTcpUseQpc;
extern float g_qpcMsPerCount;

//: The Winsock extension entry points this module looks up at run time
//: rather than importing.  A null g_pfnWSARecv is what makes PostRead fall
//: back to ReadFile on the same handle, and a null g_pfnWSASend is the same
//: fallback for IOTCPCONN::OnQueued, which falls back to an overlapped
//: WriteFile.  0x6FADA628, immediately after g_pfnWSARecv (0x6FADA624) and
//: immediately before g_pfnGetAcceptExSockaddrs (0x6FADA62C) - the four
//: extension pointers sit as one contiguous run of dwords.
extern int (__stdcall* g_pfnWSARecv)(SOCKET s, LPWSABUF buffers,
                                     DWORD bufferCount,
                                     LPDWORD bytesReceived, LPDWORD flags,
                                     LPWSAOVERLAPPED overlapped,
                                     void* completionRoutine);
extern int (__stdcall* g_pfnWSASend)(SOCKET s, LPWSABUF buffers,
                                     DWORD bufferCount,
                                     LPDWORD bytesSent, DWORD flags,
                                     LPWSAOVERLAPPED overlapped,
                                     void* completionRoutine);
extern void (__stdcall* g_pfnGetAcceptExSockaddrs)(
        void* outputBuffer, DWORD receiveDataLength,
        DWORD localAddressLength, DWORD remoteAddressLength,
        struct sockaddr** localSockaddr, int* localSockaddrLength,
        struct sockaddr** remoteSockaddr, int* remoteSockaddrLength);
//: The ws2_32 entry point nothing in either call tree calls - looked up
//: alongside WSARecv and stored, so it is untyped here.  g_pfnWSASend used
//: to be declared here as a second, `void*` spelling of the typed pointer
//: above; two declarations of one global under two types is two symbols, and
//: the merge that brought both halves together would not compile.
extern void* g_pfnWSAIoctl;     // 0x6FADA620

extern BOOL (__stdcall* g_pfnAcceptEx)(SOCKET listenSocket, SOCKET acceptSocket,
                                       void* outputBuffer,
                                       DWORD receiveDataLength,
                                       DWORD localAddressLength,
                                       DWORD remoteAddressLength,
                                       LPDWORD bytesReceived,
                                       LPOVERLAPPED overlapped);

#endif
