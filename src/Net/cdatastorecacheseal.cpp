//============================================================================
//  0x6F4C75E0 - set the "no more will be added" bit and then retire every
//  chunk the cache has finished with, once.
//
//  Its own translation unit so the drain stays a real call: in one TU /Ob2
//  inlines the whole loop and the tail jump disappears.
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

//  0x6F4C7150 - the drain itself; own TU.
void __fastcall DataStoreCacheDrain(SDataStoreCache* self);

void __fastcall DataStoreCacheSeal(SDataStoreCache* self)
{
    if (self->m_flags & 1)
        return;

    self->m_flags |= 1;
    DataStoreCacheDrain(self);
}
