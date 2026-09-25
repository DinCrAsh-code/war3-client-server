//============================================================================
//  0x6F323020 - see hashtablehashonlylookup.cpp.  Declared here so the
//  caller (sub_6F324920, unittypecacheexpiry.cpp) can compile against the
//  real member rather than a second, independently-typed redeclaration.
//============================================================================
#ifndef HASHTABLEHASHONLY_H
#define HASHTABLEHASHONLY_H

struct SRawHashBucket
{
    int m_linkoffset;   // +0x00 - TSExplicitList's own runtime link offset
    char m_reserved04[0x08];  // m_terminator.m_next (unused here)
    // +0x08 is m_terminator.m_prevlink, read directly below by offset
};

struct SRawHashTableView
{
    char             m_reserved00[0x1C];
    SRawHashBucket*  m_buckets;   // +0x1C
    char             m_reserved20[0x24 - 0x20];
    unsigned int     m_mask;      // +0x24

    void* LookupByHashOnly(unsigned int hashval, const void* key);
};

#endif
