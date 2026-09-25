//============================================================================
//  0x6F3E45C0 / 0x6F3E4600 - CCameraSetup::Save/Load, vtable slots 14/15.
//  Each calls the CAgentWar3 base first, then carries the three raw floats
//  at +0xA4/+0xA8/+0xAC directly through Net/cdatastore_write|readcfloat.cpp
//  - no registration call, unlike every other property in this class.
//============================================================================
#include "camerasetup.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"
#include "CFloat.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store,
                                          const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//  Both bodies keep the three floats' base address in one register across
//  all three calls (`add edi,0A4h` once, then `lea edx,[edi+4]`/`[edi+8]`)
//  rather than re-deriving each of the three addresses from `this` - naming
//  the pointer once and indexing off it, instead of taking `&m_saveFloatN`
//  three separate times, is what asks this compiler for that same reuse
//  (docs/msvc-vc8-idioms.md, "Naming an intermediate parks it in a
//  callee-saved register").
void CCameraSetup::Save(CDataStoreScratch* store)
{
    CAgentWar3::Save(store);
    const CFloat* p = (const CFloat*)&m_saveFloat0;
    WriteCFloat(store, p);
    WriteCFloat(store, p + 1);
    WriteCFloat(store, p + 2);
}

void CCameraSetup::Load(CDataStore* store)
{
    CAgentWar3::Load(store);
    CFloat* p = (CFloat*)&m_saveFloat0;
    ReadCFloat(store, p);
    ReadCFloat(store, p + 1);
    ReadCFloat(store, p + 2);
}
