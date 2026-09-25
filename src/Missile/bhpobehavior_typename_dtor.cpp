//============================================================================
//  0x6F479EA0 / 0x6F479F20 / 0x6F479EB0 - CBhPoBehavior's own vtable slots
//  0/1/2, from the operator's 2026-09-07 RTTI-namespace vtable-dump fix
//  (agent_worktrees/classes/0x6F951DA4.json, "NIpse::CBhPoBehavior"). Part
//  of the "cluster G" 4-class sweep alongside CBhPoProjectile/
//  CPoPoInterfLis/CPoPoVelocityMod - see bhpobehavior.h's own file comment
//  for why this class is not (yet) modelled as a real C++ base or derived
//  class of CBhPoProjectile.
//============================================================================
#include "bhpobehavior.h"
#include "storm.h"
#include "game.h"   // GetTimeSyncField838

//----------------------------------------------------------------------------
//  0x6F479EA0 - vtable slot 0.
//----------------------------------------------------------------------------
const char* CBhPoBehavior::GetTypeName()
{
    return "BhPoBehavior";
}

//----------------------------------------------------------------------------
//  0x6F479F20 - vtable slot 1, the real destructor. Byte-for-byte the same
//  shape as `CBhPoProjectile::~CBhPoProjectile`
//  (Missile/bhpoprojectile_typename_dtor.cpp), against this class's own
//  pool (`GetTimeSyncField838`, +0x838, vs CBhPoProjectile's own +0x858).
//----------------------------------------------------------------------------
CBhPoBehavior::~CBhPoBehavior()
{
    char* pool = (char*)GetTimeSyncField838();

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
//  0x6F479EB0 - vtable slot 2, ScalarDeletingDestructor. Byte-identical to
//  `CBhPoProjectile::ScalarDeletingDestructorTrivial`
//  (Missile/bhpoprojectile_typename_dtor.cpp) - see bhpobehavior.h's own
//  declaration comment.
//----------------------------------------------------------------------------
void* CBhPoBehavior::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}
