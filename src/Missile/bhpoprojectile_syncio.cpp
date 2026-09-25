//============================================================================
//  0x6F496AA0 / 0x6F496B90 - CBhPoProjectile::Save/Load, vtable slots 9/10.
//  See bhpoprojectile.h's own header comment for the shape and the one
//  real Save/Load asymmetry (Load's own version-gated tail).
//============================================================================
#include "bhpoprojectile.h"
#include "prbehavior.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

//  Already-real leaf helpers this session found under different names
//  elsewhere (Pathfinding/pathref_save.cpp / pathref_load.cpp) - redeclared
//  locally the same way every other TU in this repo forwards to a shared
//  address without a second, disagreeing definition.
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//  0x6F496AA0.
void CBhPoProjectile::Save(CDataStoreScratch* store)
{
    ((NIpse::CPrBehavior*)this)->Save(store);

    //  Each of these three groups shares one base-pointer computation
    //  (`+1`/`+2` from it) rather than an independent address-of per
    //  field - that grouping is real, not a style choice: the shipped
    //  code computes one `lea` into a register and reaches every further
    //  field in its own group off that same register, never recomputing
    //  the base address from `esi`/`this` again until the next group.
    CFloat* pAB = (CFloat*)&m_reserved38;
    WriteCFloat(store, pAB);
    WriteCFloat(store, pAB + 1);

    CFloat* pStart = (CFloat*)&m_startX;
    WriteCFloat(store, pStart);
    WriteCFloat(store, pStart + 1);
    WriteCFloat(store, pStart + 2);

    CFloat* pVel = (CFloat*)&m_velX;
    WriteCFloat(store, pVel);
    WriteCFloat(store, pVel + 1);

    //  Read early, written late - the shipped code caches this value in a
    //  stack temp well before the WriteDword call that actually uses it.
    unsigned int period = m_velPeriod;

    //  Unlike the seven calls above (each reloading `store` fresh), this
    //  quartet threads the writer's own return value through - real
    //  dataflow, not a stylistic choice this reconstruction is free to
    //  drop, since it is what the shipped instruction order shows.
    store = WriteCFloat(store, (const CFloat*)&m_velMag);
    store = WriteCFloat(store, (const CFloat*)&m_velOri);
    store = WriteCFloat(store, (const CFloat*)&m_reserved4C);
    store = WriteCFloat(store, (const CFloat*)&m_velTime);

    store = store->WriteDword(period);
    store = store->WriteDword(m_boundHandle68);
    store = store->WriteDword((unsigned int)m_boundTypeTag6C);
    store = store->WriteDwordAlt2(m_reserved70);

    store = WriteCFloat(store, (const CFloat*)&m_reserved74);
    WriteCFloat(store, (const CFloat*)&m_reserved78);
}

//  0x6F496B90 - a real virtual call through `store`'s own vtable slot 5
//  (`store->GetVersion()`, this session's own reading - nothing else in
//  this dump characterizes that slot further) gates the +0x68/+0x6C/+0x70
//  group: a save older than 0x1771 simply leaves them at whatever `Apply`
//  already reset them to.
typedef unsigned int (__thiscall *GetStoreVersionFn)(CDataStore*);

void CBhPoProjectile::Load(CDataStore* store)
{
    ((NIpse::CPrBehavior*)this)->Load(store);

    //  Same three shared-base-pointer groups as Save above.
    CFloat* pAB = (CFloat*)&m_reserved38;
    ReadCFloat(store, pAB);
    ReadCFloat(store, pAB + 1);

    CFloat* pStart = (CFloat*)&m_startX;
    ReadCFloat(store, pStart);
    ReadCFloat(store, pStart + 1);
    ReadCFloat(store, pStart + 2);

    CFloat* pVel = (CFloat*)&m_velX;
    ReadCFloat(store, pVel);
    ReadCFloat(store, pVel + 1);

    CDataStore* next = ReadCFloat(store, (CFloat*)&m_velMag);
    next = ReadCFloat(next, (CFloat*)&m_velOri);
    next = ReadCFloat(next, (CFloat*)&m_reserved4C);
    next = ReadCFloat(next, (CFloat*)&m_velTime);

    next->ReadDword(&m_velPeriod);

    GetStoreVersionFn getVersion = (GetStoreVersionFn)(*(void***)store)[0x14 / 4];
    if (getVersion(store) >= 0x1771)
    {
        store->ReadDword(&m_boundHandle68);
        store->ReadDword((unsigned int*)&m_boundTypeTag6C);
        store->ReadDwordAlt3(&m_reserved70);

        CDataStore* tail = ReadCFloat(store, (CFloat*)&m_reserved74);
        ReadCFloat(tail, (CFloat*)&m_reserved78);
    }
}
