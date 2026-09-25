//============================================================================
//  A new subsystem this closure's own dumps surface for the first time: a
//  small object-database family (`.\Database.cpp` in the ctors' own Storm
//  allocation tag, `CDatabaseBase`/`CDatabaseHashTable`/`CSLKDatabase`/
//  `CDBDatabase` in the vtable symbols each constructor stamps) that reads
//  `.slk`-shaped game-data sheets - the same kind of thing
//  `src/Game/`'s own `CObjectDataTable` (README.md's own description) reads
//  `Units\\UpgradeData` through, though nothing in this closure's own reach
//  connects the two directly.  Two `CDatabaseHashTable` subobjects sit at
//  +0x10 and +0x38 of the constructed object (0x28 bytes apart - exactly
//  `TSHashTable<SDatabaseFieldRecord, HASHKEY_STR>`'s own size, per
//  citemclosure_dbfieldhash.cpp, which both `sub_6F4CA160` and
//  `sub_6F4CA0F0` confirm by calling that same table's `Ptr()` on each in
//  turn), tagged `"HDBFIELD"`/`"HDBENTRY"` at their respective insert call
//  sites (out of this batch's own scope - the inserts themselves,
//  `sub_6F4C9C00`/`sub_6F4C9C90`/`sub_6F4C99F0`, are deeper still).
//
//  Every function below is a redirect rather than a reconstruction, for
//  the reason Containers/hashtablectors.cpp already gives for this exact
//  situation: `sub_6F4CB210`/`sub_6F4CB5C0` stamp `??_7CSLKDatabase@@6B@`/
//  `??_7CDBDatabase@@6B@` and two `??_7CDatabaseHashTable@@6B@` subobject
//  stamps apiece, and this repo has no `docs/targets/vtables/` entry - no
//  `ida_query vtable` answer at all - for any of `CDatabaseBase`,
//  `CDatabaseHashTable`, `CSLKDatabase` or `CDBDatabase`.  Writing these out
//  as real C++ constructors would stamp vtables of this build's own
//  invention, of unknown length, over four shipped classes; that is exactly
//  the "wrong-length vtable no instruction score can see" trap
//  check_vtables.py and ctor_vtable_audit.py exist to catch, so - as with
//  the font cache and command-registry tables in hashtablectors.cpp - these
//  stay redirects until a `vtable` query settles all four real shapes.  The
//  three callees below them (`sub_6F4C9B10`, `sub_6F705520`, and the SEH-
//  framed field/entry inserts `sub_6F4C9C00`/`sub_6F4C9C90`) are non-
//  constructor members with their own open questions (an indirect call
//  through `this`'s own vtable slot 0 whose real signature nothing in this
//  dump settles, and a teardown loop into `sub_6F4C9B80` that is itself out
//  of this batch's scope) and are grouped here for the same "leave it
//  working, not guessed at" reason.
//============================================================================

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  MSVC will not accept __thiscall on a free function (only a native
//  member function may declare it - see GameUI/selectmodector.cpp's own
//  comment on the same restriction for __declspec(naked) on a
//  constructor), so a thunk taking `this` plus a real stack argument is
//  spelled as a one-method struct's member, the same shape
//  Containers/hashtablectors.cpp already uses for its own four redirects.
struct SCSLKDatabaseCtorThunk    { void* Construct(int arg0); };
struct SCDBDatabaseCtorThunk     { void* Construct(int arg0, int arg1); };

//  0x6F4CB210 - CSLKDatabase's constructor.  `retn 4`.
ADDR_THUNK(void* SCSLKDatabaseCtorThunk::Construct(int arg0), 0x6F4CB210)

//  0x6F4CB5C0 - CDBDatabase's constructor, building its own two
//  CDatabaseHashTable subobjects and then copying a source header's field
//  count/stride and walking its rows (the `cmp byte ptr [ecx], 'S'`/
//  `add [ecx+4], edx` loop).  `retn 8`.
ADDR_THUNK(void* SCDBDatabaseCtorThunk::Construct(int arg0, int arg1),
           0x6F4CB5C0)

//  0x6F4C9B10 - walks both CDatabaseHashTable subobjects' m_list
//  (TSExplicitList<SDatabaseFieldRecord>), incrementing every live node's
//  refcount by one (`add [eax+4], edi` with `edi==1`) without unlinking or
//  destroying anything - a "keep everything alive across a rehash/copy"
//  sweep, not a teardown (compare Misc/citemclosure_wedbentryhash.cpp's
//  UnlinkAll, which drains the list this one only walks).  `retn 0`.
ADDR_THUNK(void __fastcall DatabaseRecordSet_AddRefAllNodes(void* self),
           0x6F4C9B10)

//  0x6F705520 - the WEDBEntryHash table's own Clear(int): unlinks
//  `m_list`, then per bucket either detaches each node in place (the
//  `arg_0 != 0` arm - the same inline TSLink::Unlink() shape
//  Misc/citemclosure_wedbentryhash.cpp's UnlinkAll uses, written out here
//  rather than called) or, when `arg_0 == 0`, calls `this`'s own vtable
//  slot 0 with the node pointer as its one explicit argument - a virtual
//  dispatch this dump never resolves to a name (no committed vtable entry
//  for `TSHashTable<WEDBEntryHash,HASHKEY_STRI>`'s own slot 0 beyond "the
//  destructor", which does not explain a call taking a node pointer), so
//  guessing at its signature would be the exact mistake
//  tools/vtable_dispatch_audit.py exists to flag.  `retn 4`.
struct SWEDBEntryHashTableThunk { void Clear(int destroyNodes); };
ADDR_THUNK(void SWEDBEntryHashTableThunk::Clear(int destroyNodes),
           0x6F705520)

//  0x6F4C9C00 / 0x6F4C9C90 - SEH-framed inserts into the two
//  CDatabaseHashTable subobjects above ("HDBFIELD" at +0x10, "HDBENTRY" at
//  +0x38): allocate a node through the shared CDataAllocator singleton
//  (Net/dataallocator.h), initialise it via `sub_6F4C99F0` (out of this
//  batch's scope) on a hit, then AddRef() the result either way (a null
//  AddRef is the documented "report and return null" case
//  Agent/refcnt.h's own AddRef() comment already covers).  Both carry the
//  `__except_handler4`-shaped frame docs/msvc-vc8-idioms.md's own note
//  says this toolchain's bundled `cl.exe` cannot reproduce even from
//  hand-written source - see "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce" for the two functions where a third session
//  already proved that out.  `retn 4` each.
struct SDatabaseFieldTableThunk { void* Insert(int key); };
struct SDatabaseEntryTableThunk { void* Insert(int key); };
ADDR_THUNK(void* SDatabaseFieldTableThunk::Insert(int key), 0x6F4C9C00)
ADDR_THUNK(void* SDatabaseEntryTableThunk::Insert(int key), 0x6F4C9C90)
