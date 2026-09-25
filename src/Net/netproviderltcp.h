//============================================================================
//  See netproviderltcp.cpp for the class-level notes.
//============================================================================
#ifndef NETPROVIDERLTCP_H
#define NETPROVIDERLTCP_H

#include <windows.h>
#include "event.h"
#include "waithandlesignal.h"
#include "storm.h"

namespace Net
{

//----------------------------------------------------------------------------
//  Net::NetProviderLTCP::LocalGameAd - the registered-notify-target list
//  element this class's Storm allocation tag names outright:
//  `".?AULocalGameAd@NetProviderLTCP@@"` (docs/msvc-vc8-idioms.md's "The
//  Storm allocation tag names a template argument you already have a class
//  for" - here it names a plain nested struct, not a template).  0xD8 bytes;
//  only the fields RegisterNotifyTarget/UpdateNotifyTarget/
//  UnregisterNotifyTarget actually touch are named, the rest stays an
//  `m_reservedNN` gap.  Its own `TSLink` lives at offset 0 (the same
//  DirectLink shape this repo's other intrusive lists use).
//----------------------------------------------------------------------------
struct LocalGameAd
{
    TSLink<LocalGameAd> m_link;   // +0x00 (8 bytes: m_next@0, m_prevlink@4)
    void*        m_target;        // +0x08 - the lookup key RegisterNotifyTarget/UpdateNotifyTarget/UnregisterNotifyTarget all compare against
    char         m_argCField[4];  // +0x0C - RegisterNotifyTarget's own `argC` stack argument, stored verbatim
    char         m_reserved010[0xB8]; // +0x10 .. +0xC8 - bulk-copied 0xB8-byte descriptor
    int          m_registered;    // +0xC8 - written 1 by RegisterNotifyTarget, overwritten by UpdateNotifyTarget's own two stack args
    int          m_reservedCC;    // +0xCC
    void*        m_handle;        // +0xD0 - RegisterNotifyTarget's own sub_6F6C5550() return
    unsigned short m_reservedD4;  // +0xD4
    char         m_reservedD6[2];
};

//----------------------------------------------------------------------------
//  Net::NetProviderLTCP - the LAN TCP net provider (user_knowledge.json's
//  vtable dump: `Net::NetProviderLTCP`, vtable 0x6F9713BC). Only the
//  listener-thread bootstrap/teardown fields this call tree touches are
//  named; everything else is an `m_reservedNN` gap.
//----------------------------------------------------------------------------
class NetProviderLTCP
{
public:
    //  0x6F663680 - vtable slot 0, the classic MSVC "scalar deleting
    //  destructor". See netproviderltcp_deleteself.cpp.
    void* DeleteSelf(unsigned int flags);

    //  0x6F6630A0 - vtable slot 1. See netproviderltcp_initialize.cpp.
    unsigned int Initialize(void* configStruct, int* outStatus);

    //  0x6F661F80 - vtable slot 2. See netproviderltcp_lifecycle.cpp.
    int Shutdown();

    //  vtable+0xC (method_index 3) - `retn 4`, one stack argument, and
    //  nothing in this override's body ever reads it (the base interface's
    //  signature evidently has a parameter this particular override does
    //  not need). See netproviderltcp.cpp.
    void Method_0xC(int unusedArg);

    //  0x6F662E10 - vtable slot 4, `retn 0` (no stack arguments at all -
    //  everything it forwards comes from `this` alone). See
    //  netproviderltcp_send.cpp.
    void FlushPendingSend();

    //  0x6F661A20 - vtable slot 5. See netproviderltcp_send.cpp.
    void CancelSend();

    //  0x6F661CD0 - stop the listener thread if one is running and wait for
    //  it to actually exit. See netproviderltcpstop.cpp.
    void StopListenerThread();

    //  netproviderltcp_addrslots.cpp - 0x6F661A90/0x6F661AA0/0x6F661AC0/
    //  0x6F661AD0, vtable slots 10/11/12/13.  Same shape as
    //  netproviderloop.h's own quartet: each overwrites `ecx` from its own
    //  stack argument before doing anything, so `this` is provably unread.
    int            GetFixedCode();                                            // slot 10
    int            AddressesEqual(const void* a, const void* b, int* outFlag); // slot 11
    unsigned int   GetAddressValue(const void* addr);                         // slot 12
    void           BuildAddress(void* out, const void* srcAddr, unsigned short port); // slot 13

    //  0x6F661AF0 - vtable slot 14. See netproviderltcp_route.cpp.
    void RouteToPort(const void* addr, int arg4, int arg8, const void* data, unsigned int size);

    //  0x6F661B20 - vtable slot 15. See netproviderltcp_bindport.cpp.
    unsigned short BindPort(unsigned short port, unsigned short limit, int target);

    //  0x6F661B70 - vtable slot 16. See netproviderltcp_freeport.cpp.
    void FreePort(unsigned short port);

    //  0x6F663A40 - vtable slot 17, `retn 1Ch` (seven stack dwords; the
    //  first is never read anywhere in the body - the same "declared but
    //  unused" leading-argument shape Method_0xC's own header note
    //  documents). See netproviderltcp_notify.cpp.
    void RegisterNotifyTarget(void* unusedArg0, void* target, void* arg8, void* descriptor,
                               void* arg10, unsigned int arg14, unsigned short arg18);

    //  0x6F6633F0 - vtable slot 18, `retn 10h` (four stack dwords; the
    //  first is never read). See netproviderltcp_notify.cpp.
    void UpdateNotifyTarget(void* unusedArg0, void* target, int arg8, int argC);

    //  0x6F663480 - vtable slot 19, `retn 0Ch` (three stack dwords; the
    //  first and third are never read). See netproviderltcp_notify.cpp.
    void UnregisterNotifyTarget(void* unusedArg0, void* target, void* unusedArg8);

    char             m_reserved000[0x24];
    //  +0x24/+0x28 - read (never written) by FlushPendingSend/slot 4 alone
    //  and forwarded verbatim into the deep send pipeline (SendViaListener) -
    //  a data pointer and a paired int, inferred from that call site's own
    //  argument shape rather than confirmed by any store this call tree
    //  contains.
    void*            m_pendingSendData;      // +0x24
    int              m_pendingSendAux;        // +0x28
    char             m_reserved02C[0x650];   // +0x2C .. +0x67C
    int              m_enabled;             // +0x67C
    CRITICAL_SECTION m_lock;                 // +0x680 (24 bytes, ends at +0x698)
    int              m_threadRunning;        // +0x698
    //  0x69C - stored from NETCONN::AddRef()'s return (ContainsW3GSBytePacketCheck's
    //  case 0) and read back through IWaitHandleSignal's one known slot
    //  (StopListenerThread) - untyped here rather than picked as one or the
    //  other, since nothing in either call tree proves it is not both (a
    //  UDPCONN-derived object implementing more virtual slots than NETCONN
    //  itself declares).
    void*            m_listenHandle;         // +0x69C
    Event            m_threadDoneEvent;      // +0x6A0
    //  0x6A4 - during StopListenerThread it holds `&`-of a local Event;
    //  during ContainsW3GSBytePacketCheck's case 2 it holds some other
    //  handle later passed to Event::Set() through a `void*` reinterpreted
    //  as the same one-HANDLE-field shape - the same physical slot serving
    //  two different pointer meanings depending on which path is live.
    void*            m_pendingSignalTarget;  // +0x6A4
    int              m_recycleFlag;          // +0x6A8 - ContainsW3GSBytePacketCheck's case 2 only
    //  +0x6AC/+0x6B0/+0x6B4 - a `TSList<LocalGameAd,0>` (storm.h's own
    //  fixed-offset list template - the dump never loads a linkoffset in
    //  front of a walk, consistent with this LINKOFFSET-0 shape rather than
    //  the runtime-offset `TSExplicitList<T>`).  RegisterNotifyTarget/
    //  UpdateNotifyTarget/UnregisterNotifyTarget address `m_terminator`'s
    //  two fields directly (never `m_linkoffset`, which the template itself
    //  never reads for a LINKOFFSET<0 template argument either).
    TSList<LocalGameAd, 0> m_notifyList;     // +0x6AC (12 bytes: m_linkoffset@0x6AC, m_terminator@0x6B0)
    //  +0x6B8 - non-zero once RegisterNotifyTarget has registered this
    //  object's periodic re-advertise callback with the timer subsystem
    //  (sub_6F62ABF0); guards that one-shot registration.
    void*            m_notifyTimerHandle;    // +0x6B8
};

}  // namespace Net

#endif
