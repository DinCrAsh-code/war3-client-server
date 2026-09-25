//============================================================================
//  0x6F2C3C40 - CBulletBase's vtable slot 39, new past CWar3Image's own 39
//  slots.  Calls the real (non-virtual) CWar3Image::CreateSpriteFromDescriptor
//  first, copies this class's own descriptor fields, and arms the target
//  ref plus a sprite-controller position-follow callback.
//
//  Best-effort: the shipped body resolves this object's own +0x2C target
//  ref twice in a row - once directly (SOptionalHandleRefResolver::
//  ResolveChained, 0x6F0419C0, handlereref_resolve.cpp) and again through
//  the full SetTargetRef (0x6F2C2AC0, missile_thunderbolt.h), which itself
//  calls ClearTargetRef+ResolveChained - a genuine-looking redundancy in
//  the shipped stream (both calls take the identical `desc->+0x38`
//  argument) that this reconstruction transcribes rather than "fixes".
//  Not yet scored EXACT; see docs/targets/CBulletBase.md.
//============================================================================
#include "bulletbase.h"
#include "sprite.h"
#include "missile_thunderbolt.h"

struct SOptionalHandleRefResolver : SOptionalHandleRef
{
    SOptionalHandleRef& ResolveChained(const void* ref);
};

void __fastcall SetSpriteStateBits(CSprite* self, unsigned short bits, int set);
void __fastcall BindControllerSlotDirect(void* controller, int slot,
                                         void* callback, void* context,
                                         float blend);
void __fastcall CBulletBase_SpriteFollowCallback(void* self, void* out);

//  0x6F0418E0-family FloatMini "assign and notify" - vtable slot 0, two
//  args (see Math/floatmini.h's own FloatMiniB::SetPair for the identical
//  shape).
typedef void (__thiscall *FloatMiniAssignFn)(void* self, const void* value, int notify);

void CBulletBase::CreateSpriteFromDescriptor(const SWidgetArtDescriptor* desc)
{
    CWar3Image::CreateSpriteFromDescriptor(desc);

    const char* d = (const char*)desc;

    ((SOptionalHandleRefResolver*)&m_targetHandle)->ResolveChained(*(const void**)(d + 0x38));

    m_fieldA = *(const unsigned int*)(d + 0x3C);
    m_fieldB = *(const unsigned int*)(d + 0x40);

    ((FloatMiniAssignFn)(*(void***)&m_floatA)[0])(&m_floatA, d + 0x44, 1);
    ((FloatMiniAssignFn)(*(void***)&m_floatB)[0])(&m_floatB, d + 0x48, 1);
    ((FloatMiniAssignFn)(*(void***)&m_floatC)[0])(&m_floatC, d + 0x4C, 1);

    m_fieldC = *(const unsigned int*)(d + 0x54);
    m_fieldD = *(const unsigned int*)(d + 0x58);
    m_fieldE = *(const unsigned int*)(d + 0x60);

    //  this->Method_0xAC-style field copy, direct (non-virtual) call in
    //  the shipped stream: sub_6F2C2AC0 is CMissileThunderBoltRefs::
    //  SetTargetRef (missile_thunderbolt.h) called on `this`.
    ((CMissileThunderBoltRefs*)this)->SetTargetRef(*(void**)(d + 0x38));

    if (*(const unsigned int*)(d + 0x5C) != 0)
        m_flags |= 2u;

    SetSpriteStateBits(m_pSprite, 7, 1);
    BindControllerSlotDirect(m_pSprite, 1,
                              (void*)&CBulletBase_SpriteFollowCallback, this, 0.0f);
}
