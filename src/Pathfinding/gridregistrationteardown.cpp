//============================================================================
//  0x6F4A00B0 - CGridRegistration::TeardownRegistration.
//
//  Was a naked `mov eax,<addr>/jmp eax` redirect (Widget/pathref_dtor.cpp's
//  own note on it, from the ReleasePathRegistration target); real body now,
//  reached by pulling this address's own raw disassembly out of
//  agent_worktrees (no asm/ dump for it - see CLAUDE.md's "There is no live
//  IDA query tool any more").
//
//  This is the direct, concrete link between per-mover pathfinding
//  registration and the checksum-provider registry's own "tempest
//  presence" table: every CGridRegistration releases its own presence-slot
//  handle through the exact STwoIndexedFreeLists::Push
//  (Math/indexedfreelistpush.cpp) the checksum investigation already had in
//  hand, on the exact global (`g_unk6FAB778C` - now RTTI-confirmed to be a
//  live `NTempest::CDynTable<CPrRgEntry>`, see that global's own comment
//  below) CNetData::FireTickNotification reads `presenceTag` from - see
//  docs/notes/checksum-provider-registry.md.  The *allocation* counterpart
//  (whoever first sets m_presenceSlot on a fresh CGridRegistration) is
//  still not found; nothing in this call tree reaches it.
//
//  Own translation unit: three real calls out (ForEachCellInBox,
//  STwoIndexedFreeLists::Push, CancelPendingRequestIfIdle), none of them
//  small enough for this build's /Ob2 to want to inline regardless, but
//  kept isolated the same way every other member of this family is.
//============================================================================
#include "indexedfreelist.h"
#include "spatialgrid.h"

//  g_unk6FAB778C - the tempest presence host (Net/tempestproviderlisthash.cpp
//  / Net/netcommand_dispatch.cpp), which both already declare `extern
//  void*` - kept the same type here rather than the more specific
//  STwoIndexedFreeLists* this file would prefer, because MSVC mangles a
//  funcmap.DATA global's declared type into its own linker symbol
//  (tools/link_check.py's own "SPLIT DATA GLOBAL" check) - two different
//  types for the same extern would be two symbols the real link can never
//  resolve to one address.  *Not* the same global as
//  Pathfinding/handletable.h's g_pHandleTable (0x6FAB7788, four bytes
//  away) despite the identical STwoIndexedFreeLists shape; see that
//  header's own note.
//
//  CORRECTED IDENTITY (live-debugger RTTI, 2026-09-07): this object's real
//  type is `NTempest::CDynTable<CPrRgEntry>` (-> `CMemBlockT<CPrRgEntry>`
//  -> `CMemBlock` -> `CEntity`), confirmed by a breakpoint on
//  `STwoIndexedFreeLists::AllocateAt` (0x6F4BC310,
//  Math/indexedfreelistallocatedispatch.cpp) with `this` == the live
//  `g_unk6FAB778C` pointer, resolving its vtable's RTTI - the exact
//  live-debugger recipe docs/notes/checksum-provider-registry.md's own
//  "presencetag-slotalloc" batch laid out. A *prior* live-debugger read
//  this same day misattributed a related capture to `g_pHandleTable`
//  instead (see that global's own header, Pathfinding/handletable.h) -
//  this note corrects the class *identity* question that read left open,
//  not that same-day address mixup, which was already fixed. The `Push`
//  call below is unchanged: `STwoIndexedFreeLists` still models the exact
//  byte layout `CDynTable<CPrRgEntry>`'s own free-list storage uses (a
//  templated dynamic table built on a generic two-free-list allocator is
//  exactly this shape), so nothing about the reconstructed instructions
//  was wrong - only "what class actually owns this" was unknown before.
//  Which of `CDynTable`/`CMemBlockT`/`CMemBlock`/`CEntity` contributes
//  which byte offset is still not established beyond this layout; see
//  Math/indexedfreelist.h's own correction note.
extern void* g_unk6FAB778C;

//  0x6F49E850 - Pathfinding/pathclientidlecancel.cpp.  Not declared in any
//  shared header yet (nothing else in this repo has called it before this
//  file) - `int __stdcall`, one stack argument, exactly as that file's own
//  dump established.
int __stdcall CancelPendingRequestIfIdle(void* owner);

void CGridRegistration::TeardownRegistration()
{
    if (!(m_moveFlags & 0x10000000))
        m_grid->ForEachCellInBox(&m_box, this, 0);

    m_visitStamp = -1;
    ((STwoIndexedFreeLists*)g_unk6FAB778C)->Push((int)m_presenceSlot, (int)this);

    m_presenceSlot2 = 0xFFFFFFFFu;
    m_presenceSlot  = 0xFFFFFFFFu;

    CancelPendingRequestIfIdle(this);
}
