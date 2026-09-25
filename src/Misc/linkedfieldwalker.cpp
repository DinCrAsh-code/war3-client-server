//============================================================================
//  0x6F421A20 / 0x6F421E50 - a small linked-list walk and its zero-step
//  caller.  Neither receiver is named further than "whatever object owns
//  this shape at this offset" - nothing in this call tree's own reach
//  establishes a class for either.
//
//  0x6F421A20 calls the already-reconstructed SFallbackFieldOwner::
//  Field04Or08 (0x6F41FF40, Misc/sweep_predicates.cpp) on the object four
//  bytes into `this`, so it is declared again here rather than defined -
//  the same body, the same mangled name, one definition.
//============================================================================
struct SFallbackFieldOwner { int __thiscall Field04Or08(const int* candidate); };

//----------------------------------------------------------------------------
//  0x6F421A20 - step `count` times through the list rooted at +0x0C, each
//  step Field04Or08 on the object at +0x04, and hand back +0x08 of
//  wherever that lands.  `retn 4`.
//
//  Three of the shipped tests fold into the loop guard as written:
//  `count >= +0x10` bails to 0 outright before anything else runs, a
//  non-positive starting cursor never enters the loop at all (its own
//  `test/jle` collapses into the loop condition below), and the loop
//  itself re-tests the cursor after every step and returns 0 the moment it
//  goes non-positive rather than finishing the walk - which is why the
//  count comparison happens before the step and not after.
//----------------------------------------------------------------------------
struct SLinkedFieldWalker
{
    int __thiscall WalkAndFetchField08(unsigned int count);
};

int __thiscall SLinkedFieldWalker::WalkAndFetchField08(unsigned int count)
{
    if (count >= *(unsigned int*)((char*)this + 0x10))
        return 0;

    int cursor = *(int*)((char*)this + 0x0C);
    if (cursor <= 0)
        return 0;

    for (unsigned int i = 0; i < count; i++)
    {
        cursor = ((SFallbackFieldOwner*)((char*)this + 4))
                     ->Field04Or08((const int*)cursor);
        if (cursor <= 0)
            return 0;
    }

    return *(int*)(cursor + 8);
}

//----------------------------------------------------------------------------
//  0x6F421E50 - the zero-step case: no list to walk, so this is really
//  "peek the head's own +0x08", guarded by the pointer at +0x20C existing
//  at all.  `retn 0`.
//----------------------------------------------------------------------------
struct SFieldWalkerHolder
{
    SLinkedFieldWalker* m_pWalker;   // +0x20C
};

int __fastcall PeekFieldWalkerHead(SFieldWalkerHolder* self)
{
    SLinkedFieldWalker* walker = *(SLinkedFieldWalker**)((char*)self + 0x20C);
    if (walker == 0)
        return 0;
    return walker->WalkAndFetchField08(0);
}
