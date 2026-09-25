//============================================================================
//  0x6F47A880 - SBoolQueryOwner::QueryBool.  A naked redirect in
//  missile_dealdamage.cpp until now.
//
//  Three calls' worth of work in seven instructions: resolve this object's
//  own active entry (boolqueryresolve.cpp), resolve *that* entry's
//  {handle, typeTag} pair at its +0x68/+0x6C
//  (SHandleRefAt0x68Owner::ResolveHandle, misc_handle_resolvers.cpp), and
//  answer whether anything came back.
//
//  The `neg` / `sbb` / `neg` triple at the tail is this compiler's
//  normalise-to-0-or-1, i.e. a `!= 0` in the return expression rather than
//  an `if`.
//
//  Own translation unit: it calls ResolveActiveEntry for real, and
//  DealDamage calls it for real.
//============================================================================
#include "boolqueryowner.h"

//  misc_handle_resolvers.cpp's, re-declared exactly - same class name, same
//  signature, so the two mangle alike.
struct SHandleRefAt0x68Owner { void* __thiscall ResolveHandle(); };

int __thiscall SBoolQueryOwner::QueryBool()
{
    return ((SHandleRefAt0x68Owner*)ResolveActiveEntry())->ResolveHandle() != 0;
}
