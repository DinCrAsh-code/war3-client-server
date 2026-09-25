//============================================================================
//  Three `.\Jass.cpp` TSHashTable constructors, redirected rather than
//  reconstructed - the exact same situation Containers/hashtablectors.cpp
//  documents for the font cache and the command registries, just a
//  different module's instantiations:
//
//      0x6F44F990  TSHashTable<NativeFunc,  HASHKEY_STR>       - JassThreadLocal's native table (+0x18)
//      0x6F4528D0  TSHashTable<Script2HandleReg, HASHKEY_NONE> - JassThreadLocal's script->handle table (+0x60)
//      0x6F450B80  TSHashTable<JassHandle,  HASHKEY_CONSTSTR>  - the JASS handle registry
//
//  Each stamps its own `??_7?$TSHashTable@...@@6B@`, but `ida_query vtable`
//  precedent set by Containers/hashtablectors.cpp holds here too: the
//  shipped tables carry four vtable slots where this repo's own TSHashTable
//  template (Storm/storm.h) declares only two virtuals (the destructor and
//  AllocNode, both declared and never defined for the same reason storm.h's
//  own comment gives).  Constructing one for real would stamp a two-slot
//  vtable over a four-slot shipped class - a wrong-length vtable no
//  instruction score can see and that dispatches the wrong function at or
//  past the mistake, exactly what tools/check_vtables.py exists to catch.
//  A redirect leaves the shipped constructor, and therefore the shipped
//  vtable, in place.
//============================================================================
#ifndef JASSHASHTABLECTORS_H
#define JASSHASHTABLECTORS_H

//  0x28 bytes: vtable pointer, the all-nodes TSExplicitList, the probe
//  count, the three words of the bucket array and the mask - the same
//  storage every TSHashTable<T,K> instantiation carries (Storm/storm.h).
struct SJassNativeFuncTable
{
    //  0x6F44F990 - `retn 0`.  Redirect; see file header.
    SJassNativeFuncTable* Construct();
    char m_storage[0x28];
};

struct SJassScript2HandleTable
{
    //  0x6F4528D0 - `retn 0`.  Redirect; see file header.
    SJassScript2HandleTable* Construct();
    char m_storage[0x28];
};

struct SJassHandleTable
{
    //  0x6F450B80 - `retn 0`.  Redirect; see file header.
    SJassHandleTable* Construct();
    //  0x6F454050 - vtable slot 2 (agent_worktrees' own RTTI name for this
    //  vtable, "TAllocatorHashTable<JassHandle,HASHKEY_CONSTSTR,128>", is
    //  IDA's fixed demangler reading the same class this Construct()
    //  redirects into).  Same `ScalarDeletingDestructor` shape as
    //  Pathfinding/pathclient_scalardtor.cpp; kept a naked thunk of its
    //  own because the real destructor (0x6F454050's own callee,
    //  0x6F450DA0, outside this session's range) tail-jumps into
    //  `__CxxFrameHandler3` - see jasshashtablectors2.h's own note.
    void* ScalarDeletingDestructor(unsigned int flags);
    char m_storage[0x28];
};

#endif
