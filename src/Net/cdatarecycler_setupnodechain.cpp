//============================================================================
//  0x6F4C6740 - CDataRecycler::SetupNodeChain (agent-networking-
//  wave4-2026-08-29). Own translation unit, same reasoning as every other
//  split-out callee in this codebase (CLAUDE.md: "put a callee in its own
//  TU rather than reaching for __declspec(noinline)"): ReleaseChunk's own
//  call site (cdatarecycler_slots.cpp) folded this tiny function's whole
//  body into itself and tail-merged its own inner PushOntoStack call
//  straight into the caller when both lived in the same file, which is
//  not what the shipped image does (a real out-of-line `call`).
//============================================================================
#include "cdatarecycler.h"

void __stdcall PushOntoStack(void** headPtr, void* node, int linkOffset);

//----------------------------------------------------------------------------
//  Refill the free-link-node stack (m_field14) from one fresh SMemAlloc'd
//  block, chaining `m_count` 0xC-byte nodes starting at `block + 4` into a
//  forward singly-linked list, then bulk-pushing that whole chain onto
//  `*poolHead` in one PushOntoStack call whose own `linkOffset` argument
//  targets the LAST node's own next-slot rather than the first's
//  (`(m_count - 1) * 0xC` bytes past `block + 4`) - the same primitive
//  ReleaseChunk uses for a single node, reused here to splice an entire
//  prebuilt chain onto the stack at once.  `block + 0` itself is never
//  touched here; it is what Purge's own m_field0C loop frees later.
//----------------------------------------------------------------------------
void CDataRecycler::SetupNodeChain(void* block, void** poolHead)
{
    char* node = (char*)block + 4;
    if (m_count - 1 != 0)
    {
        unsigned int built = 0;
        do
        {
            *(char**)node = node + 0xC;
            node += 0xC;
            built += 1;
        } while (built < m_count - 1);
    }
    unsigned int lastOffset = m_count * 0xC - 0xC;
    PushOntoStack(poolHead, (char*)block + 4, (int)lastOffset);
}
