//============================================================================
//  The 0x6F0A43xx module: CMissile's own DumpState/SaveFlags/LoadFlags
//  overrides.  All three call CBulletBase's own implementation *directly*
//  (non-virtually) rather than chaining through CBullet's own layered
//  version - confirmed by the call targets (0x6F051520/0x6F051470/
//  0x6F0515D0, all CBulletBase's own addresses per bulletbase.h, not
//  CBullet's 0x6F0517F0/0x6F0517C0/0x6F051810) - so each of these
//  re-does CBullet's own single-field addition (m_floatMini) inline
//  alongside its own five new fields, rather than delegating it.
//============================================================================
#include "missile.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"
#include "textsink.h"
#include "CFloat.h"

//  0x6F6EFE00 / 0x6F6EED50 - same helpers bulletbase_core.cpp already
//  declares for this exact purpose.
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore*        __fastcall ReadCFloat(CDataStore* store, CFloat* out);

typedef void (__thiscall *FloatMiniLoadFn)(void*, void*, int);

//----------------------------------------------------------------------------
//  0x6F0A4330 - vtable slot 17, DumpState.  CBulletBase::DumpState(sink)
//  directly, then m_floatMini (CBullet's own +0x88 field, dumped here
//  rather than through CBullet::DumpState), the +0xA0 ref through
//  SRefMiniValue::DumpTo (same dumper CBulletBase's own m_targetHandle
//  pair uses), then m_floatD/m_floatE.
//----------------------------------------------------------------------------
void CMissile::DumpState(void* sink)
{
    CBulletBase::DumpState(sink);
    m_floatMini.DumpTo(sink);
    ((SRefMiniValue*)&m_fieldA0Handle)->DumpTo(sink);
    m_floatD.DumpTo(sink);
    m_floatE.DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F0A42B0 - vtable slot 18, SaveFlags.  CBulletBase::SaveFlags(store)
//  directly, then m_floatMini through WriteCFloat, m_fieldA0Handle/
//  m_fieldA4TypeTag through plain WriteDword, m_reservedA8 through
//  WriteDwordAlt2, then m_floatD/m_floatE through WriteCFloat.
//----------------------------------------------------------------------------
void CMissile::SaveFlags(CDataStoreScratch* store)
{
    CBulletBase::SaveFlags(store);
    WriteCFloat(store, &m_floatMini.m_value);
    store->WriteDword(m_fieldA0Handle);
    store->WriteDword((unsigned int)m_fieldA4TypeTag);
    store->WriteDwordAlt2(m_reservedA8);
    WriteCFloat(store, &m_floatD.m_value);
    WriteCFloat(store, &m_floatE.m_value);
}

//----------------------------------------------------------------------------
//  0x6F0A4380 - vtable slot 19, LoadFlags.  CBulletBase::LoadFlags(store)
//  directly, then m_floatMini/m_floatD/m_floatE each through ReadCFloat and
//  their own vtable-slot-0 "assign and notify" - the same idiom
//  CBulletBase's own three FloatMinis use (bulletbase_core.cpp).  Does
//  *not* read back m_fieldA0Handle/m_fieldA4TypeTag/m_reservedA8 at all -
//  confirmed against the raw disassembly, not a modelling gap: SaveFlags
//  writes five extra fields past the base, LoadFlags reads only three of
//  them back.
//----------------------------------------------------------------------------
void CMissile::LoadFlags(CDataStore* store)
{
    CBulletBase::LoadFlags(store);

    CFloat v;
    ReadCFloat(store, &v);
    ((FloatMiniLoadFn)(*(void***)&m_floatMini)[0])(&m_floatMini, &v, 0);
    ReadCFloat(store, &v);
    ((FloatMiniLoadFn)(*(void***)&m_floatD)[0])(&m_floatD, &v, 0);
    ReadCFloat(store, &v);
    ((FloatMiniLoadFn)(*(void***)&m_floatE)[0])(&m_floatE, &v, 0);
}
