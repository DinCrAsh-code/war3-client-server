//============================================================================
//  CItem-vtable-closure batch M: a UI-frame "shortcut key registration"
//  cluster - CLayer and CLayoutFrame destructors plus the
//  TSHashTable<SHORTCUTREGNODE,HASHKEY_NONE> table and several
//  TSExplicitList-shaped member-list teardown helpers they call.  The
//  evidence is each destructor's own vftable stamp
//  (`??_7CLayer@@6B@` / `??_7CLayoutFrame@@6B@`) and the constructor's own
//  `??_7?$TSHashTable@USHORTCUTREGNODE@@VHASHKEY_NONE@@@@6B@` stamp - none
//  of it CItem/CSelectable positioning or selection logic, reached only
//  because this BFS frontier walks every callee a slot 32/104 dispatch can
//  indirectly reach through a shared UI-frame teardown path.
//
//  Every destructor and constructor here is left a redirect for the same
//  reason Containers/hashtablectors.cpp's own four constructors are:
//  Storm/storm.h's TSHashTable<T,K> declares two virtuals it never defines,
//  so stamping the template concrete here would stamp a vtable of this
//  build's own making over the shipped table's real one.  CLayer and
//  CLayoutFrame are not modelled classes anywhere in this repo yet, so
//  their own destructors carry the same risk one level up: a `??_7CLayer@
//  @6B@` stamp this build emits with no vtable definition here to back it
//  would be exactly ctor_vtable_audit.py's own crash class.
//
//  Same 0% instruction match by construction as every other redirect at
//  this depth - see each BEHAVIOUR entry in tools/funcmap.py.
//============================================================================

#define FRAMELAYER_THUNK(sig, addr)    \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//----------------------------------------------------------------------------
//  0x6F600CF0 - CLayer::~CLayer: releases a `SHORTCUTREGNODE` table sub-
//  object (+0x80, via sub_6F600610 below), two TSExplicitList-shaped member
//  lists (+0x74/+0x4C, via ReleaseShortcutRegNodeArrayA/CLayer_ReleaseField4C
//  below), two TRefCnt-shaped fields (+0x38/+0x20, inline release-if-last-
//  ref) and a third list (+0x14, via CLayer_ReleaseField14 below), then
//  forwards to a base-class destructor (sub_6F62A9D0).  SEH-framed for the
//  same reason every /GS-cookied dtor in this repo is.  `retn 0`.
//----------------------------------------------------------------------------
FRAMELAYER_THUNK(void __cdecl CLayer_Destructor(), 0x6F600CF0)

//----------------------------------------------------------------------------
//  0x6F5FD3B0 - releases two adjacent TSExplicitList-shaped sub-objects
//  (+0x0C and +0x04 of the object CLayer::~CLayer passes it, i.e. the
//  layer's own +0x4C, +0x54) - the inlined TSLink::Unlink-shaped single-node
//  detach, run once per list rather than drained in a loop (so: unlink at
//  most one lingering node from each, not a general teardown).  `retn 0`.
//----------------------------------------------------------------------------
FRAMELAYER_THUNK(void __cdecl CLayer_ReleaseField4C(), 0x6F5FD3B0)

//----------------------------------------------------------------------------
//  0x6F5FD560 - drains a `TSGrowableArray<TSExplicitList<T> >`-shaped member
//  array of lists (indexed off +0x08's own count), unlinking one node per
//  entry, looping until the count field itself is exhausted.  Called from
//  CLayer::~CLayer on the layer's own +0x74.  `retn 0`.
//----------------------------------------------------------------------------
FRAMELAYER_THUNK(void __cdecl ReleaseShortcutRegNodeArrayA(), 0x6F5FD560)

//----------------------------------------------------------------------------
//  0x6F5FD5B0 - the identical drain shape as ReleaseShortcutRegNodeArrayA
//  above, over a different member array.  Called from CLayer::~CLayer on
//  the layer's own +0x14.  `retn 0`.
//----------------------------------------------------------------------------
FRAMELAYER_THUNK(void __cdecl CLayer_ReleaseField14(), 0x6F5FD5B0)

//----------------------------------------------------------------------------
//  0x6F600610 - TSHashTable<SHORTCUTREGNODE,HASHKEY_NONE>::<constructor>:
//  stamps `??_7?$TSHashTable@USHORTCUTREGNODE@@VHASHKEY_NONE@@@@6B@`,
//  releases the table's own +0x14 member list through sub_6F5FF380 (out of
//  this batch's scope) and Initialize()s the table via sub_6F5FD600 (out of
//  this batch's scope) - SEH-framed.  Called from CLayer::~CLayer on the
//  layer's own +0x80.  `retn 0`.
//----------------------------------------------------------------------------
FRAMELAYER_THUNK(void __cdecl ConstructShortcutRegTable(), 0x6F600610)

//----------------------------------------------------------------------------
//  0x6F6064D0 - CLayoutFrame::~CLayoutFrame: releases a
//  TSHashTable<SHORTCUTREGNODE,HASHKEY_NONE>-shaped member (+0x38, via
//  ReleaseShortcutRegNodeArrayC below) and a second TSExplicitList-shaped
//  member (+0x30, inlined single-node unlink), SEH-framed.  `retn 0`.
//----------------------------------------------------------------------------
FRAMELAYER_THUNK(void __cdecl CLayoutFrame_Destructor(), 0x6F6064D0)

//----------------------------------------------------------------------------
//  0x6F6060C0 - the same drain shape as ReleaseShortcutRegNodeArrayA/B
//  above, over CLayoutFrame's own +0x38 sub-array.  Called from both
//  CLayoutFrame::~CLayoutFrame and (through sub_6F606490, out of this
//  batch's scope) CLayoutFrame::ReleaseShortcutBindings below.  `retn 0`.
//----------------------------------------------------------------------------
FRAMELAYER_THUNK(void __cdecl ReleaseShortcutRegNodeArrayC(), 0x6F6060C0)

//----------------------------------------------------------------------------
//  0x6F606290 - CLayoutFrame::ReleaseShortcutBindings: when the object's own
//  +0x64 flag is set, forwards to sub_6F604FC0(0) first; walks a probe list
//  off +0x40 calling sub_6F606210(this, 1) per entry through sub_6F606160's
//  own lookup; then, while +0x40 is still set, calls its own vtable slot 0
//  and hands the record to sub_6F4C1B50(&unk_6FACE740, record, 0, 0) each
//  time round - a release-and-unregister loop, not a simple destructor.
//  `retn 0`.
//----------------------------------------------------------------------------
FRAMELAYER_THUNK(void __cdecl CLayoutFrame_ReleaseShortcutBindings(), 0x6F606290)

//----------------------------------------------------------------------------
//  0x6F607710 / 0x6F607750 - two more instances of the same
//  TSGrowableArray<TSExplicitList<T> >-shaped drain shape as
//  ReleaseShortcutRegNodeArrayA/B/C above, over member arrays this batch's
//  dump does not root a caller for (siblings reached only by address
//  proximity, not by a call site in this closure).  Both `retn 0`.
//----------------------------------------------------------------------------
FRAMELAYER_THUNK(void __cdecl ReleaseShortcutRegNodeArrayD(), 0x6F607710)
FRAMELAYER_THUNK(void __cdecl ReleaseShortcutRegNodeArrayE(), 0x6F607750)

#undef FRAMELAYER_THUNK
