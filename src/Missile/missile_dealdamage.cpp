//============================================================================
//  0x6F2C2AF0 - CMissileThunderBolt's own "deal damage" call, frame [0]'s
//  own tail per the pipeline notes (missile+0x20 bit 0x20000000 guards a
//  single call to this).  CMissileThunderBolt impact pipeline, see
//  docs/targets/cmissilethunderbolt_impact_pipeline.md for the full
//  writeup and for what remains uncertain here.
//============================================================================
#include "missile_thunderbolt.h"
#include "game.h"
#include "CFloat.h"
#include "boolqueryowner.h"

struct SFVec3 { int x, y, z; };

//  vtable+0xB8 on the "data" object: called twice with different arities
//  (this is the same slot index sub_6F2BB4F0's own dispatch switch reaches
//  as "vtable+0xB8", but on a *different* object - `data`, not the
//  missile).
typedef void* (__thiscall *DataSlotB8NoArgFn)(void* self);
typedef void* (__thiscall *DataSlotB8PosFn)(void* self, SFVec3* outBuf);

//  vtable+0xA0 on the missile - the "no target" position fallback, taking
//  the same one out-buffer argument sub_6F29E6A0 (the target's own
//  resolve, deferred) does; the earlier `push` that sets that argument up
//  happens before the target-null branch and is shared by both arms.
typedef SFVec3* (__thiscall *MissileSlotA0Fn)(void* self, SFVec3* outBuf);

//  vtable+0xE4 on `data` - returns a CFloat via the FPU (fstp, not a
//  pointer): (this, a 12-byte {x,y,0.0f} point built from the resolved
//  position's x/y - z is a literal 0.0f, not the resolved z - plus three
//  int arguments (-1, 0, 0)).
struct SPartialPoint { CFloat x, y, zZero; };
typedef CFloat (__thiscall *DataSlotE4Fn)(void* self, SPartialPoint* point, int a, int b, int c);

//  vtable+0x118 on `data` - takes the address of DealDamage's own first
//  stack argument (`target`) and returns a pointer to a CFloat.
typedef CFloat* (__thiscall *DataSlot118Fn)(void* self, void** targetAddr);

//  sub_6F29E6A0 - resolves `target`'s own position into the caller-owned
//  out-buffer and returns that same pointer (deferred: its own closure is
//  a further five-function chain, sub_6F29E5F0/sub_6F4AFCD0/sub_6F07B950/
//  sub_6F2835A0/sub_6F07B8F0, not walked this session).  Naked jmp thunk to
//  the real shipped address (same pattern as
//  src/unreconstructed_thunks.cpp's MEMBER_THUNK / misc_adjuster_thunks.cpp)
//  so build_mix.py's full-DLL link resolves it - verify.py never links, so
//  a declared-only extern compiles clean here and only fails there.
struct STargetPositionResolver { SFVec3* __thiscall ResolvePosition(SFVec3* outBuf); };
__declspec(naked) SFVec3* __thiscall STargetPositionResolver::ResolvePosition(SFVec3*)
{
    __asm { mov eax, 0x6F29E6A0 }
    __asm { jmp eax }
}

//  sub_6F47A880 - a bool-normalising query on missile+0x78.  A real body
//  now, in boolquery.cpp on boolqueryresolve.cpp; declared here, on the
//  same class name, so this call site reaches it with a real `call`.

//  0x6F2BCC50 - CMissileThunderBoltRefs::Apply, missile_apply.cpp.

void __thiscall CMissileThunderBoltRefs::DealDamage(void* target, void* data)
{
    ClearTargetRef();
    SetTargetRef(target);
    ClearFieldA0Ref();
    SetFieldA0Ref(data);

    int gate = (int)((DataSlotB8NoArgFn)(*(void***)data)[0xB8 / 4])(data);

    //  Zeroed then unconditionally overwritten below - dead by the time
    //  DealDamage returns (nothing in this function reads it again), but
    //  the shipped code still performs the zero-init: the compiler can't
    //  prove it dead once its address has escaped through the resolve
    //  call.  Kept for that reason, not because anything needs it.
    SFVec3 pos = { 0, 0, 0 };

    SFVec3 resolveBuf;
    SFVec3* resolved;
    if (target)
        resolved = ((STargetPositionResolver*)target)->ResolvePosition(&resolveBuf);
    else
        resolved = ((MissileSlotA0Fn)(*(void***)this)[0xA0 / 4])(this, &resolveBuf);
    pos = *resolved;

    //  One expression, not a named intermediate - the shipped code pushes
    //  SmartPtrToWorldVec3's own return buffer before the vtable+0xB8 call
    //  that produces its `this`, which MSVC only does when nothing names
    //  the pointer in between (the same idiom widget_slot65.cpp's own
    //  RefreshTerrainTint documents).  The position argument here is
    //  `resolved` (the resolve's own out-buffer), not `pos` - the shipped
    //  code reuses that buffer rather than the copy it just made.
    CWorldVec3 worldPos =
        ((const CPathRef*)((DataSlotB8PosFn)(*(void***)data)[0xB8 / 4])(data, resolved))
            ->SmartPtrToWorldVec3();

    //  Each component copied out as CFloat and reinterpreted as a hardware
    //  float, same idiom as RefreshTerrainTint's own three components -
    //  except only x/y feed the point below; z is a literal 0.0f.
    CFloat wx = worldPos.m_x;
    CFloat wy = worldPos.m_y;

    SPartialPoint point;
    point.x = wx;
    point.y = wy;
    point.zZero = CFloat();

    CFloat e4Result = ((DataSlotE4Fn)(*(void***)data)[0xE4 / 4])(data, &point, -1, 0, 0);

    CFloat* one18Result = ((DataSlot118Fn)(*(void***)data)[0x118 / 4])(data, &target);

    CFloat combined = e4Result + *one18Result;

    //  Not `worldPos.m_z` - the shipped code reuses the already-loaded raw
    //  x/y bits (wx/wy above) rather than reloading worldPos, and pairs
    //  them with `combined` as the point's own z.  CFloat's bit pattern is
    //  a hardware float's bit pattern by construction (CFloat.h), so
    //  reusing the raw int bits here costs nothing.
    SImpactPoint point2;
    point2.x = wx;
    point2.y = wy;
    point2.z = combined;

    int boolFlag = ((SBoolQueryOwner*)((char*)this + 0x78))->QueryBool();
    int finalFlag = -boolFlag & gate;

    //  `&pos.z` - the shipped code passes the address of the dead `pos`
    //  local's own z field as this opaque first argument (Apply itself
    //  never dereferences it, per sub_6F2BCC50's own body - just forwards
    //  it to sub_6F47A650). Very likely incidental stack-slot reuse rather
    //  than a meaningful "z address" - flagged, not claimed as intentional.
    Apply(&pos.z, &point2, finalFlag);
}
