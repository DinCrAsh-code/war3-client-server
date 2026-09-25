//============================================================================
//  SResidueKey - see residuekey.cpp for the reconstruction and what its own
//  arithmetic does.  Split into a header because a second, unrelated call
//  tree (checksum-provider-registry, provider-slots-0-4: CRandData's own
//  45-record history buffer, Sync/randdata.h) turned out to share the same
//  0x6F4AB660 - the same "stamp a value and its four-modulus residue
//  signature" primitive, reused by two otherwise-unconnected callers.
//============================================================================
#ifndef RESIDUEKEY_H
#define RESIDUEKEY_H

struct SResidueKey
{
    //  0x6F4AB660.
    void Set(unsigned int value);

    unsigned int m_value;       // +0x00
    unsigned int m_residues;    // +0x04
};

#endif
