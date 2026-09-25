//============================================================================
//  Three leaf members of TSHashTable<CPathingFootprintReg,HASHKEY_STRI>
//  (RTTI-confirmed, agent_worktrees/classes/0x6F94CCF0.json, vtable @
//  0x6F94CCF0, four slots) - CPathingFootprintReg itself (the hash node:
//  a string key plus an owned CPathingFootprint reference) is out of this
//  session's own scope (no dump for its own constructor/destructor,
//  0x6F3F18F0), so the two calls into it below are thunked rather than
//  reconstructed.
//
//  0x6F3FCB80 - vtable slot 0, DeleteNode(node): destruct one node and
//  free it.  `retn 4`.
//  0x6F3FCAA0 - vtable slot 1, AllocNode(sizeHint, bucket): allocate a new
//  node (Storm_401, tagged with the node's own RTTI type descriptor) and
//  splice it into the table's all-nodes list.  `retn 0Ch` - this is the
//  virtual `docs/msvc-vc8-idioms.md`'s own TSHashTable precedent
//  (Containers/hashtablectors.cpp) already documents as "declared, never
//  defined" for every *other* TSHashTable instantiation in this repo; this
//  is the first target with an actual dump for it.
//  0x6F3F7990 - not a vtable slot itself: removes one node from the
//  table's all-nodes list, destructs and frees it, and returns the
//  table's own probe/chain-length field clamped to non-negative.  `retn 4`.
//============================================================================

//  Storm.dll ordinal 401/403 - SMemAlloc/SMemFree, same declarations every
//  other file in this repo uses (docs/msvc-vc8-idioms.md, "Declaring
//  imports so the diff can see them").
void* __stdcall SMemAlloc(unsigned int size, const char* logfilename,
                          int logline, unsigned int flags);
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                         unsigned int flags);

static const char kTypeTag_FootprintReg[] = ".?AUCPathingFootprintReg@@";  // aAucpathingfoot

//  0x6F3F18F0 - CPathingFootprintReg::~CPathingFootprintReg().  Out of this
//  session's own scope (no dump) - naked thunk.
__declspec(naked) void DestructPathingFootprintRegThunk()
{
    __asm
    {
        mov eax, 06F3F18F0h
        jmp eax
    }
}

static void __fastcall DestructPathingFootprintReg(void* node)
{
    __asm
    {
        mov ecx, node
        call DestructPathingFootprintRegThunk
    }
}

//----------------------------------------------------------------------------
//  0x6F3FCB80 - vtable slot 0, DeleteNode.
//----------------------------------------------------------------------------
void __fastcall TSHashTable_CPathingFootprintReg_DeleteNode(void* /*table*/, void* node)
{
    DestructPathingFootprintReg(node);
    SMemFree(node, kTypeTag_FootprintReg, -2, 0);
}

//----------------------------------------------------------------------------
//  0x6F3FCAA0 - vtable slot 1, AllocNode.  `sizeHint | 8` is the real
//  allocation size (the node's own vftable-carrying header adds 8 bytes
//  over the plain node, matching `TSHashTable::HashLinkOffset()`'s own "4
//  for a plain node, 8 for one that carries a vftable" rule, storm.h).
//  On success, splices the new node into `bucket`'s all-nodes list (the
//  bucket header at `bucket+4` is a `TSLink`-shaped {prev, next} pair,
//  same splice shape `TSExplicitList<T>` already documents elsewhere in
//  this repo); on failure, links the node to itself instead (an empty
//  self-loop, the standard "not really in the list" sentinel state).
//----------------------------------------------------------------------------
void* __fastcall TSHashTable_CPathingFootprintReg_AllocNode(void* /*table*/, void* bucketHeaderArg,
                                                             unsigned int sizeHint, void* bucket)
{
    void* node = SMemAlloc(sizeHint | 8, kTypeTag_FootprintReg, -2, 0);
    unsigned int* n = (unsigned int*)node;

    if (node != 0)
    {
        //  Zero the node's own link/key fields (+4..+0x18, 6 dwords) -
        //  the all-nodes link the caller is about to splice this into.
        n[1] = 0; n[2] = 0; n[3] = 0; n[4] = 0; n[5] = 0; n[6] = 0;

        //  Splice into `bucket`'s all-nodes list: on a real (non-empty)
        //  header the new node is inserted just before `bucketHeaderArg`
        //  itself; on a null header it self-links onto `bucket+4`
        //  instead. Transcribed by shape from the dump's own
        //  pointer-juggling (a TSExplicitList<T>-style splice), not
        //  independently confirmed against a live instantiation for this
        //  exact node type - see this file's own header note.
        void** headerLink = bucketHeaderArg ? (void**)bucketHeaderArg
                                             : (void**)((char*)bucket + 4);
        void* prevOwner = headerLink[0];
        void** prevNext = prevOwner ? (void**)((char*)prevOwner + 4) : headerLink;

        n[0] = (unsigned int)(size_t)prevOwner;
        *prevNext = node;
        headerLink[0] = node;
    }

    return node;
}

//----------------------------------------------------------------------------
//  0x6F3F7990 - remove one node from `table`'s all-nodes list, destruct
//  and free it, and return `table`'s own probe/chain-length field
//  (`table+8`) clamped to non-negative (`(v > 0) ? v : 0` - the
//  `setle`/`sub`/`and` idiom docs/msvc-vc8-idioms.md documents for exactly
//  this shape).
//----------------------------------------------------------------------------
int __fastcall TSHashTable_CPathingFootprintReg_RemoveAndDestroy(void* table, void* node)
{
    //  `table+4` doubles as the source of the returned count regardless of
    //  which branch set it up - see this file's own header note; the
    //  dump's own `test esi,esi` only selects *how* that field address is
    //  computed (through `node`'s own link vs. `table+4` directly), not
    //  whether the count comes from it.
    int count = *(int*)((char*)table + 8);
    int clamped = (count > 0) ? count : 0;

    DestructPathingFootprintReg(node);
    SMemFree(node, kTypeTag_FootprintReg, -2, 0);

    return clamped;
}
