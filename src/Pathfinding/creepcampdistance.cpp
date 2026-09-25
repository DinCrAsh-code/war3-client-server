//============================================================================
//  0x6F477070 - a small helper in CPathTrace::RampValueAt's own module
//  (0x6F477180, pathtraceramp.cpp - address neighbourhood), and the
//  createunit-G3 nearest-creep-spot search's own distance probe
//  (creepcampfind.cpp) is its only caller in this closure.
//
//  Resolves `other` to a world-space CWorldVec3 - through
//  CPathRef::SmartPtrToWorldVec3 (already reconstructed,
//  Pathfinding/CPathRef.cpp) when `useSmartPtr` is set, or through
//  0x6F474680 otherwise (a second, unnamed resolver, outside this
//  group's own worklist slice - declared, not defined) - copies the
//  three floats into a local CWorldVec3, and hands that, `this` and the
//  output pointer to 0x6F476860 (also outside this slice).  The caller's
//  own use of the result (creepcampfind.cpp: compared against a
//  config-derived squared-distance threshold) says 0x6F476860 computes a
//  squared distance and writes it through `out`, but that computation is
//  not in this function's own body to confirm from here, and neither
//  out-of-slice callee has a dump - both are declared with a best-effort
//  argument shape read off this call site's own pushes, not a confirmed
//  signature.
//============================================================================
#include "game.h"

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig             \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  Out of this group's own worklist slice - naked redirects to the real,
//  unhooked shipped bodies rather than reconstructions of their own; the
//  argument shape is a best-effort read off this call site's own pushes,
//  not a confirmed signature.
ADDR_THUNK(CWorldVec3* __fastcall ResolveFixedWorldPoint(void* self, CWorldVec3* out), 0x6F474680)
ADDR_THUNK(void* __fastcall FinishCreepSpotDistance(void* self, void* out,
                                         const CWorldVec3* vec3, int flag), 0x6F476860)

void* __fastcall ComputeCreepSpotDistance(void* self, void* out, void* other,
                                          int useSmartPtr)
{
    CWorldVec3 point;

    if (useSmartPtr)
        point = ((const CPathRef*)other)->SmartPtrToWorldVec3();
    else
        point = *ResolveFixedWorldPoint(other, &point);

    return FinishCreepSpotDistance(self, out, &point, useSmartPtr);
}
