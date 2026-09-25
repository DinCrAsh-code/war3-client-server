//============================================================================
//  0x6F4A4310 - NIpse::SRegistryRoot::LinkNodeIntoRegistry.  Own translation
//  unit - both callers (rlagentdef_apply.cpp's own Apply,
//  rlagentdef_load.cpp's own Load) keep this a real out-of-line `call` in
//  the dump, and inlining it into either cost that caller's own score
//  (CLAUDE.md's "put a callee in its own TU").
//
//  Splices `node` onto the front of the doubly-linked registry rooted at
//  `this`: `node`'s own +0x04/+0x08 pair becomes {this, this's old first
//  node}, the old first node's own +0x04 is repointed at `node` (if there
//  was one), and `this`'s own +0x08 becomes `node`.  The dump re-reads
//  `this+0x08` a second time rather than caching the first read in a
//  register - transcribed as written, not "optimised" into one read.
//============================================================================
#include "rlagentdef.h"

namespace NIpse {

void SRegistryRoot::LinkNodeIntoRegistry(void* node)
{
    char* n = (char*)node;
    char* h = (char*)this;

    *(void**)(n + 4) = this;
    *(void**)(n + 8) = *(void**)(h + 8);

    void* oldFirst = *(void**)(h + 8);
    if (oldFirst != 0)
        *(void**)((char*)oldFirst + 4) = n;

    *(void**)(h + 8) = n;
}

}  // namespace NIpse
