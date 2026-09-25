//============================================================================
//  The 0x6F0514xx-0x6F0516xx module: the rest of CBulletBase's own vtable
//  overrides that live next to the constructor (bulletbase_ctordtor.cpp is
//  a separate file only because its own destructor-inlining rule forces
//  the split - see that file's header).
//============================================================================
#include "bulletbase.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"
#include "textsink.h"
#include "handletriple.h"
#include "CFloat.h"

//  0x6F6EFE00 / 0x6F6EED50 - cdatastore_writecfloat.cpp/cdatastore_readcfloat.cpp.
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore*        __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//  Explicit vtable-slot dispatch on the embedded CAgentTimer/TRefCnt
//  object at +0x64 - CAgentTimer's own header (agenttimer.h) does not
//  model a Save/Load virtual because nothing else in this repo's own
//  closure reaches one; house style (CLAUDE.md) for a real indirect call
//  with no modelled virtual is the raw offset idiom below.
typedef void (__thiscall *TimerStoreFn)(void*, void*);

//----------------------------------------------------------------------------
//  0x6F051460 - vtable slot 10, GetRecordVersion.  A direct (non-virtual)
//  call to the shared CAgent::GetRecordVersion leaf (agent.h, "always 0"),
//  plus a per-level additive constant - the same shape CAgentWar3's own
//  override of this slot already uses one level up (agentwar3.h, +1
//  there), +10 at this level.
//----------------------------------------------------------------------------
int CBulletBase::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 10;
}

//----------------------------------------------------------------------------
//  0x6F051590 - vtable slot 14, Save.  Base Save(store), then the embedded
//  timer's own vtable slot 8.
//----------------------------------------------------------------------------
void CBulletBase::Save(CDataStoreScratch* store)
{
    CWar3Image::Save(store);
    ((TimerStoreFn)(*(void***)&m_timer)[8])(&m_timer, store);
}

//----------------------------------------------------------------------------
//  0x6F0515B0 - vtable slot 15, Load.  Same shape, timer's own slot 9.
//----------------------------------------------------------------------------
void CBulletBase::Load(CDataStore* store)
{
    CWar3Image::Load(store);
    ((TimerStoreFn)(*(void***)&m_timer)[9])(&m_timer, store);
}

//----------------------------------------------------------------------------
//  0x6F051520 - vtable slot 17, DumpState.  No base call: the inherited
//  chain up to CAgent::DumpState is a no-op the compiler elides (agent.h).
//  Dumps this class's own ten new fields, offset order, each through the
//  dumper its own type uses elsewhere in this repo (Storm/textsink.h):
//  plain dwords through SIntMiniValue, the target ref through SRefMiniValue,
//  the three FloatMinis through their own FloatMini::DumpTo.
//----------------------------------------------------------------------------
void CBulletBase::DumpState(void* sink)
{
    ((SIntMiniValue*)&m_flags)->DumpTo(sink);
    ((SRefMiniValue*)&m_targetHandle)->DumpTo(sink);
    ((SIntMiniValue*)&m_fieldA)->DumpTo(sink);
    ((SIntMiniValue*)&m_fieldB)->DumpTo(sink);
    m_floatA.DumpTo(sink);
    m_floatB.DumpTo(sink);
    m_floatC.DumpTo(sink);
    ((SIntMiniValue*)&m_fieldC)->DumpTo(sink);
    ((SIntMiniValue*)&m_fieldD)->DumpTo(sink);
    ((SIntMiniValue*)&m_fieldE)->DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F051470 - vtable slot 18, SaveFlags.  Despite the inherited slot's
//  name, this level's override serialises every one of CBulletBase's own
//  new fields, not just flag bits.
//----------------------------------------------------------------------------
void CBulletBase::SaveFlags(CDataStoreScratch* store)
{
    store->WriteDword(m_flags);
    store->WriteDword(m_targetHandle);
    store->WriteDword((unsigned int)m_targetTypeTag);
    store->WriteDword(m_reserved34);
    store->WriteDword(m_fieldA);
    store->WriteDword(m_fieldB);
    WriteCFloat(store, &m_floatA.m_value);
    WriteCFloat(store, &m_floatB.m_value);
    WriteCFloat(store, &m_floatC.m_value);
    store->WriteDword(m_fieldC);
    store->WriteDword(m_fieldD);
    store->WriteDword(m_fieldE);
}

//----------------------------------------------------------------------------
//  0x6F0515D0 - vtable slot 19, LoadFlags.  Reads the same fields back,
//  plus three sub-object loads through their own vtable slot 0 (matching
//  Save/Load's own m_timer dispatch shape, on the three FloatMinis this
//  time).
//----------------------------------------------------------------------------
typedef void (__thiscall *FloatMiniLoadFn)(void*, void*, int);

void CBulletBase::LoadFlags(CDataStore* store)
{
    unsigned int tmp;
    store->ReadDword(&tmp);
    m_flags = tmp;
    ReadHandleTriple(store, (SHandleTriple*)&m_targetHandle);
    store->ReadDword(&m_fieldA);
    store->ReadDword(&m_fieldB);

    CFloat v;
    ReadCFloat(store, &v);
    ((FloatMiniLoadFn)(*(void***)&m_floatA)[0])(&m_floatA, &v, 0);
    ReadCFloat(store, &v);
    ((FloatMiniLoadFn)(*(void***)&m_floatB)[0])(&m_floatB, &v, 0);
    ReadCFloat(store, &v);
    ((FloatMiniLoadFn)(*(void***)&m_floatC)[0])(&m_floatC, &v, 0);

    store->ReadDword(&m_fieldC);
    store->ReadDword(&m_fieldD);
    store->ReadDword(&m_fieldE);
}

//----------------------------------------------------------------------------
//  0x6F051420 - vtable slot 40, GetTrackedVector.  Default "no tracked
//  vector": zero the caller's 3-float buffer.  Reached polymorphically
//  through the sprite-controller callback (bulletbase_dispatch.cpp's own
//  CBulletBase_SpriteFollowCallback) - a subclass overriding this slot
//  supplies a real position/velocity instead.
//----------------------------------------------------------------------------
void CBulletBase::GetTrackedVector(float* out3) const
{
    out3[0] = 0.0f;
    out3[1] = 0.0f;
    out3[2] = 0.0f;
}

//----------------------------------------------------------------------------
//  0x6F051440 / 0x6F051450 - vtable slots 41 and 44.  Bare `retn`, no
//  stack args, always-true.  Named for slot index (house style,
//  agentwar3.h) - nothing in this dump's own closure pins a narrower
//  meaning to either.
//----------------------------------------------------------------------------
int CBulletBase::Method_0xA4() const
{
    return 1;
}

int CBulletBase::Method_0xB0() const
{
    return 1;
}
