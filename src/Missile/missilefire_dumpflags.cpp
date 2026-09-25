//============================================================================
//  The 0x6F0F66xx/0x6F0F67xx module: CMissileFire's own DumpState/
//  SaveFlags/LoadFlags overrides.
//
//  DumpState (0x6F0F6760) calls CBulletBase's own implementation
//  *directly* (0x6F051520, confirmed against the call target - not
//  CBullet's 0x6F0517F0 or CMissile's 0x6F0A4330), the same "skip straight
//  to CBulletBase" shape CMissile's own DumpState already uses one level
//  down - so this level re-does CBullet's and CMissile's own field
//  additions inline (m_floatMini, the +0xA0 ref, m_floatD/m_floatE)
//  alongside its own four new fields, rather than delegating any of it.
//
//  SaveFlags/LoadFlags do *not* follow that shape: both call CMissile's
//  own implementation directly (0x6F0A42B0/0x6F0A4380, confirmed against
//  the call targets), chaining through the immediate base the ordinary
//  way, then add only this level's own four new fields.
//============================================================================
#include "missilefire.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"
#include "textsink.h"
#include "CFloat.h"

//  0x6F6EFE00 / 0x6F6EED50 - same helpers missile_dumpflags.cpp already
//  declares for this exact purpose.
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore*        __fastcall ReadCFloat(CDataStore* store, CFloat* out);

typedef void (__thiscall *FloatMiniLoadFn)(void*, void*, int);

//----------------------------------------------------------------------------
//  0x6F0F6760 - vtable slot 17, DumpState.  CBulletBase::DumpState(sink)
//  directly, then m_floatMini (CBullet's own +0x88 field), the +0xA0 ref
//  through SRefMiniValue::DumpTo, m_floatD/m_floatE (CMissile's own +0x90/
//  +0x98 fields), then this level's own m_fieldAC/m_fieldB0 through
//  SIntMiniValue::DumpTo and m_floatF/m_floatG through FloatMini::DumpTo.
//----------------------------------------------------------------------------
void CMissileFire::DumpState(void* sink)
{
    CBulletBase::DumpState(sink);
    m_floatMini.DumpTo(sink);
    ((SRefMiniValue*)&m_fieldA0Handle)->DumpTo(sink);
    m_floatD.DumpTo(sink);
    m_floatE.DumpTo(sink);
    ((SIntMiniValue*)&m_fieldAC)->DumpTo(sink);
    ((SIntMiniValue*)&m_fieldB0)->DumpTo(sink);
    m_floatF.DumpTo(sink);
    m_floatG.DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F0F6700 - vtable slot 18, SaveFlags.  CMissile::SaveFlags(store)
//  directly (chains through the immediate base here, unlike DumpState
//  above), then m_fieldAC/m_fieldB0 through plain WriteDword, then
//  m_floatF/m_floatG through WriteCFloat.
//----------------------------------------------------------------------------
void CMissileFire::SaveFlags(CDataStoreScratch* store)
{
    CMissile::SaveFlags(store);
    store->WriteDword((unsigned int)m_fieldAC);
    store->WriteDword((unsigned int)m_fieldB0);
    WriteCFloat(store, &m_floatF.m_value);
    WriteCFloat(store, &m_floatG.m_value);
}

//----------------------------------------------------------------------------
//  0x6F0F67E0 - vtable slot 19, LoadFlags.  CMissile::LoadFlags(store)
//  directly, then m_fieldAC/m_fieldB0 read back through plain ReadDword,
//  then m_floatF/m_floatG each through ReadCFloat and their own
//  vtable-slot-0 "assign and notify" - the same idiom CMissile's own
//  LoadFlags uses for its own three FloatMinis one level down.
//----------------------------------------------------------------------------
void CMissileFire::LoadFlags(CDataStore* store)
{
    CMissile::LoadFlags(store);

    unsigned int tmp;
    store->ReadDword(&tmp);
    m_fieldAC = (int)tmp;
    store->ReadDword(&tmp);
    m_fieldB0 = (int)tmp;

    CFloat v;
    ReadCFloat(store, &v);
    ((FloatMiniLoadFn)(*(void***)&m_floatF)[0])(&m_floatF, &v, 0);
    ReadCFloat(store, &v);
    ((FloatMiniLoadFn)(*(void***)&m_floatG)[0])(&m_floatG, &v, 0);
}
