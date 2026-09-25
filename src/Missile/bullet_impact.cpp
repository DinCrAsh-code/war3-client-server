//============================================================================
//  0x6F2C3CF0 - CBullet's vtable slot 48, new past CBulletBase's own 48.
//  Calls the real (non-virtual) CBulletBase::CreateSpriteFromDescriptor
//  first, resolves a terrain height under the descriptor's own {x, y}
//  (the already-known deep terrain query, `QueryGroundHeightHelper`,
//  0x6F0126F0, item.cpp/cameraprojectground.cpp), installs the resulting
//  {x, y, height} triple on m_projectilePosition, always sets one flag
//  there, assigns m_floatMini from the descriptor's own +0x50 field
//  through the usual "assign and notify" vtable-slot-0 idiom, and forwards
//  to CMissileThunderBoltRefs::Apply (missile_thunderbolt.h) with a flag
//  read through this object's own new slot 49 (GetImpactApplyFlag,
//  bullet_core.cpp) only when the descriptor's own +0x64 is set - a
//  genuine virtual redispatch on `this`, not a direct call, so a subclass
//  overriding slot 49 changes what flag this hands to Apply.  Finally arms
//  a controller-slot callback the same way CBulletBase::
//  CreateSpriteFromDescriptor arms its own (bulletbase_createsprite.cpp),
//  slot 4 rather than slot 1, with its own callback address.
//
//  Best-effort: the two m_projectilePosition leaves this calls
//  (0x6F47A560, 0x6F47A990) are not further reconstructed this session -
//  see bullet_position.cpp's own header for why - and the impact callback
//  this arms (0x6F2BCCE0) is only ever taken by address here, never
//  called, so it is left a naked redirect rather than a real body.
//============================================================================
#include "bullet.h"
#include "sprite.h"
#include "missile_thunderbolt.h"

//  0x6F0126F0 - item.cpp's own redirect; re-declared here, never
//  re-thunked (item_place.cpp, jasslocationheight.cpp and
//  cameraprojectground.cpp do the same). This file used to carry a
//  second, independent naked definition of the same address - a real
//  LNK2005/LNK4006 duplicate-symbol defect (link_check.py's own
//  docstring names this exact class of bug) that build_mix.py's stricter
//  duplicate check turns fatal - fixed by declaring, not redefining.
float __fastcall QueryGroundHeightHelper(int flag, void* outPtr,
                                         float x, float y, int unused);

//  0x6F47A560 - install a resolved {x, y, height} triple on
//  m_projectilePosition.  Not reconstructed this session (bullet_position
//  .cpp's own header comment).
struct SProjectilePositionSet
{
    void __thiscall SetWorldPosition(const float* xyz3);
};
__declspec(naked) void __thiscall SProjectilePositionSet::SetWorldPosition(const float*)
{
    __asm { mov eax, 0x6F47A560 }
    __asm { jmp eax }
}

//  0x6F47A990 - a one-bit flag setter/clearer on the resolved CPathTrace,
//  gated on its own stack argument (agent_worktrees raw_asm: `cmp
//  [esp+arg_0],0` then either `or dword ptr[eax],02000000h` or `and
//  dword ptr[eax], 0FDFFFFFFh`).  Called here with a literal 1, i.e.
//  always the "set" branch.  Not reconstructed this session.
struct SProjectilePositionFlag
{
    void __thiscall SetFlag(int set);
};
__declspec(naked) void __thiscall SProjectilePositionFlag::SetFlag(int)
{
    __asm { mov eax, 0x6F47A990 }
    __asm { jmp eax }
}

typedef void (__thiscall *FloatMiniAssignFn)(void* self, const void* value, int notify);

void __fastcall BindControllerSlotDirect(void* controller, int slot,
                                         void* callback, void* context,
                                         float blend);

//  0x6F2BCCE0 - the impact-side controller callback this arms.  Only ever
//  taken by address at this call site, never called from here - left a
//  naked redirect rather than a guessed body.
void __fastcall CBullet_ImpactCallback(void* self, void* out);
__declspec(naked) void __fastcall CBullet_ImpactCallback(void*, void*)
{
    __asm { mov eax, 0x6F2BCCE0 }
    __asm { jmp eax }
}

void CBullet::OnImpact(const SWidgetArtDescriptor* desc, SImpactPoint* point)
{
    CBulletBase::CreateSpriteFromDescriptor(desc);

    const char* d = (const char*)desc;
    float x = *(const float*)(d + 0x00);
    float y = *(const float*)(d + 0x04);
    float height = QueryGroundHeightHelper(-1, 0, x, y, 0);

    float triple[3] = { x, y, height };
    ((SProjectilePositionSet*)&m_projectilePosition)->SetWorldPosition(triple);
    ((SProjectilePositionFlag*)&m_projectilePosition)->SetFlag(1);

    ((FloatMiniAssignFn)(*(void***)&m_floatMini)[0])(&m_floatMini, d + 0x50, 1);

    int flag = 0;
    if (*(const unsigned int*)(d + 0x64) != 0)
        flag = this->GetImpactApplyFlag();

    ((CMissileThunderBoltRefs*)this)->Apply((void*)(d + 8), point, flag);

    BindControllerSlotDirect(m_pSprite, 4, (void*)&CBullet_ImpactCallback, this, 0.0f);
}
