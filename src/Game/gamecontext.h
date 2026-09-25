//============================================================================
//  The per-thread game context, and the two things this call tree reads out of
//  it: the pair of configuration files, and the lazily created CMiscCustom.
//
//  The context itself is a pointer held in a Storm-managed thread-local array,
//  fetched by index; index 0x0D is the one the data lookups use.  Nothing in
//  the tree reads any other index, and nothing reads any member of either
//  object beyond the ones below, so everything else is a reserved block.
//============================================================================
#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include "config.h"

class CMiscCustom;
class CAbilityDatabase;
class CAbilityCustomData;
class CItemCustomData;

//  Slot 0x0D of the per-thread array: the game's data context.
const unsigned int kThreadLocalGameData = 0x0D;

//----------------------------------------------------------------------------
//  Whatever hangs off the game context at +0x10.  It caches the CMiscCustom
//  at +0x4C; the CAbility depth<=5 closure
//  (Item/abilitygenericfieldrecord.cpp) reads two more lazily-created
//  slots off it, +0x14 and +0x40 - each behind its *own* lazy-init helper
//  (0x6F25EFE0 for +0x14, 0x6F25AC40 for +0x40), taking the field's own
//  array index from the cache's base (5 and 0x10 respectively, i.e. byte
//  offset / 4) rather than an address, so the two fields are named plainly
//  rather than modelled as one array.
//
//  Both are real, identified types as of the checksum-provider-registry
//  batch D session (docs/notes/checksum-provider-registry.md):
//    +0x14 - a CAbilityDatabase (Item/abilitydatabase.h), stamped directly
//            by the lazy-init helper's own inlined trivial constructor.
//    +0x40 - a CAbilityCustomData (Item/abilitycustomdata.h), constructed
//            by CAbilityCustomData::Construct() (0x6F256520) behind the
//            lazy-init helper's own real __try/__finally (that helper's own
//            SEH frame is the toolchain-unreproducible
//            __CxxFrameHandler3 shape docs/msvc-vc8-idioms.md documents,
//            not the source itself).
//
//  A third lazy singleton traced this batch (checksum-provider registry,
//  CUpgradeDatabase/CDestructableDatabase/CItemDatabase session,
//  docs/notes/checksum-provider-registry.md): +0x34 (array index 13,
//  0x0D) holds a CItemCustomData (Item/customdatasiblings.h), found the same
//  way while tracing CItemDatabase::SetRecordSet's own field-registration
//  helper chain - see Item/itemcustomdata_lazyinit.cpp.
//----------------------------------------------------------------------------
struct CGameDataCache
{
    char                 m_reserved00[0x14];
    CAbilityDatabase*    m_field14;              // 0x14 - lazy-init index 5
    char                 m_reserved18[0x34 - 0x18];
    CItemCustomData*     m_field34;              // 0x34 - lazy-init index 0x0D
    char                 m_reserved38[0x40 - 0x38];
    CAbilityCustomData*  m_field40;              // 0x40 - lazy-init index 0x10
    char                 m_reserved44[0x4C - 0x44];
    CMiscCustom*         m_pMiscCustom;          // 0x4C
};

//  0x6F25EFE0 - lazily create/fetch cache->m_field14 (index 5), a
//  CAbilityDatabase.  `retn 0`.  Item/abilitydatabase_lazyinit.cpp.
void* __fastcall LazyInitGameDataCacheField14(unsigned int index, unsigned int);
//  0x6F25AC40 - lazily create/fetch cache->m_field40 (index 0x10), a
//  CAbilityCustomData.  `retn 0`.  Item/abilitycustomdata_lazyinit.cpp.
void* __fastcall LazyInitGameDataCacheField40(unsigned int index, unsigned int);
//  0x6F25AB40 - lazily create/fetch cache->m_field34 (index 0x0D), a
//  CItemCustomData.  `retn 0`.  Item/itemcustomdata_lazyinit.cpp.
void* __fastcall LazyInitGameDataCacheField34(unsigned int index, unsigned int);

//----------------------------------------------------------------------------
//  The game data context.  Two configuration files: the one at +0x04 is
//  consulted first and only if it actually carries the key, so it is the
//  override, and +0x00 is the fallback.
//----------------------------------------------------------------------------
struct CGameDataContext
{
    CConfigFile*    m_pConfig;          // 0x00
    CConfigFile*    m_pConfigOverride;  // 0x04
    //  +0x08..+0x14 is a real, RTTI-confirmed `TSFixedArray<CSynchronousData*>`
    //  (Game/synchronousdata.h, storm.h) - `m_reserved08` is that array's own
    //  `m_alloc` field, `m_providerCount` its `m_count`, `m_pCache` its
    //  `m_data`. The constructor that installs this object
    //  (ConstructGameDataContext, Game/gamedatacontextctor.cpp) sets
    //  `m_providerCount` to a **fixed constant, 0x14 (20)**, once, and
    //  nothing else this repo has found ever touches it again - it is not
    //  "how many of m_pCache's leading slots are live checksum-provider
    //  entries", it is the array's own fixed capacity.  sub_6F005E20 (the
    //  real per-tick walk - GameDataContextChecksumProviderHash below) and
    //  the dead debug dump sub_6F5455A0
    //  (docs/notes/checksum-provider-registry.md, hard-coded 20) therefore
    //  cover the *same* 20 slots, always - including the three the cache
    //  repurposes for unrelated lazy singletons (CAbilityDatabase at +0x14,
    //  LazyInitGameDataCacheField40's target at +0x40, m_pMiscCustom at
    //  +0x4C) once whichever of those has actually been created.
    char            m_reserved08[4];
    unsigned int    m_providerCount;    // 0x0C
    CGameDataCache* m_pCache;           // 0x10
};

//  0x6F005E20 - fold every live checksum-provider slot's vtable-slot-0
//  result into one rolling hash, walking from the last populated slot
//  down to the first.  `this` = the per-thread CGameDataContext itself
//  (TLS slot 0x0D), not m_pCache - m_pCache is read fresh out of it once
//  and then indexed as a flat void* array rather than through any of
//  CGameDataCache's own named fields, which is why this takes the context
//  and not the cache.  Called by CNetData::FireTickNotification
//  (0x6F545840, Net/netcommand_dispatch.cpp) to build the "net" leg of the
//  per-tick turn-sync value - see docs/notes/checksum-provider-registry.md
//  for the whole mechanism this is one leg of.
unsigned int __fastcall GameDataContextChecksumProviderHash(CGameDataContext* context);

//  0x6F4C34D0 - one entry of this thread's pointer array, or null if the array
//  was never set up for this thread.
void* __fastcall GetThreadLocalSlot(unsigned int slot);

//  0x6F002790 - the map's configuration overrides, created on first use.
CMiscCustom* GetMiscCustom();

//  0x6F009EE0 - a named configuration value as a deterministic float.
CFloat __fastcall GetConfigFloat(const char* section, const char* key,
                                 int index);

#endif
