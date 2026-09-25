//============================================================================
//  0x6F4AEF40 - write one pathing-reference entry into a save stream, or a
//  {-1, -1} sentinel when the slot is empty.
//
//  Own translation unit: SPathRefArray::Save (pathref_save.cpp) calls it
//  once per element with a real call.
//
//  __fastcall with the entry in edx: the shipped body opens with
//  `test edx,edx` before touching the stack at all, and the store arrives
//  in ecx.  Both branches chain the two writes through the writer's own
//  `this` return (`mov ecx,eax`), which is what makes CDataStoreScratch's
//  fluent shape observable here rather than just plausible.
//============================================================================
#include "cdatastorescratch.h"

//  The entry's own two saved words.  Only these two offsets are reached.
struct SPathRefSaveEntry
{
    char         m_reserved00[0x14];
    unsigned int m_field14;    // +0x14
    unsigned int m_field18;    // +0x18
};

//  Declared with a `const void*` second parameter, matching its one caller
//  (pathref_save.cpp) exactly: MSVC mangles the parameter type into the
//  symbol, so a `const SPathRefSaveEntry*` here and a `const void*` there
//  are two different symbols that never meet - the LNK2019 class
//  docs/notes/verifier-gate-link-failures.md is about, and one that
//  verify.py cannot see because it never links.
void __fastcall WritePathRefEntry(CDataStoreScratch* store, const void* raw)
{
    const SPathRefSaveEntry* entry = (const SPathRefSaveEntry*)raw;
    if (entry == 0)
    {
        store->WriteDword((unsigned int)-1)->WriteDword((unsigned int)-1);
        return;
    }
    //  Both fields into locals before the first write: the shipped code
    //  reads +0x18 into a callee-saved register up front (which is what the
    //  `push esi` at the top of that branch is for) and only then starts
    //  writing.  Left inside the chain, the second read happens after the
    //  first call and no register needs saving.
    unsigned int second = entry->m_field18;
    unsigned int first = entry->m_field14;
    store->WriteDword(first)->WriteDword(second);
}
