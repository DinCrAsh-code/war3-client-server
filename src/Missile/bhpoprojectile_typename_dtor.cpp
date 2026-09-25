//============================================================================
//  0x6F47A090 / 0x6F47A430 / 0x6F47A470 - CBhPoProjectile's own vtable slots
//  0/1/2, from the operator's 2026-09-07 RTTI-namespace vtable-dump fix
//  (agent_worktrees/classes/0x6F951E04.json, "NIpse::CBhPoProjectile" - see
//  docs/targets/NTempestNIpse_BaseChain.md). Part of the "cluster G" 4-class
//  sweep alongside CBhPoBehavior/CPoPoInterfLis/CPoPoVelocityMod.
//============================================================================
#include "bhpoprojectile.h"
#include "storm.h"
#include "game.h"   // GetTimeSyncField858

//----------------------------------------------------------------------------
//  0x6F47A090 - vtable slot 0. Returns the shipped-source class name as a
//  literal string, byte-for-byte the same shape every other class in this
//  vtable family uses at slot 0 (Agent/presence.h's own `GetTypeName`
//  precedent). `mov eax, offset ...; retn` - no `this` read at all.
//----------------------------------------------------------------------------
const char* CBhPoProjectile::GetTypeName()
{
    return "BhPoProjectile";
}

//----------------------------------------------------------------------------
//  0x6F47A430 - vtable slot 1, the real (non-scalar) destructor. Splices
//  `this` out of its own intrusive doubly-linked list (+0x04 = next,
//  +0x08 = prev, both null-checked before the far-side write), zeroes both
//  links, then returns the object to CBhPoProjectilePool's own fast free
//  list exactly the way CBhPoProjectilePool::AllocateFromPool
//  (Missile/bhpoprojectile_attach.cpp) pops it back off: the allocator's own
//  header dword sits 4 bytes before the object (`raw = this - 4`, mirroring
//  that function's own `obj = raw + 4`), threaded onto the pool's own
//  `m_fastFreeHead` (+0x14) and counted down at its own +0x18. Addressed by
//  raw offset from the pool pointer rather than through
//  `CBhPoProjectilePool` itself - that struct is declared inside
//  bhpoprojectile_attach.cpp's own translation unit, not this one.
//----------------------------------------------------------------------------
CBhPoProjectile::~CBhPoProjectile()
{
    char* pool = (char*)GetTimeSyncField858();

    void* next = *(void**)((char*)this + 4);
    void* prev = *(void**)((char*)this + 8);
    if (next != 0)
        *(void**)((char*)next + 8) = prev;
    if (prev != 0)
        *(void**)((char*)prev + 4) = next;
    *(void**)((char*)this + 4) = 0;
    *(void**)((char*)this + 8) = 0;

    void* raw = (char*)this - 4;
    *(void**)raw = *(void**)(pool + 0x14);
    --*(unsigned int*)(pool + 0x18);
    *(void**)(pool + 0x14) = raw;
}

//----------------------------------------------------------------------------
//  0x6F47A470 - vtable slot 2, ScalarDeletingDestructor. Byte-identical in
//  shape to `NTempest::CPresence::ScalarDeletingDestructorTrivial`
//  (Agent/presence_dtor.cpp) and to `CBhPoBehavior`'s own
//  (Missile/bhpobehavior.h): restamps straight to `??_7CPresence@NTempest@
//  @6B@` with no base-class teardown call at all, then frees through Storm
//  if bit 0 of `flags` is set. Named `ScalarDeletingDestructorTrivial` for
//  the same reason presence.h's own is - see that header's own file
//  comment for the open question of why this multi-class shape skips real
//  teardown.
//----------------------------------------------------------------------------
void* CBhPoProjectile::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}
