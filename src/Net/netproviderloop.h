//============================================================================
//  Net::NetProviderLOOP - the loopback/single-player net provider (vtable
//  0x6F9712CC, 42 slots).  Flat, non-virtual, manual-vtable-slot-dispatch
//  class in the same style netproviderltcp.h uses for its sibling - see
//  that header's own note.  Only the fields this session's twelve slots
//  actually touch are named; everything else is a `m_reservedNN` gap.
//
//  One header shared by every .cpp this session added (netproviderltcp.h's
//  own arrangement), each providing the out-of-line body for a disjoint
//  subset of the methods below - grouped one file per address neighbourhood
//  the way CLAUDE.md's "one translation unit per original module" asks.
//============================================================================
#ifndef NETPROVIDERLOOP_H
#define NETPROVIDERLOOP_H

#include <windows.h>
#include "storm.h"

namespace Net
{

//----------------------------------------------------------------------------
//  One registered loopback port, keyed by port number - TSList<T,0> puts
//  the TSLink first (8 bytes: m_next@0, m_prevlink@4), which is why the
//  slot-14/15/16 walks below read `node+8`/`node+0xC` with no link-offset
//  `add` (storm.h's own TSList comment).
//----------------------------------------------------------------------------
struct LoopbackPortEntry
{
    TSLink<LoopbackPortEntry> m_link;        // +0x00
    unsigned short            m_port;        // +0x08 (host order)
    unsigned short            m_reservedA;   // +0x0A
    int                       m_target;      // +0x0C - opaque payload the sender looks up by port
};

//----------------------------------------------------------------------------
//  0x6F6617D0 - netproviderloop_freeport.cpp's own callee, not itself a
//  vtable slot.  A list-specific extension the same way ostcp.h's own
//  OUTPUTLIST is (`class OUTPUTLIST : public TSList<OUTPUT, 0>`): no new
//  data members, so the layout is unchanged.
//----------------------------------------------------------------------------
class LoopbackPortList : public TSList<LoopbackPortEntry, 0>
{
public:
    int RemoveAndFree(LoopbackPortEntry* node);
};

class NetProviderLOOP
{
public:
    //  netproviderloop_deleteself.cpp - 0x6F661700, vtable slot 0.
    void* DeleteSelf(unsigned int flags);

    //  netproviderloop_initialize.cpp - 0x6F661730, vtable slot 1.
    unsigned int Initialize(void* configStruct, int* outStatus);

    //  netproviderloop_lifecycle.cpp - 0x6F660EA0, direct callee of slots
    //  2 and 3, not itself a vtable slot.
    void ClearShutdownPending();

    //  netproviderloop_lifecycle.cpp - 0x6F661060, vtable slot 2.
    int Shutdown();

    //  netproviderloop_lifecycle.cpp - 0x6F6610D0, vtable slot 3.  `flags`
    //  is read by neither branch (the same "declared but unused" shape
    //  netproviderltcp.h's own Method_0xC documents).
    void NotifyStateChange(int flags);

    //  netproviderloop_addrslots.cpp - 0x6F660F00/0x6F660F10/0x6F660F30/
    //  0x6F660F40/0x6F660F90, vtable slots 10/11/12/13/41.  All five ignore
    //  `this` except slot 41 - the first four overwrite `ecx` from their own
    //  stack argument before ever touching the shipped `this`, so they are
    //  declared as ordinary members purely to occupy their slot.
    int            GetFixedCode();                                  // slot 10
    int            AddressesEqual(const void* a, const void* b, int* outFlag); // slot 11
    unsigned int   GetAddressValue(const void* addr);                // slot 12
    void           BuildAddress(void* out, const void* srcAddr, unsigned short port); // slot 13
    //  slot 41 - `retn 10h` with none of its four stack dwords ever read;
    //  declared with four unused params purely to match the caller-cleanup
    //  byte count (see thunk_abi_audit.py's note on why this matters even
    //  for a hookable, non-thunk function's own signature).
    int            NotifySubsystem(int, int, int, int);              // slot 41

    //  netproviderloop_route.cpp - 0x6F661840, vtable slot 14.  `arg4`/
    //  `arg8` pass straight through to the thunked sub_6F6E2A60 with no
    //  read here, so their real meaning is opaque from this call tree.
    void RouteToPort(const void* addr, int arg4, int arg8, const void* data, unsigned int size);

    //  netproviderloop_bindport.cpp - 0x6F6618C0, vtable slot 15.
    unsigned short BindPort(unsigned short port, unsigned short limit, int target);

    //  netproviderloop_freeport.cpp - 0x6F6617D0/0x6F6619B0, vtable slot 16
    //  plus its direct callee (not itself a slot).
    void FreePort(unsigned short port);

    char             m_reserved000[0x67C];
    //  +0x67C - non-zero once Initialize (slot 1) has brought the loopback
    //  subsystem up; guards both Initialize and Shutdown (slot 2), both
    //  under the *global* lock below rather than this object's own.
    int              m_started;
    //  +0x680 - Shutdown/slot 3 set and clear this; guarded by the same
    //  global lock as m_started.
    int              m_shutdownPending;
    //  +0x684 - this object's own lock (24 bytes), guarding the port list
    //  below.  Distinct from the module-global stru_6FACFE48 that guards
    //  m_started/m_shutdownPending.
    CRITICAL_SECTION m_portLock;
    //  +0x69C - the registered-port list (TSList<T,0>: m_linkoffset@0x69C,
    //  m_terminator@0x6A0/0x6A4).  Slots 14/15/16 read it as raw offsets off
    //  `this` rather than through the list's own API - matching the shipped
    //  code, which never calls TSList<T,0>::Tail()/Prev() here (those are
    //  __forceinline and would inline identically either way; the shipped
    //  stream is hand-rolled pointer arithmetic instead, reproduced the
    //  same way below).
    LoopbackPortList m_ports;
};

}  // namespace Net

#endif
