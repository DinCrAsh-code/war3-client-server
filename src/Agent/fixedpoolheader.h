//============================================================================
//  SFixedPoolHeader - shared declaration.  Pulled out of
//  agentbasefixedpoolheader.cpp (which keeps the one real out-of-line
//  definition, 0x6F4C1A10) so a second caller - Net/netobserver.cpp's own
//  CNetObserver::CNetObserver, cnd-ctor-agent-2 session - can construct one
//  without a second, ODR-conflicting local declaration.  See that .cpp for
//  the constructor's own body and reasoning.
//============================================================================
#ifndef FIXEDPOOLHEADER_H
#define FIXEDPOOLHEADER_H

struct SFixedPoolHeader
{
    SFixedPoolHeader(unsigned int elementSize, unsigned int capacity);

    //  0x6F4C1A50 (IDA's nullsub_19) - genuinely empty; nothing here owns a
    //  resource to release.  Own translation unit
    //  (fixedpoolheaderdtor.cpp) so a call site (CNetObserver::~CNetObserver,
    //  Net/netobserver.cpp) keeps a real out-of-line `call` rather than
    //  this trivial body vanishing inline.
    ~SFixedPoolHeader();

    unsigned int m_elementSize;    // +0x00, clamped >= 4
    unsigned int m_capacity;       // +0x04, clamped >= 1
    unsigned int m_reserved08;     // +0x08
    unsigned int m_reserved0C;     // +0x0C
    unsigned int m_reserved10;     // +0x10
};

#endif
