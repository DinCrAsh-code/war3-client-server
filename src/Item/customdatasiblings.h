//============================================================================
//  CUnitCustomData / CItemCustomData / CUpgradeCustomData /
//  CDestructableCustomData / CDoodadCustomData / CBuffCustomData - the six
//  other per-content-type override tables in the checksum-provider
//  registry's `CCustomData` family (docs/notes/checksum-provider-registry.md),
//  siblings of `CAbilityCustomData` (Item/abilitycustomdata.h).  All seven
//  share the identical four-slot vtable shape (slot 0/1 inherited from
//  `CCustomData`/`CSynchronousData`, slot 2 = DeleteSelf, slot 3 = a
//  per-class override) and the identical `CCustomData::Construct(int)`
//  dispatch, one integer type tag per sibling: Unit=0, Item=1,
//  Destructable=2, Doodad=3, Ability=4, Buff=5, Upgrade=6 - found by
//  reading every real caller of `CCustomData::Construct`
//  (`tools/worktree_store.py xrefs 0x6F2518A0 --to`), which turned up
//  exactly six more small `Construct()` redirects alongside
//  `CAbilityCustomData::Construct` (abilitycustomdata_ctor.cpp) itself, at
//  0x6F2564C0/F0/0x256550/80/0x2565B0/E0.  Each one's own vtable stamp
//  (`??_7<Class>@@6B@`) confirmed the class name directly out of the dump,
//  and `agent_worktrees/classes/<vtable-addr>.json`'s own Class Informer
//  RTTI (`hierarchy_raw: "<Class>: CCustomData, CSynchronousData;"`)
//  confirms the inheritance chain for all six.  checksum-batch-B
//  (2026-09-12).
//
//  **Construct() stays a naked redirect for all six, same reason as
//  CCustomData::Construct/CAbilityCustomData::Construct** (see
//  abilitycustomdata.h's own note): each one's only real work is calling
//  the SEH-carrying `CCustomData::Construct(int)`, which this toolchain
//  cannot reproduce.
//
//  **Slot 3 (`Method3` override) is a real, confirmed self-redirect thunk
//  for all six - not an empty stub.**  Every one of these six overrides
//  (customdatasiblings_method3.cpp) has the *identical* shape: read a
//  field at `this+0x08`, lazily fetch a per-class "DB" singleton through
//  one `sub_6F71Bxxx`/`sub_6F71Cxxx` helper, thread the result through the
//  shared `sub_6F6F4B50` one-line store, lazily fetch a second per-class
//  "MetaDB" singleton through another `sub_6F71Cxxx` helper, then
//  tail-jump into the shared `sub_6F7057A0` (clears two dynamic arrays at
//  `this+0x1CC`/`this+0x1F4`).  Both per-class fetch helpers, for all six
//  classes checked, carry the identical `__except_handler4`-shaped SEH
//  frame (`push 0FFFFFFFFh` / `push offset SEH_<addr>` / `mov eax, large
//  fs:0` / the `dword_6FAAE140` cookie XOR) docs/msvc-vc8-idioms.md already
//  documents as unreproducible with this toolchain's fixed `/GS- /EHs-c-`
//  (the same shape `CAbilityCustomData`'s own still-open slot 3
//  (`sub_6F251BB0`) has, per this batch's own seed note) - confirming the
//  seed's own "if you confirm this, it's legitimately a THUNK" call for
//  all six, not just a guess extended from one.  `CAbilityCustomData`'s own
//  slot 3 stays out of scope for this batch (ten functions deep reaching
//  two more not-yet-reconstructed classes, per abilitycustomdata.h's own
//  note) - only these six, structurally identical but each already fully
//  read this session, are thunked here.
//============================================================================
#ifndef CUSTOMDATASIBLINGS_H
#define CUSTOMDATASIBLINGS_H

#include "abilitycustomdata.h"

//  0x6F92CBDC, type tag 0.  Ctor 0x6F2564C0, DeleteSelf 0x6F25B3F0,
//  Method3 0x6F251B70.
class CUnitCustomData : public CCustomData
{
public:
    CUnitCustomData* Construct();
    virtual CUnitCustomData* DeleteSelf(unsigned int flags);
    virtual void Method3();
};

//  0x6F92CBF0, type tag 1.  Ctor 0x6F2564F0, DeleteSelf 0x6F25B4B0,
//  Method3 0x6F251B90.
class CItemCustomData : public CCustomData
{
public:
    CItemCustomData* Construct();
    virtual CItemCustomData* DeleteSelf(unsigned int flags);
    virtual void Method3();
};

//  0x6F92CC40, type tag 2.  Ctor 0x6F2565B0, DeleteSelf 0x6F25B7B0,
//  Method3 0x6F251C10.
class CDestructableCustomData : public CCustomData
{
public:
    CDestructableCustomData* Construct();
    virtual CDestructableCustomData* DeleteSelf(unsigned int flags);
    virtual void Method3();
};

//  0x6F92CC54, type tag 3.  Ctor 0x6F2565E0, DeleteSelf 0x6F25B870,
//  Method3 0x6F251C30.
class CDoodadCustomData : public CCustomData
{
public:
    CDoodadCustomData* Construct();
    virtual CDoodadCustomData* DeleteSelf(unsigned int flags);
    virtual void Method3();
};

//  0x6F92CC18, type tag 5.  Ctor 0x6F256550, DeleteSelf 0x6F25B630,
//  Method3 0x6F251BD0.
class CBuffCustomData : public CCustomData
{
public:
    CBuffCustomData* Construct();
    virtual CBuffCustomData* DeleteSelf(unsigned int flags);
    virtual void Method3();
};

//  0x6F92CC2C, type tag 6.  Ctor 0x6F256580, DeleteSelf 0x6F25B6F0,
//  Method3 0x6F251BF0.
class CUpgradeCustomData : public CCustomData
{
public:
    CUpgradeCustomData* Construct();
    virtual CUpgradeCustomData* DeleteSelf(unsigned int flags);
    virtual void Method3();
};

#endif
