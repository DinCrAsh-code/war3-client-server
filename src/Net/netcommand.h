//============================================================================
//  CNetCommandBase - the wire-command objects CNetData's action-byte
//  dispatcher (sub_6F550730, netcommand_dispatch.cpp) builds on its own
//  stack for every one of the ~133 turn-stream action bytes, then hands to
//  CNetData::FireCommand (0x6F53B790) to broadcast to a per-player pair of
//  embedded CObserver lists (0x6F5378E0) to any UI-side listener registered
//  against the wire type id (observer_slots.cpp's ObserverEventReg map,
//  keyed by m_id).
//
//  Field layout is exact, taken from CNetCommandPause's own stack-object
//  writes (0x6F5402B0) and confirmed identical across every sibling builder
//  in the same module.  It overlays observer.h's SAgentMessage in its first
//  0x0C bytes (m_id at +0x08 is the same field SAgentMessage::m_id names) -
//  that is why CNetData::FireToObserverLists and CObserver::Method_0x10/
//  0x14 can treat any CNetCommandXxx generically as a SAgentMessage*
//  without knowing which concrete command type it is.
//
//  Every concrete CNetCommandXxx is constructed as a local, its whole
//  constructor body inlined into the builder that stamps it - the vftable
//  store, both zero fields and the wire id all appear as plain stores at
//  the builder's own address, with no visible `call` to a constructor.
//  What *is* a real out-of-line call per builder is a trivial per-class
//  "attach" hook (0x6F5546E0 for Pause, a different address per sibling)
//  that takes the reader and the freshly-built command and does nothing
//  observable - see netcommand_pause.cpp's own comment on it.  The base
//  class needs a virtual (even if empty) destructor for the vftable store
//  to be real rather than invented, which is also why every builder in
//  this module carries the unreproducible `__except_handler4`-shaped SEH
//  frame docs/msvc-vc8-idioms.md already establishes this toolchain cannot
//  emit - a local object with a non-trivial destructor next to a call that
//  might throw needs one, and it produces no instructions of its own on
//  the normal-return path (nothing to clean up), so its absence here costs
//  only the frame furniture, not the body.
//============================================================================
#ifndef NETCOMMAND_H
#define NETCOMMAND_H

#include "netdata.h"

class CNetCommandBase
{
public:
    virtual ~CNetCommandBase() {}

    //  Sets every field but m_ctx/m_sender, which the two builder-supplied
    //  values only known to the derived class fill in afterward - matching
    //  the observed instruction order, where the always-constant fields
    //  (including the derived class's own vftable, stamped last as the
    //  final surviving store once the base subobject's own redundant
    //  vftable write is dead-store-eliminated) are grouped together ahead
    //  of the two runtime-value stores.
    CNetCommandBase(unsigned int id, unsigned char subIndex)
        : m_field4(0), m_id(id), m_field0C(0), m_ctx(0), m_subIndex(subIndex), m_sender(0)
    {
    }

    int           m_field4;     // +0x04 (always 0 in every builder seen)
    unsigned int  m_id;         // +0x08 (wire net-command type, e.g. 0xA0001 - SAgentMessage::m_id)
    int           m_field0C;    // +0x0C (always 0 in every builder seen)
    int           m_ctx;        // +0x10 (dispatcher-supplied edx - turn/ctx value)
    unsigned char m_subIndex;   // +0x14 (per-class literal; CNetCommandPause=1, CNetCommandUnpause=2)
    unsigned char m_sender;     // +0x15 (sending player id, or 0xFF for system)
};

//----------------------------------------------------------------------------
//  0x6F53B790 - CNetData::FireCommand.  Logs "War3Net: (INFO): turn %u: %s
//  from %u\n" through nullsub_1 when `verbose` is set, then - unless the
//  command's sender byte is 0xFF (system, never broadcast) - forwards to
//  CNetData::FireToObserverLists.
//----------------------------------------------------------------------------
void __fastcall CNetData_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                      const char* name, int verbose, unsigned int turn);

//----------------------------------------------------------------------------
//  0x6F5378A0 - CNetData::ParseError: logs "CNetObserver::ParseError (%s)\n"
//  with the given tag and returns.  Reached when a builder's overflow guard
//  (reader position past its declared length) trips.
//----------------------------------------------------------------------------
void CNetData_ParseError(const char* tag);

#endif
