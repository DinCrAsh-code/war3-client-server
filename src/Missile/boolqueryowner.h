//============================================================================
//  0x6F47A880 / 0x6F47A540 - the two-step "is there a live thing here"
//  query CMissileThunderBoltRefs::DealDamage (missile_dealdamage.cpp) runs
//  on the missile's own +0x78 sub-object.
//
//  Both are members of the *same* object: 0x6F47A880's first instruction is
//  a bare `call 0x6F47A540` with ecx untouched from its own entry, so the
//  two share a `this`.
//
//  Two translation units, because the first calls the second for real -
//  boolquery.cpp and boolqueryresolve.cpp.  The header is what keeps the
//  one class declaration in one place; MSVC puts the class name in the
//  mangled name, so a second copy that drifts is an LNK2019 only the gate
//  would see.
//============================================================================
#ifndef BOOLQUERYOWNER_H
#define BOOLQUERYOWNER_H

struct SBoolQueryOwner
{
    //  0x6F47A880 - `retn 0`: no arguments, the object in ecx.
    int   __thiscall QueryBool();
    //  0x6F47A540 - the same.  Resolves the {handle, typeTag} pair at
    //  +0x08/+0x0C and hands back the first entry of the resolved object's
    //  own list, or null when that list is empty.
    void* __thiscall ResolveActiveEntry();

    char         m_reserved00[8];
    unsigned int m_handle;      // +0x08
    int          m_typeTag;     // +0x0C
};

#endif
