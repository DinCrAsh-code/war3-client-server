//============================================================================
//  The JASS interpreter's own global call-frame pool (g_jassFramePool,
//  unk_6FAB728C, jassframepool.h) and one frame's own shape - discovered
//  from JassInstance's own callers into it (0x6F45F8B0/0x6F45FA30, still
//  TODO - not attempted this session) which acquire/release a frame once
//  per nested JASS call.  This is the concrete answer to "where does the
//  VM keep its call frames": not a plain contiguous stack, but a
//  free-list pool of reusable "AUFrame"-tagged blocks, each holding a
//  fixed-size local-variable slot array and its own embedded
//  SScriptDataTable.
//============================================================================
#ifndef JASSSCRIPTFRAMEPOOL_H
#define JASSSCRIPTFRAMEPOOL_H

#include "jasshashtablectors2.h"   // SScriptDataTable::Construct - 0x6F450450
#include "jassframepool.h"  // JassFramePool/g_jassFramePool - jassmoduleteardown.cpp
#include "storm.h"         // TSLink<T> - 0x6F45ACE0's own tail is its Unlink()

//  ??_7ScriptDataTable@@6B@ @ 0x6F950E30 - resolved via
//  `worktree_store.py resolve`.  AcquireScriptFrame's own dump stamps
//  this explicitly right after calling SScriptDataTable::Construct().
const unsigned int kScriptDataTableVftable = 0x6F950E30u;

//  One frame as AcquireScriptFrame/ReleaseScriptFrameContents see it.
//  AcquireScriptFrame only ever writes +0x00/+0x04/+0x08 (the last a
//  constant 0x20) and placement-constructs +0x90; ReleaseScriptFrameContents
//  reads +0x00/+0x04/+0x08 back, a 0x20-entry pointer array starting at
//  +0x0C (`[ebp+0]`, `ebp` incrementing by 4 each of the 0x20 iterations,
//  so the array is inline, not indirected through a separate data
//  pointer) and the same +0x90 table - so both functions agree on this
//  shape.  +0x00/+0x04 double as the pool's own TSLink<T> fields
//  (m_frameListHead threads idle frames through them) while a frame is
//  parked in the pool, and as real content once it is handed out; nothing
//  in this closure reaches between the array (ending at 0xC+0x20*4=0x8C)
//  and the table (starting at 0x90), so those 4 bytes stay reserved.
struct JassScriptFrame : public TSLink<JassScriptFrame>
{
    unsigned int     m_localCount;      // +0x08 - always 0x20 when freshly
                                         //   acquired; ExecuteOpcodeStream's
                                         //   own closure (still TODO) is
                                         //   presumably what can shrink it
    void*            m_locals[0x20];    // +0x0C - fixed local-slot array
    char              m_reserved8C[0x90 - 0x8C];
    SScriptDataTable  m_scriptData;     // +0x90
};

//  0x6F45C0D0 - see jassframepool.h's own JassFramePool::AcquireFrame.
//  Pop a frame off g_jassFramePool's own m_frameListHead LIFO if one is
//  sitting idle (the same singly-linked stack jassmoduleteardown.cpp's
//  JassFrameNode::Unlink walks from the teardown side); otherwise fall
//  through to the pool's CDataAllocator half for a fresh "AUFrame"-tagged
//  block and placement-construct a fresh SScriptDataTable at +0x90 into
//  it.  Returns a pointer 4 bytes into the block (past its own
//  intrusive-list link word) - a JassScriptFrame*, reinterpreted as
//  `void*` because nothing in this closure's own callers
//  (0x6F45F8B0/0x6F45FA30, still TODO) is reached to confirm the cast is
//  safe there too.

//  0x6F45A040 - kept THUNK: real, unhooked shipped code.  A per-slot
//  teardown callback for one entry of the local array
//  ReleaseScriptFrameContents below walks (conditionally fires a virtual
//  "release" through the slot's own +0x20 field - a different +0x20 than
//  JassScriptFrame's own m_localCount above; this is the callback's own
//  parameter's layout, an entirely different object - when a tag at
//  +0x18 reads >= 9, then tail-calls sub_6F44B960) - genuinely small, but
//  opens with the same push-0FFFFFFFFh/push-offset-SEH_.../mov-eax,
//  large-fs:0 `_except_handler4` prologue and closes through
//  `__CxxFrameHandler3` that docs/targets/JassVM.md's own flagship-
//  function note and this fixed `/GS- /EHs-c-` toolchain already
//  document as unreproducible from C++ source - see pipeline/CLAUDE.md's
//  SEH note next to JassInstance::Construct/Run.
//  ReleaseScriptFrameContents below calls it once per populated slot
//  exactly the way the shipped loop does.
void __fastcall ReleaseLocalSlot(void* slot);

//  0x6F45ACE0 - `retn 0`.  Tear down one JassScriptFrame's own contents:
//  destroy its embedded SScriptDataTable (sub_6F44CFC0, still TODO - not
//  attempted this session, forward-declared below purely to keep this
//  call site's own argument shape faithful), release every one of its
//  0x20 local slots back to g_jassScriptDataPool (unk_6FAB7278,
//  jassmoduleteardown.cpp) through CDataAllocator::Free (0x6F4C1B50,
//  dataallocator.h) after running each through ReleaseLocalSlot() above,
//  then unlink `frame` itself off whatever list it is presently threaded
//  into via its own embedded TSLink<T> (storm.h) - the function's own
//  tail is TSLink<T>::Unlink()'s body, confirmed instruction-for-
//  instruction against storm.h's already reconstructed generic version.
//  Does not itself return `frame` to g_jassFramePool - nothing in this
//  function's own body touches m_frameListHead - so whatever calls this
//  is presumably what does that next, outside this closure.
void __fastcall ReleaseScriptFrameContents(JassScriptFrame* frame);

#endif
