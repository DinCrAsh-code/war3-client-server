//============================================================================
//  SNetPlayerDataTable and AppendPlayerDataRecords - see
//  netdata_appendplayerdata.cpp for the derivation.  Declared here so that
//  file and CNetData::LoadReplayHeader (netdata_loadreplayheader.cpp) agree
//  on one type and one mangled name.
//============================================================================
#ifndef NETDATA_APPENDPLAYERDATA_H
#define NETDATA_APPENDPLAYERDATA_H

struct SNetPlayerDataTable
{
    //  0x6F549C90 - `this` in ecx, both `count` and `srcRecords` on the
    //  stack (`retn 8`): a real member, not a two-register `__fastcall`,
    //  because the shipped call site pushes `count` rather than putting it
    //  in edx.
    int Append(unsigned int count, const void* srcRecords);

    int   m_capacity;       // +0x00
    int   m_count;          // +0x04
    void* m_data;           // +0x08
    int   m_elemSizeHint;   // +0x0C - read only; never written in this closure
};

#endif
