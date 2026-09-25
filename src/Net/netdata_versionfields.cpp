//============================================================================
//  0x6F534A20 / 0x6F534A30 / 0x6F534A60 - three one-instruction field reads
//  on the same versioned-stream overlay CDataStore::GetVersion
//  (datastoreversion.cpp, +0x238) already names, at +0x230/+0x234/+0x23C.
//
//  Its own translation unit for the same reason GetVersion's is: the only
//  caller that reaches these three (CNetData::LoadReplayHeader,
//  netdata_loadreplayheader.cpp) calls all three with real calls rather
//  than letting /Ob2 inline them, so nothing else in the codebase drags a
//  second, drifting copy of the overlay in with it.
//============================================================================
#include "cdatastore.h"

struct SVersionedStreamFields
{
    char m_reserved00[0x230];
    int  m_field230;    // +0x230
    int  m_field234;    // +0x234
    int  m_field238;    // +0x238 - CDataStore::GetVersion's own field
    int  m_field23C;    // +0x23C
};

//----------------------------------------------------------------------------
//  0x6F534A20
//----------------------------------------------------------------------------
int CDataStore::GetField230()
{
    return ((SVersionedStreamFields*)this)->m_field230;
}

//----------------------------------------------------------------------------
//  0x6F534A30
//----------------------------------------------------------------------------
int CDataStore::GetField234()
{
    return ((SVersionedStreamFields*)this)->m_field234;
}

//----------------------------------------------------------------------------
//  0x6F534A60
//----------------------------------------------------------------------------
int CDataStore::GetField23C()
{
    return ((SVersionedStreamFields*)this)->m_field23C;
}
