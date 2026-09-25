//============================================================================
//  0x6F256190 - CCustomData::ComputeChecksum(), overriding
//  CSynchronousData::ComputeChecksum() (vtable slot 0, shared by
//  CAbilityCustomData and every other not-yet-found CCustomData subclass -
//  see abilitycustomdata.h's own header note).
//
//  Folds this object's own m_pPool checksum cache unconditionally, then
//  additionally XORs in m_ownedRef's own checksum cache if m_ownedRef is
//  non-null, before a final `rol 3` over the combined value - the same
//  `rol(hash ^ contribution, 3)` fold CGameWar3::ComputeChecksum and
//  CAbilityDatabase::GetChecksum already establish for this family
//  (checksum-provider-registry.md).  Both sub-object walkers
//  (PoolChecksumCacheHash/OwnedRefChecksumCacheHash) are real
//  reconstructions now too - Misc/poolchecksumcachehash.cpp and
//  Misc/ownedrefchecksumcachehash.cpp (customdatachecksumcache.h) -
//  reached from `this`'s own m_pPool (+0x0C) and m_ownedRef (+0x08) fields.
//
//  m_ownedRef is documented elsewhere (abilitycustomdata.h) as always null
//  through this repo's own Construct() redirect, so the m_ownedRef half of
//  this fold is dead for every object this repo can build but real and
//  exercised for whichever not-yet-found construction path leaves it
//  non-null - same situation ReleaseOwnedRefAndFinalize() already
//  documents for the same field.
//============================================================================
#include "abilitycustomdata.h"
#include "customdatachecksumcache.h"

//  Field offsets: m_ownedRef is at +0x08, m_pPool at +0x0C (abilitycustomdata.h).
//  Read here through a raw byte view rather than through CCustomData's own
//  private members - this TU is not a member function's own body reaching
//  its own `this` fields the ordinary way because the two helper calls
//  below take a plain `void*`, not a `CCustomData*`, matching the shipped
//  disassembly's own `mov ecx,[esi+0Ch]` / `mov ecx,[esi+8]` reads off a
//  bare object pointer with no class assumed past this point.
struct SCCustomDataChecksumFields
{
    char  m_reserved00[8];   // vtable ptr (+0x00) and m_type (+0x04)
    void* m_ownedRef;        // +0x08
    void* m_pPool;           // +0x0C
};

unsigned int CCustomData::ComputeChecksum()
{
    SCCustomDataChecksumFields* self = (SCCustomDataChecksumFields*)this;

    unsigned int hash = PoolChecksumCacheHash(self->m_pPool);

    //  The final `rol 3` only happens on the m_ownedRef branch - when
    //  m_ownedRef is null the shipped code returns PoolChecksumCacheHash's
    //  own result completely unfolded, per the dump's own `je` target
    //  landing past the `rol eax, 3` as well as the call.
    if (self->m_ownedRef != 0)
    {
        hash ^= OwnedRefChecksumCacheHash(self->m_ownedRef);
        hash = (hash << 3) | (hash >> 29);
    }

    return hash;
}
