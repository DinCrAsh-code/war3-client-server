//============================================================================
//  The engine's own global handle table (`g_pHandleTable`, dword_6FAB7788) -
//  an STwoIndexedFreeLists (Math/indexedfreelist.h) plus two more monotonic
//  "next never-recycled id" counters appended right after it.
//
//  IMPORTANT, and the reason this header exists rather than folding these
//  three functions into the checksum-provider-registry investigation's own
//  files: `g_pHandleTable` (0x6FAB7788) is a *different* global from the
//  "tempest presence" host `g_unk6FAB778C` (0x6FAB778C,
//  Net/tempestproviderlisthash.cpp / Net/netcommand_dispatch.cpp) that
//  CNetData::FireTickNotification's own already-reconstructed source reads
//  `presenceTag` from (`*(unsigned int*)((char*)g_unk6FAB778C + 0x50)`).
//  The two globals are four bytes apart and share the identical
//  "STwoIndexedFreeLists + two counters" shape - which is what makes them
//  easy to confuse from a live debugger's own symbol view - but they are
//  two separate heap objects: nothing in the entire agent_worktrees dump
//  (raw disassembly for essentially every function in the binary) ever
//  writes the literal address 0x6FAB778C from any of the functions below,
//  and nothing ever writes 0x6FAB7788 from CNetData::FireTickNotification/
//  TempestProviderListHash/ChecksumProviderDebugPrint's own call tree.
//
//  See docs/notes/checksum-provider-registry.md's own "LIVE FINDING:
//  presenceTag is a slot-allocation counter" section for the correction
//  this establishes: the live breakpoint that resolved to
//  sub_6F4AEFF0/0x6F4AF07F was almost certainly on this table
//  (g_pHandleTable, +0x50 is its own "next positive id" counter, exactly
//  the shape the operator's note describes) rather than on the tempest
//  presence host the rest of that investigation was chasing - the two
//  addresses differ by one hex digit and the raw byte encoding of every
//  function here is unambiguous (`8b 0d 88 77 ab 6f`, not `...8c...`).
//
//  A SECOND, LATER live-debugger read (2026-09-07) settled what the
//  tempest presence host (`g_unk6FAB778C`) actually *is*, by RTTI: a real
//  `NTempest::CDynTable<CPrRgEntry>` (Math/indexedfreelist.h's own
//  correction note, Pathfinding/gridregistrationteardown.cpp). That finding
//  is about `g_unk6FAB778C`, not this table - nothing here changes.
//  `g_pHandleTable` is built on the identical STwoIndexedFreeLists shape
//  (see that struct's own name and this file's opening comment) but has
//  not itself been shown by any live read to be a `CDynTable` instantiation
//  or any other named RTTI class; it stays the generic, untyped handle
//  table it already was. Do not fold the two globals' identities together
//  just because one of them now has a confirmed class.
//============================================================================
#ifndef HANDLETABLE_H
#define HANDLETABLE_H

#include "indexedfreelist.h"

//  A record describing how to (re)register an SHandleTableEntry: either a
//  fresh registration (m_priorSlot == -1) or a request to reuse two
//  already-known slot values (m_priorSlot/m_secondSlot both set), used when
//  an entry that already has a table slot is being re-registered rather
//  than created for the first time.  -2 in m_priorSlot is a third case: "go
//  through the ordinary allocation path but always the positive pool" (see
//  RegisterOrReuse's own comment).
struct SHandleRegistrationRequest
{
    char m_reserved00[0x24];
    int  m_priorSlot;    // +0x24 - -1: none yet: -2: allocate now (positive pool);
                         //         else: an already-known slot to reuse
    int  m_secondSlot;   // +0x28 - -1: not yet known; else: an already-known
                         //         second slot to reuse
};

//  The registration record every handle-table entry embeds: a copy of the
//  slot handle AllocateAt returned (top bit = which of the two pools) and,
//  separately, a freshly minted "never recycled" id from the table's own
//  trailing counters.  Only the two touched fields are named; every
//  concrete class that embeds this (there is at least one, reached through
//  its own vtable slot 1 by sub_6F4AEF00 below) puts them at a fixed
//  offset, matching CGridRegistration's own unrelated +0x14/+0x18 pair in
//  spatialgrid.h (a coincidence of two engine subsystems both reusing the
//  same idiom, not the same object - see that header's own note).
struct SHandleTableEntry
{
    //  0x6F4AEFC0 - mint a fresh, never-recycled id into `m_slotIndex` from
    //  whichever of g_pHandleTable's own two counters `m_slotHandle`'s own
    //  top bit selects (set: the negative pool's counter at +0x54; clear:
    //  the positive pool's at +0x50), post-incrementing it.  `retn 0` (no
    //  stack arguments).
    void AssignFreshHandleId();

    //  0x6F4AEFF0 - register (or re-register) this entry into
    //  g_pHandleTable.  See handletableregister.cpp for the three shapes
    //  `request` (or its absence) selects between.  `retn 4`.
    void RegisterOrReuse(SHandleRegistrationRequest* request);

    //  0x6F4AEF00 - release this entry's own table slot (if it has one),
    //  reset both fields to "unregistered", then dispatch the object's own
    //  vtable slot 1 with no arguments.  `arg` is never read anywhere in
    //  this function's own body - confirmed dead by its dump - so `retn 4`
    //  is cleaning up a stack argument this function itself never uses,
    //  not forwarding it anywhere.  See handletableregister.cpp.
    void UnregisterAndNotify(int arg);

    char         m_reserved00[0x14];
    unsigned int m_slotHandle;   // +0x14 - AllocateAt's own return value
    unsigned int m_slotIndex;    // +0x18 - a freshly minted id, or a copy
                                 //         of m_slotHandle (see RegisterOrReuse)
};

//  g_pHandleTable itself: an STwoIndexedFreeLists plus two more "next
//  never-recycled id" counters right after it.
struct SHandleTable : STwoIndexedFreeLists
{
    int m_nextPositiveId;   // +0x50
    int m_nextNegativeId;   // +0x54
};

extern SHandleTable* g_pHandleTable;

#endif
