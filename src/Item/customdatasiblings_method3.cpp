//============================================================================
//  All six siblings' own vtable slot 3 override - a naked self-redirect,
//  same shape and same reason as CCustomData::Construct/
//  CAbilityCustomData::Construct (abilitycustomdata_ctor.cpp): the real
//  body at each address below lazily fetches two further SEH-carrying
//  singletons of its own (a per-class "DB" then "MetaDB", each behind its
//  own `__except_handler4`-shaped frame this toolchain cannot reproduce -
//  confirmed for all six this session, see customdatasiblings.h's own
//  header), then tail-jumps into a shared cleanup helper.  Nothing here is
//  reproducible from source, so each is filed as a self-redirect to its
//  own real address rather than an empty stub - CLAUDE.md's own
//  "SELF-REDIRECT THUNK" shape, which `hookset.py` already knows to treat
//  as unhookable.  checksum-batch-B (2026-09-12).
//============================================================================
#include "customdatasiblings.h"

#define SELF_THUNK(addr)         \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

//  0x6F251B70.
__declspec(naked) void CUnitCustomData::Method3()
    SELF_THUNK(06F251B70h)

//  0x6F251B90.
__declspec(naked) void CItemCustomData::Method3()
    SELF_THUNK(06F251B90h)

//  0x6F251C10.
__declspec(naked) void CDestructableCustomData::Method3()
    SELF_THUNK(06F251C10h)

//  0x6F251C30.
__declspec(naked) void CDoodadCustomData::Method3()
    SELF_THUNK(06F251C30h)

//  0x6F251BD0.
__declspec(naked) void CBuffCustomData::Method3()
    SELF_THUNK(06F251BD0h)

//  0x6F251BF0.
__declspec(naked) void CUpgradeCustomData::Method3()
    SELF_THUNK(06F251BF0h)

#undef SELF_THUNK
