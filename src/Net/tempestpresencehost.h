//============================================================================
//  Shared declarations for the "8 unreconstructed g_unk6FAB778C-touching
//  functions" investigation (docs/notes/checksum-provider-registry.md /
//  docs/notes/tempest-presence-host-8fn-session.md): the presence host's own
//  (re)construction (tempestpresencehostctor.cpp), teardown
//  (tempestpresencehost_teardown.cpp), broadcast
//  (tempestpresencehost_broadcast.cpp), plain accessors
//  (tempestpresencehost_getters.cpp), and its own save/verify-load pair
//  (tempestpresencehost_save.cpp / tempestpresencehost_load.cpp).
//
//  g_unk6FAB778C itself is declared exactly the same way every existing
//  user of it does (Net/tempestproviderlisthash.cpp,
//  Pathfinding/gridregistrationteardown.cpp): opaque `void*`, not the more
//  specific STwoIndexedFreeLists*/CDynTable<CPrRgEntry>* either of those
//  files would prefer, because tools/link_check.py's own "SPLIT DATA
//  GLOBAL" check requires every translation unit that touches a
//  funcmap.DATA global to agree on its mangled type.
//============================================================================
#ifndef TEMPESTPRESENCEHOST_H
#define TEMPESTPRESENCEHOST_H

#include "storm.h"

extern void* g_unk6FAB778C;

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

#endif  // TEMPESTPRESENCEHOST_H
