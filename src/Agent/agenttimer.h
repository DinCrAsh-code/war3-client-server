//============================================================================
//  CAgentTimer: a small TRefCnt-derived class the CItem__Constructor dump
//  (0x6F2B7520) embeds twice as a plain member (+0xD0 and +0xE4, 0x14 bytes
//  apart - CAgentTimer's own size).
//
//  Layout established by CAgentTimer::CAgentTimer() (0x6F0418E0) and
//  CAgentTimer::Cancel()/~CAgentTimer() (0x6F477D30/0x6F477E20), the only
//  three methods this call tree reaches:
//    +0x00  vtable (TRefCnt's slots, overridden here - see refcnt.h)
//    +0x04  m_refcount (TRefCnt's own field)
//    +0x08  m_target   - a real, typed CAgentPtr member (agent.h), not raw
//                         storage: ~CAgentTimer() (0x6F477E20) reads and
//                         releases +0x08 a *second* time right after
//                         Cancel() already zeroed it - dead in the shipped
//                         binary every time, and the only shape that
//                         explains a "redundant-looking" second release is
//                         ~CAgentPtr()'s own implicit member-destructor
//                         call running automatically after Cancel()'s
//                         explicit one already nulled the pointer.
//    +0x0C  m_queue    - a pointer to some other, unrecovered object;  only
//                         two of its own fields are ever touched, both by
//                         Cancel() (a flag byte at +0x12, a flags dword at
//                         +0x10 that gets one bit OR'd in) - raw offsets,
//                         no formal type, per CLAUDE.md ("do not invent
//                         fields the call tree does not read").
//    +0x10  m_flags    - CAgentTimer's own flags; Cancel() clears bits
//                         0x2/0x4 (mask 0xFFFFFFF9) unconditionally.
//============================================================================
#ifndef AGENTTIMER_H
#define AGENTTIMER_H

#include "refcnt.h"
#include "agent.h"

class CAgentTimer : public TRefCnt
{
public:
    //  0x6F0418E0 - see agenttimer.cpp.
    CAgentTimer();

    //  0x6F477E20 - Cancel(), then destruct m_target a second time (see
    //  +0x08 above; a no-op every time by then) and reset the vtable to
    //  TRefCnt's own on the way out.  Not virtual: nothing in either call
    //  tree reaches it through the vtable.
    ~CAgentTimer();

    //  0x6F477D30 - if m_queue is set and its own "already flagged" byte
    //  (+0x12, bit 0) is clear, OR a "cancelled" bit (0x10000) into its
    //  flags dword (+0x10); clear m_queue either way, release m_target's
    //  own referenced CAgent if this was the last reference, then clear
    //  m_flags' own bits 0x2/0x4.  Named for what it does (unbind from
    //  whatever queue this timer was pending on) - nothing in this call
    //  tree gives the queue object a real type or a better name for this
    //  method itself.
    void Cancel();

    //  Declaration order is layout order (C++'s own rule) - m_target has to
    //  come first to land at +0x08 with m_queue right after it at +0x0C,
    //  matching CAgentTimer::CAgentTimer()'s own evidence (the CAgentPtr
    //  constructor call targets +0x08, the plain zero-store lands on
    //  +0x0C).
    CAgentPtr    m_target;   // +0x08
    void*        m_queue;    // +0x0C
    unsigned int m_flags;    // +0x10
};

#endif
