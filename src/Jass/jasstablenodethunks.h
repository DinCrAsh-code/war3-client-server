//============================================================================
//  `.\Jass.cpp` - a family of small hash-table-node record types
//  (NativeFunc, HashedString, ScriptData, FuncAddr, JassHandle,
//  String2HandleReg, FuncAddr2HandleReg, FuncAddr2NameReg,
//  Script2HandleReg), each with the same three-part shipped shape, kept as
//  redirects for the same reason jasshashtablectors.h/jasshashtablectors2.h
//  already document for these tables' own Construct() bodies:
//
//    - a `New<Type>Node(table, flag, extra)` - `retn 0xC`, `__stdcall`
//      (neither incoming register is read before being overwritten, so this
//      is not a `this`-taking method despite manipulating a table object -
//      TAllocatorHashTable<T,K,N>::NewNode's own shape, Containers/
//      allochashtable.h) that stamps the record's own `??_7<Type>@@6B@`
//      vtable and splices the node into the table's bucket and all-nodes
//      lists.  Reached twice each for several of these types (no
//      `/OPT:ICF`, so the identical body is compiled out-of-line at more
//      than one call site) - the second copy is named `...Alias`, the same
//      convention Containers/commandregtable_dtor.cpp's own
//      `SCommandRegBucketArrayDtorAlias` uses for the compiler's own second
//      copy of one destructor, not a second real function;
//    - a `Destruct()` - `retn 0`, real `this`-in-ecx - that also stamps that
//      vtable (MSVC's ABI resets a class's own vtable pointer at the start
//      of its destructor).  This is a destructor and not a second
//      constructor because a caller proves it: 0x6F451CA0's own
//      scalar-deleting-destructor calls 0x6F450370 and then conditionally
//      frees `this`, which only makes sense if 0x6F450370 already tore the
//      object down - the vtable-stamp shape alone cannot tell a constructor
//      and a destructor apart, only a caller can;
//    - NativeFunc alone also has a `DeletingDestructor(flags)` - `retn 4`,
//      real `this`-in-ecx - the MSVC scalar-deleting-destructor wrapper
//      around a `Destruct()`: test bit 0 of the caller's flags, `operator
//      delete` the object through Storm ordinal 403 when it is set.  Six of
//      the other eight types have a *different*, simpler shape instead - see
//      the `Free<Type>`/`Dtor()` pairs below - which is real, not a
//      redirect: no flags argument, no conditional delete, just an
//      unconditional "tear down, then free" `retn 4` free function taking
//      the object on the stack, with its own base `Dtor()` unlinking two
//      embedded `TSLink<T>` members.
//
//  None of these declares a `virtual` method (CLAUDE.md's own house rule),
//  so none can stamp the real, shipped multi-slot vtable for real -
//  constructing or destroying one of these node types would install a
//  wrong-length vtable no instruction score can see and that dispatches the
//  wrong function at or past the mistake, exactly what tools/check_vtables.py
//  exists to catch.  A redirect leaves the shipped vtable, and the shipped
//  virtual dispatch through it, untouched - the same reasoning
//  jasshashtablectors.h/jasshashtablectors2.h already carry for these same
//  tables' own Construct() entry points.
//============================================================================
#ifndef JASSTABLENODETHUNKS_H
#define JASSTABLENODETHUNKS_H

#include "tslist.inl"
#include "jassscriptframepool.h"   // ReleaseLocalSlot - 0x6F45A040

//----------------------------------------------------------------------------
//  A real, shared shape found while attempting these reconstructions: six of
//  the nine record types embed two adjacent TSLink<T> members (the table's
//  own bucket-chain link at +0x04, the table-wide list's link at +0x0C) and
//  release them with two `Unlink()` calls, most-recently-declared-first
//  (+0x0C then +0x04).  The freeing wrapper that calls this base `Dtor()` and
//  then `SMemFree`s the node is a plain `retn 4` free function taking the
//  object on the stack - not MSVC's scalar-deleting-destructor shape (no
//  flags argument, no conditional `delete`) despite looking similar at a
//  glance; NativeFunc's own equivalent (0x6F44FB90, kept a redirect below)
//  really is the scalar-deleting-destructor shape, which is how the two were
//  first told apart.
//----------------------------------------------------------------------------
struct FuncAddr
{
    char              m_reserved00[4];
    TSLink<FuncAddr>  m_reserved04;   // +0x04 - the table's own bucket chain
    TSLink<FuncAddr>  m_reserved0C;   // +0x0C - the table-wide all-nodes list

    void Dtor();   // 0x6F450780 - `retn 0`
};
void __stdcall FreeFuncAddr(FuncAddr* obj);    // 0x6F450980 - `retn 4`

struct String2HandleReg
{
    char                       m_reserved00[4];
    TSLink<String2HandleReg>   m_reserved04;
    TSLink<String2HandleReg>   m_reserved0C;

    void Dtor();   // 0x6F450EE0 - `retn 0`
};
void __stdcall FreeString2HandleReg(String2HandleReg* obj);   // 0x6F4510E0 - `retn 4`

struct FuncAddr2HandleReg
{
    char                          m_reserved00[4];
    TSLink<FuncAddr2HandleReg>    m_reserved04;
    TSLink<FuncAddr2HandleReg>    m_reserved0C;

    void Dtor();   // 0x6F451340 - `retn 0`
};
void __stdcall FreeFuncAddr2HandleReg(FuncAddr2HandleReg* obj);   // 0x6F451540 - `retn 4`

struct JassHandle
{
    char                m_reserved00[4];
    TSLink<JassHandle>  m_reserved04;
    TSLink<JassHandle>  m_reserved0C;

    void Dtor();   // 0x6F44BED0 - `retn 0` (this record's own two-link
                    // release, called from 0x6F450C80 below - out of this
                    // sweep's own assigned address range, but small,
                    // self-contained, and needed to make that real)
};
void __stdcall FreeJassHandle(JassHandle* obj);   // 0x6F450C80 - `retn 4`

//  HashedString frees one more thing first: an owned buffer at +0x14 the
//  table doesn't itself allocate the way a plain node's fields do - see
//  jasshashedstringtable.h's own SHashedStringTablePrimer for the allocating
//  side of that same field.
struct HashedString
{
    char                    m_reserved00[4];
    TSLink<HashedString>    m_reserved04;
    TSLink<HashedString>    m_reserved0C;
    char                    m_reserved14pad[8];
    void*                   m_ownedBuffer;   // +0x14 - freed first, generic
                                              // Storm header tag (not this
                                              // record's own type descriptor)

    void Dtor();   // 0x6F44BE40 - `retn 0` (out of this sweep's own range,
                    // same reasoning as JassHandle::Dtor above)
};
void __stdcall FreeHashedString(HashedString* obj);   // 0x6F450250 - `retn 4`

//  ScriptData needs no record-specific Dtor of its own: its base release is
//  the already-thunked, already-named ReleaseLocalSlot (0x6F45A040,
//  jassscriptframepool.h) - a JASS script frame's own local-slot teardown,
//  reused here rather than redeclared.
struct ScriptData;
void __stdcall FreeScriptData(ScriptData* obj);   // 0x6F4504B0 - `retn 4`


//----------------------------------------------------------------------------
//  NativeFunc - `.\Jass.cpp`'s own TSHashTable<NativeFunc,HASHKEY_STR> node
//  (jassregisternative.h's own NativeFunc struct, jasshashtablectors.h's
//  SJassNativeFuncTable at 0x6F44F990).
//----------------------------------------------------------------------------
void __stdcall NewNativeFuncNode(void* table, unsigned int flag, void* extra);        // 0x6F44FC30 - `retn 0xC`
void __stdcall NewNativeFuncNodeAlias(void* table, unsigned int flag, void* extra);    // 0x6F4527B0 - compiler's own second copy

struct NativeFuncNode
{
    void DeletingDestructor(unsigned int flags);   // 0x6F44FB90 - `retn 4`
};
struct NativeFuncTable
{
    void Destruct();                // 0x6F451BC0 - `retn 0`
};

//----------------------------------------------------------------------------
//  HashedString - TSHashTable<HashedString,HASHKEY_STR>
//  (jasshashedstringtable.h's own SHashedStringHashTable::Construct at
//  0x6F450150).
//----------------------------------------------------------------------------
void __stdcall NewHashedStringNode(void* table, unsigned int flag, void* extra);       // 0x6F450270 - `retn 0xC`
void __stdcall NewHashedStringNodeAlias(void* table, unsigned int flag, void* extra);  // 0x6F452C70 - compiler's own second copy

struct HashedStringTable
{
    void Destruct();                // 0x6F450370 - `retn 0`
    void ConstructComposite();      // 0x6F450410 - `retn 0`, a second (no-SEH)
                                     // physical copy of the same table's own
                                     // fresh-construction path (m_mask set to
                                     // -1, allochashtable.h's own "unbuilt"
                                     // sentinel, rather than a vtable stamp)
};

//----------------------------------------------------------------------------
//  ScriptData - TSHashTable<ScriptData,HASHKEY_CONSTSTR>
//  (jasshashtablectors2.h's own SScriptDataTable::Construct at 0x6F450450).
//----------------------------------------------------------------------------
void __stdcall NewScriptDataNode(void* table, unsigned int flag, void* extra);         // 0x6F450530 - `retn 0xC`
void __stdcall NewScriptDataNodeAlias(void* table, unsigned int flag, void* extra);    // 0x6F452DF0 - compiler's own second copy

struct ScriptDataTable
{
    void Destruct();                // 0x6F450640 - `retn 0`
    void ConstructComposite();      // 0x6F4506E0 - `retn 0`
};

//----------------------------------------------------------------------------
//  FuncAddr - TSHashTable<FuncAddr,HASHKEY_CONSTSTR>
//  (jasshashtablectors2.h's own SFuncAddrTable::Construct at 0x6F450720).
//----------------------------------------------------------------------------
void __stdcall NewFuncAddrNode(void* table, unsigned int flag, void* extra);           // 0x6F4509A0 - `retn 0xC`
void __stdcall NewFuncAddrNodeAlias(void* table, unsigned int flag, void* extra);      // 0x6F452FC0 - compiler's own second copy

struct FuncAddrTable
{
    void Destruct();                // 0x6F450AA0 - `retn 0`
    void ConstructComposite();      // 0x6F450B40 - `retn 0`
};

//----------------------------------------------------------------------------
//  JassHandle - TSHashTable<JassHandle,HASHKEY_CONSTSTR> (jasshashtablectors.h
//  documents this table's own Construct - the very next real address after
//  this closure's own 0x6F450B40 is jasshashtablectors.h's SJassHandleTable
//  at 0x6F450B80, so this node/table pair feeds directly into it).
//----------------------------------------------------------------------------
void __stdcall NewJassHandleNode(void* table, unsigned int flag, void* extra);         // 0x6F450CA0 - `retn 0xC`
void __stdcall NewJassHandleNodeAlias(void* table, unsigned int flag, void* extra);    // 0x6F4530F0 - compiler's own second copy

struct JassHandleTableDtor
{
    void Destruct();                // 0x6F450DA0 - `retn 0`
    void ConstructComposite();      // 0x6F450E40 - `retn 0`
};

//----------------------------------------------------------------------------
//  String2HandleReg - TSHashTable<String2HandleReg,HASHKEY_CONSTSTR>
//  (jasshashtablectors2.h's own SString2HandleTable::Construct at 0x6F450E80).
//----------------------------------------------------------------------------
void __stdcall NewString2HandleNode(void* table, unsigned int flag, void* extra);      // 0x6F451100 - `retn 0xC`
void __stdcall NewString2HandleNodeAlias(void* table, unsigned int flag, void* extra); // 0x6F453220 - compiler's own second copy

struct String2HandleTableDtor
{
    void Destruct();                // 0x6F451200 - `retn 0`
    void ConstructComposite();      // 0x6F4512A0 - `retn 0`
};

//----------------------------------------------------------------------------
//  FuncAddr2HandleReg - TSHashTable<FuncAddr2HandleReg,HASHKEY_NONE>
//  (jasshashtablectors2.h's own SFuncAddr2HandleTable::Construct at
//  0x6F4512E0).
//----------------------------------------------------------------------------
void __stdcall NewFuncAddr2HandleNode(void* table, unsigned int flag, void* extra);        // 0x6F451560 - `retn 0xC`
void __stdcall NewFuncAddr2HandleNodeAlias(void* table, unsigned int flag, void* extra);   // 0x6F453350 - compiler's own second copy

struct FuncAddr2HandleTableDtor
{
    void Destruct();                // 0x6F451660 - `retn 0`
};

//----------------------------------------------------------------------------
//  FuncAddr2NameReg - TSHashTable<FuncAddr2NameReg,HASHKEY_NONE>
//  (jasshashtablectors2.h's own SFuncAddr2NameTable::Construct at 0x6F451740).
//----------------------------------------------------------------------------
//  0x6F4519C0 - `retn 0xC`, SEH-guarded.  Not the same node-insert shape the
//  other eight types' NewXNode above are: this allocates through
//  CDataAllocator::Alloc (sub_6F4C1AB0, the same allocator the flagship AST
//  builder's own NodeXxx constructors use - docs/targets/
//  jassvm-flagship-opcode-table.md) tagged "AUFuncAddr2Nam", then
//  placement-constructs into it via sub_6F451930 - the same "resolve or
//  create" shape jassregisternative.cpp's ResolveNativeFuncRecord already
//  documents for a sibling table, just reached through the allocator rather
//  than a hash table's own NewNode.  Kept a redirect: sub_6F451930 is
//  unreconstructed, out of this batch's own closure.
void __stdcall NewFuncAddr2NameNode(void* table, unsigned int flag, void* extra);      // 0x6F4519C0 - `retn 0xC`

//  0x6F451A50 - `retn 4`.  This one *is* the plain `Free(obj)` shape (calls
//  a base `Dtor()`, then `SMemFree`s) - but that base, sub_6F4518E0, is
//  itself SEH-guarded and calls two more unreconstructed functions
//  (sub_6F4C48F0, sub_6F4517A0), so unlike FuncAddr/JassHandle/
//  String2HandleReg/FuncAddr2HandleReg/HashedString above there is no small,
//  self-contained base to pull in here. Kept a redirect for that reason.
void __stdcall FreeFuncAddr2NameReg(void* obj);   // 0x6F451A50 - `retn 4`

//  0x6F453410 - `retn 4`, SEH-guarded.  A *third* shape for this same type:
//  allocates through CDataAllocator::Alloc (sub_6F4C1AB0) tagged
//  "AUFuncAddr2Nam", then placement-constructs via sub_6F451930 - the same
//  "resolve or create" pattern jassregisternative.cpp's
//  ResolveNativeFuncRecord documents for a sibling table, just reached
//  through the allocator rather than a hash table's own NewNode. Kept a
//  redirect: sub_6F451930 is unreconstructed, out of this batch's closure.
void __stdcall ResolveFuncAddr2NameRecord(void* arg);   // 0x6F453410 - `retn 4`

struct FuncAddr2NameTableDtor
{
    void Destruct();                // 0x6F451AE0 - `retn 0`
};

//----------------------------------------------------------------------------
//  Script2HandleReg - TSHashTable<Script2HandleReg,HASHKEY_NONE>
//  (jasshashtablectors.h's own SJassScript2HandleTable::Construct at
//  0x6F4528D0).
//----------------------------------------------------------------------------
void __stdcall NewScript2HandleNode(void* table, unsigned int flag, void* extra);      // 0x6F452A80 - `retn 0xC`

struct Script2HandleTableDtor
{
    void Destruct();                // 0x6F4535A0 - `retn 0`
};

#endif
