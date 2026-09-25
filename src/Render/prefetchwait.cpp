//============================================================================
//  0x6F4BC650 - PrefetchNode::WaitReady: block until the load behind this
//  node has finished.
//
//  Two instructions, `add ecx,20h` / `jmp`, which is the member tail call
//  docs/msvc-vc8-idioms.md records under "`add ecx, <offset>` / `jmp` is a
//  member tail call, not an MI thunk": the offset is the embedded event's,
//  and the callee's definition is in another translation unit (here, a
//  redirect in renderthunks.cpp, because IDA gives 0x6F6D7E90 no function of
//  its own).  Writing the WaitForSingleObject out inline instead scores
//  better and is the wrong shape - it loses the `add` and reads the handle
//  at +0x30 off the node rather than at +0x10 off the event.
//============================================================================
#include "filecache.h"

void PrefetchNode::WaitReady()
{
    m_loaded.Wait();
}
