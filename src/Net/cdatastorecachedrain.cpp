//============================================================================
//  0x6F4C7150 - retire every chunk the cache has finished with, and
//  0x6F4C75E0 - set the "no more will be added" bit and then do exactly
//  that, once.
//
//  Both loops re-read the two counters rather than caching either, because
//  the retire itself advances one of them.
//============================================================================
#include <windows.h>

struct SDataStoreCache
{
    char         m_reserved00[0x34];
    unsigned int m_flags;       // +0x34
    char         m_reserved38[0x10];
    unsigned int m_limit;       // +0x48
    char         m_reserved4C[0x10];
    unsigned int m_retired;     // +0x5C
};

//  0x6F4C7060 - retire one chunk; own TU.
void __fastcall DataStoreCacheRetireOne(SDataStoreCache* self);

void __fastcall DataStoreCacheDrain(SDataStoreCache* self)
{
    while (self->m_retired < self->m_limit)
        DataStoreCacheRetireOne(self);
}
