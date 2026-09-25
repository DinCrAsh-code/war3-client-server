//============================================================================
//  The 0x6F0517xx module: CBullet's own DumpState/SaveFlags/LoadFlags
//  overrides, each layering m_floatMini on top of CBulletBase's own
//  version of the same slot - the identical shape bulletbase_core.cpp
//  itself already uses for its own three embedded FloatMinis.
//============================================================================
#include "bullet.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"
#include "CFloat.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore*        __fastcall ReadCFloat(CDataStore* store, CFloat* out);

typedef void (__thiscall *FloatMiniLoadFn)(void*, void*, int);

//----------------------------------------------------------------------------
//  0x6F0517F0 - vtable slot 17, DumpState.  CBulletBase's own DumpState,
//  then this class's own m_floatMini through its own FloatMini::DumpTo
//  (0x6F4770E0 - the same address bulletbase.h's own three FloatMinis use).
//----------------------------------------------------------------------------
void CBullet::DumpState(void* sink)
{
    CBulletBase::DumpState(sink);
    m_floatMini.DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F0517C0 - vtable slot 18, SaveFlags.  CBulletBase's own SaveFlags,
//  then m_floatMini.m_value through WriteCFloat.
//----------------------------------------------------------------------------
void CBullet::SaveFlags(CDataStoreScratch* store)
{
    CBulletBase::SaveFlags(store);
    WriteCFloat(store, &m_floatMini.m_value);
}

//----------------------------------------------------------------------------
//  0x6F051810 - vtable slot 19, LoadFlags.  CBulletBase's own LoadFlags,
//  then a ReadCFloat into a local followed by m_floatMini's own
//  vtable-slot-0 "assign and notify" (same idiom bulletbase_core.cpp's own
//  three-FloatMini LoadFlags already uses).
//----------------------------------------------------------------------------
void CBullet::LoadFlags(CDataStore* store)
{
    CBulletBase::LoadFlags(store);

    CFloat v;
    ReadCFloat(store, &v);
    ((FloatMiniLoadFn)(*(void***)&m_floatMini)[0])(&m_floatMini, &v, 0);
}
