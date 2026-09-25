//============================================================================
//  JassInstance - the JASS interpreter's per-call-frame object (Jass2/
//  Instance.h).  Its constructor (0x6F4549E0, jassvm-batch-1's own
//  reconstruction) is a 0x28A8-byte allocation that walks a long trail of
//  named sub-tables - ScriptDataTable, TAllocatorHashTable<FuncAddr,...>,
//  TAllocatorHashTable<JassHandle,...>, TAllocatorHashTable<
//  String2HandleReg,...>, TAllocatorHashTable<FuncAddr2HandleReg,...>,
//  TAllocatorHashTable<FuncAddr2NameReg,...> - each stamped with its own
//  real vtable in the disassembly (`??_7?$TAllocatorHashTable@U...`).  Only
//  the offsets this session's own functions read or write are named here;
//  everything else in the object - including most of that table trail - is
//  reserved rather than guessed at, because the tables' own node types are
//  out of this closure's reach (no AllocNode/Ptr call in this slice reaches
//  far enough to type them).
//============================================================================
#ifndef JASSINSTANCE_H
#define JASSINSTANCE_H

#include "storm.h"

class CDataStore;

//  0x6F459660's own struct (jassenginehandlerefs.cpp) - a fixed-stride
//  handle table: count at +4, entries at +8, each entry 0x10 bytes with
//  its own refcount at +0xC.  JassInstance's own +0x2874 is this same
//  table (ReleaseCompiledFunc reads the identical shape).
struct JassHandleTableHeader
{
    char         m_reserved0[4];
    unsigned int m_count;     // +0x04
    void*        m_entries;   // +0x08 - stride 0x10, refcount at +0xC
};

class JassInstance
{
public:
    //  0x6F4549E0 - `retn 8`, two stack args (__thiscall - `this` is the
    //  freshly allocated 0x28A8-byte block every call site hands it).
    //  jassinstancebigthunks.cpp (kept a naked redirect - see that file's
    //  own note; spelled as an ordinary member rather than a real
    //  constructor because MSVC will not take `__declspec(naked)` on one -
    //  Containers/hashtablectors.cpp's own precedent).
    void Construct(void** globalsBase, int flag);

    //  0x6F455250 - `retn 0x10`, four stack args (__thiscall).
    //  jassinstancebigthunks.cpp.
    void SpawnChildInstance(void*, void*, void*, void*);

    //  0x6F45E300 - `retn 0`, no stack args (__thiscall).
    //  jassinstancebigthunks.cpp.
    JassInstance* CreateChildInstance();

    //  0x6F45E140 - `retn 4` (__thiscall).  jassinstancebigthunks.cpp.
    void LoadScriptDataRecord(void* store);

    //  0x6F459180 - `retn 4`, one stack argument (__thiscall).
    void SetField48(int value);
    //  0x6F459190 - `retn 0`, no stack argument.
    int GetField48();

    char          m_reserved00[0x18];
    //  +0x18/+0x1C - a two-word state pair InitSpawnFields (below) resets on
    //  a freshly spawned child: m_field18 to 0 and m_field1C to the parent's
    //  own m_field1C minus one - a shape that reads as "recursion budget
    //  carried down one level", but nothing else in this closure reads
    //  either field back to confirm it.
    int           m_field18;              // +0x18
    int           m_field1C;              // +0x1C
    //  0x6F459610 - `retn 4`, one stack arg (__thiscall): the spawning
    //  parent instance.  Also invalidates m_cachedGlobalSlotIndex (-1).
    void InitSpawnFields(JassInstance* parent);
    //  +0x20 - the running opcode stream's own cursor: null when nothing
    //  has been compiled/executed into this instance yet (tested by
    //  sub_6F44B640, still not reconstructed this session, to choose
    //  between re-running an already-compiled stream and compiling one
    //  for the first time), advanced 4 bytes per decoded opcode by
    //  sub_6F45E9D0 (kept THUNK - a 1116-instruction `_except_handler4`-
    //  SEH-framed opcode-stream walker/interpreter this fixed
    //  `/GS- /EHs-c-` toolchain cannot emit; see jassexecutestream.h's
    //  own note).  This is the concrete field CLAUDE.md's own "where's
    //  its instruction pointer" question was asking about.
    void*         m_streamCursor;         // +0x20
    char          m_reserved24[0x48 - 0x24];
    //  +0x48 - set to 1 by the constructor's own default, overwritten from
    //  a caller-supplied value by sub_6F459180 (JassInstance::SetField48)
    //  right after construction (see jassinstancespawn.cpp).  Nothing in
    //  this closure says what it counts; jassnativehandledispatch.cpp's
    //  own dispatch reads it through neither of its two paths, so it stays
    //  a plain int rather than a guessed enum.
    int           m_field48;              // +0x48
    char          m_reserved4C[0x2854 - 0x4C];
    //  +0x2854 - a one-entry cache invalidated to -1 by InitSpawnFields
    //  right alongside m_field18/m_field1C above, immediately ahead of
    //  m_field2858 itself; named for what GetGlobalSlotIndex (below) hands
    //  back, the most likely thing a one-slot cache sitting next to the
    //  globals base pointer would hold.
    int           m_cachedGlobalSlotIndex; // +0x2854
    //  +0x2858 - the constructor's own `arg_0` (its caller's first
    //  parameter): a pointer to the global-variable array's own base
    //  pointer (one indirection past the array itself), indexed by
    //  JassInstance::GetGlobalSlot (jassinstanceglobalslot.cpp).
    void**        m_field2858;            // +0x2858
    //  0x6F4596D0 - `retn 4`, `&(*m_field2858)[index]`.
    void** GetGlobalSlot(unsigned int index);
    //  0x6F459580 - `retn 0`.  Plain getter for m_field2858 itself (not an
    //  indexed slot) - jassinstanceextrafields.cpp.
    void** GetGlobalsBase();
    //  0x6F4596B0 - `retn 4`.  The inverse of GetGlobalSlot: given a slot
    //  pointer this instance itself handed back, recover its index.
    //  jassinstanceextrafields.cpp.
    int GetGlobalSlotIndex(void* slot);
    char          m_reserved285C[0x2874 - 0x285C];
    //  +0x2874/+0x2878/+0x287C - three of the constructor's own allocation
    //  trail entries (the 0x10-byte block right before
    //  TAllocatorHashTable<String2HandleReg,...> and the two 0x10-byte
    //  blocks - blocks "16Bh"/"16Ch" in that trail's own tags - reached
    //  from the same neighbourhood; see jassnativecalldispatch.cpp's own
    //  JassInstance::ReleaseCompiledFunc for the one member that touches
    //  them).  Their own element types are as unconfirmed as the rest of
    //  the trail this closure never resolves a node type for, save that
    //  +0x2874 is the same [count@+4,entries@+8,stride 0x10,refcount@+0xC]
    //  shape jassenginehandlerefs.cpp's own JassHandleTableHeader already
    //  names for this same object (AddHandleRef reads the identical offset
    //  through a differently-named outer class - same field, two sessions).
    JassHandleTableHeader* m_field2874;   // +0x2874
    //  0x6F459640 - `retn 4`.  &m_field2874->m_entries[index] as a raw byte
    //  pointer (stride 0x10, per JassHandleTableHeader's own note above).
    //  jassinstanceextrafields.cpp.
    void* GetHandleTableEntry(unsigned int index);
    TSGrowableArray<void*>* m_field2878;  // +0x2878 - a free-index list;
                                           // ReleaseCompiledFunc's own tail
                                           // Grow()s it and stores the freed
                                           // index into the new slot.
    void*         m_field287C;            // +0x287C - a table with its own
                                           // vtable (slot 0 called directly
                                           // in ReleaseCompiledFunc); Find/
                                           // Unlink bodies are unreconstructed
                                           // (see that file's own note).
    char          m_reserved2880[0x2884 - 0x2880];
    //  +0x2884 - TAllocatorHashTable<FuncAddr,HASHKEY_CONSTSTR,256>* (the
    //  RTTI-stamped table the constructor builds at this exact offset -
    //  0x6F454AF2's own `??_7?$TAllocatorHashTable@UFuncAddr@@...` comment).
    //  Declared as `void*` rather than the real template: FuncAddr's own
    //  layout and its table's Ptr() (0x6F45AE80) are jassvm-batch-2's own
    //  reconstruction, out of this session's reach.
    void*         m_funcAddrTable;        // +0x2884
    //  0x6F45E8C0 - `retn 4`.
    void* FindFuncAddr(unsigned int key);
    //  +0x2888 - TSGrowableArray<void*>*, indexed by GetField2888Element
    //  below.  No caller in this closure ever writes it or the pointer it
    //  refers to, so which of the constructor's own allocation-trail
    //  entries this is stays unconfirmed past "one more pointer-array
    //  field in the same neighbourhood as m_field2878".
    TSGrowableArray<void*>* m_field2888;  // +0x2888
    //  0x6F459690 - `retn 4`, `(*m_field2888)[index]`.
    //  jassinstanceextrafields.cpp.
    void* GetField2888Element(unsigned int index);
    char          m_reserved288C[0x28A0 - 0x288C];
    //  +0x28A0/+0x28A4 - a resolved native function pointer and its extra
    //  argument, both written directly by the instance's own caller (not
    //  the constructor) right after construction - see
    //  jassinstancespawn.cpp - and read by the two dispatch members in
    //  jassnativecalldispatch.cpp.
    void*         m_nativeFuncPtr;        // +0x28A0
    void*         m_nativeFuncArg;        // +0x28A4
    //  0x6F459560 - `retn 0`.  jassinstanceextrafields.cpp.
    void* GetNativeFuncPtr();
    //  0x6F459570 - `retn 0`.  jassinstanceextrafields.cpp.
    void* GetNativeFuncArg();

    //  0x6F45B1A0 - `retn 4`.  jassnativecalldispatch.cpp.
    void** ReleaseCompiledFunc(unsigned int index);

    //  0x6F4484F0 - `__fastcall(ecx=arg, edx=this)`, `retn 0`.  A thin
    //  wrapper tail-calling Run(arg) and handing back `arg` itself rather
    //  than Run's own return value - jassnativecalldispatch.cpp.
    void* Run(void* arg);

    //  0x6F45D720 - `retn 0`.  jassinstancedispatch.cpp.
    void ExecuteDeclStream();

    //  0x6F45E450 - `retn 4`.  jassinstancedispatch.cpp.
    void LoadScriptDataRecords(CDataStore* store);

    //  0x6F45E9D0 - `retn 10h`.  jassexecutestream.h/.cpp - kept THUNK,
    //  see that header's own note.  The VM's own opcode-stream
    //  interpreter: walks `cursor` (normally m_streamCursor itself)
    //  4 bytes per decoded opcode through a 42-way switch.
    void* ExecuteOpcodeStream(void* cursor, int arg2, int stepBudget,
                               int arg4);

    //  0x6F45FD00 - `retn 0`.  jassexecutestream.h/.cpp.  Re-run the
    //  compiled opcode stream already sitting at m_streamCursor with a
    //  fresh 0x493E0 (300000) step budget.  Return type corrected this
    //  round (jass-instance-targeting-D) from `void` to `int`: the body
    //  is a pure tail-call into ExecuteOpcodeStream with nothing after it
    //  (`call sub_6F45E9D0` then a bare `retn`), and RunOrCompileStream's
    //  own caller (jassrunorcompilestream.cpp, and now
    //  jassgetorrunscriptinstance.cpp's own GetOrRunJassInstanceForScript
    //  above it) genuinely reads the eax this leaves behind as a success
    //  code - see docs/notes/jass-runtime-instance-targeting.md.
    int RunOpcodeStream();
};

//  0x6F4484F0 - jassnativecalldispatch.cpp.
void* __fastcall JassInvokeInstanceRun(void* arg, JassInstance* instance);

//  A bare `this`-only view of the FuncAddr table, declared just deep enough
//  to reach 0x6F45AE80 as a real __thiscall member call rather than a free
//  function, which is the only way MSVC accepts that calling convention
//  outside a class.  NOTE: a later rebase shows 0x6F45AE80 already claimed
//  as `TAllocatorHashTable<JASSTYPENODE2,HASHKEY_STR,64>::PtrInterned` (a
//  jassvm-batch-2 table with no relation to FuncAddr) - either this
//  session misattributed the callee, or, like this repo's own
//  ComputeChunk/UnlinkAll instantiations, the two tables' Ptr bodies
//  happen to compile byte-identical and share the one address.  Left
//  unclaimed in funcmap.py either way (no row added for this address here)
//  so the two names never collide; worth a fresh look before this is
//  trusted for a live hook.
class JassFuncAddrTableView
{
public:
    //  0x6F45AE80 - kept a naked redirect (not a reconstruction) because
    //  ownership of this exact address is ambiguous - see this header's
    //  own note above, by JassInstance::FindFuncAddr's use of it.
    void* Ptr(unsigned int key);
};

#endif
