//============================================================================
//  The JASS interpreter's per-thread state, and the two writes into the
//  *running* thread that the trigger natives make.
//
//  The class name is the binary's own: `??_7JassThreadLocal@@6B@` sits at
//  0x6F950EA8 with `aJassCpp` ("Jass.cpp") four bytes past it, so slot 5 of
//  this thread's pointer array is a `JassThreadLocal` and 0x6F44B2F0 /
//  0x6F44B3A0 are two free functions in Jass.cpp.
//
//  jassthreadlocalctor.cpp's own reconstruction of the constructor
//  (0x6F453930) now gives the rest of this class's layout, and confirms
//  rather than contradicts the note this header used to carry: +0x04 really
//  is a TSGrowableArray<SJassThread*> (m_alloc/m_count/m_data/m_chunk,
//  Storm/storm.h - its own RTTI allocation tag is `.PAUInstance@@`,
//  Jass2/Instance.h's VM-instance record, kept spelled `SJassThread` here
//  for the reason jassinstancearray.cpp's own header explains), whose
//  m_data field *is* this struct's existing m_threads at +0x0C - it is
//  pre-sized to 128 slots (`SetCount(0x80)`) as a fixed backing store.
//  +0x14 is a genuinely separate field, immediately after the array's own
//  m_chunk (+0x10): a live "how many threads are pushed right now" count,
//  distinct from the backing array's allocated size, which is exactly what
//  JassThreadSleep/JassThreadSyncStart's own `m_threads[m_threadCount - 1]`
//  needs it to be.
//============================================================================
#ifndef JASSTHREADSTATE_H
#define JASSTHREADSTATE_H

#include "CFloat.h"
#include "jasshashtablectors.h"

class JassScript;

//  ??_7JassThreadLocal@@6B@.  JassThreadLocal's own virtual methods are not
//  reconstructed here (out of this session's scope - see CLAUDE.md), so the
//  constructor stamps the shipped address directly rather than declaring
//  `virtual` and letting MSVC emit a table of our own the way
//  Frame/clayer.h's kLayerNodeVftable and its siblings already do.
const unsigned int kJassThreadLocalVftable = 0x6F950EA8u;

//  One JASS thread.  Four adjacent words, all four written by these two
//  functions and by nothing else in this call tree; the names come from the
//  natives that reach them ("TriggerSleepAction", "TriggerSyncStart").
//
//  jassvm-sweep-K1's own addition: the same four fields' own out-of-line
//  accessor/mutator bodies, at 0x6F448A80-0x6F448AD0 - a separate module
//  neighbourhood from jassthreadstate.cpp's 0x6F44B2xx (JassThreadSleep
//  inlines the writes into the *running* thread directly rather than
//  calling these), reached with no static call xref in either direction -
//  almost certainly bound as native-callback/table entries rather than
//  reached by a plain `call`, the same way this whole address range's own
//  natives are reached through SJassNativeFuncTable.
struct SJassThread
{
    char   m_reserved00[0x34];
    int    m_sleeping;        // +0x34 - set to 1 by TriggerSleepAction
    int    m_sleepKind;       // +0x38 - the native always passes 0
    int    m_syncStart;       // +0x3C - set to 1 by TriggerSyncStart
    CFloat m_sleepSeconds;    // +0x40

    //  0x6F448A80 - `retn 8`.  Sets all three sleep fields at once.
    void Sleep(const CFloat* seconds, int kind);
    //  0x6F448AA0 - `retn 0`.
    int IsSleeping() const { return m_sleeping; }
    //  0x6F448AB0 - `retn 0`.
    void SyncStart() { m_syncStart = 1; }
    //  0x6F448AC0 - `retn 4`.  CFloat returned by value -> hidden pointer,
    //  same idiom as SPlayerScore::GetUpgradeRatio (playerscore_forward.cpp).
    CFloat GetSleepSeconds() const { return m_sleepSeconds; }
    //  0x6F448AD0 - `retn 0`.
    int GetSleepKind() const { return m_sleepKind; }
};

//  A NativeFunc* or JassScript* array's own TSGrowableArray header
//  (Storm/storm.h field order: m_alloc, m_count, m_data, m_chunk) is not
//  reusable by value here without pulling every instantiation's own .cpp
//  into this header, so JassThreadLocal spells its four members out by
//  hand at the matching offsets instead.
struct NativeFunc;

//  Forward-declared rather than pulled in: jassinstance.h already includes
//  this header (for kThreadLocalJass), so the reverse include would be
//  circular.  Nothing here needs more than the pointer.  jassvm-batch-3's
//  own slice of the call tree reaches this same struct's +0x88 handle
//  array as SJassThread** already does above - the RTTI tag on the array's
//  own allocation (`.PAUInstance@@`) says the two names are the one real
//  Jass2/Instance.h type, so JassInstance here is not a second guess at the
//  layout, just this session's own name for objects it reaches by that
//  array rather than by the array itself.
class JassInstance;

struct JassThreadLocal
{
    JassThreadLocal();   // 0x6F453930
    //  0x6F455820 - tears down every member the constructor above builds.
    //  Kept a naked thunk: its own prologue (`push -1 / push offset
    //  SEH_6F455820 / mov eax,fs:0`, the /GS cookie XOR right after) is the
    //  same `__except_handler4`-framed SEH shape this fixed `/GS- /EHs-c-`
    //  toolchain cannot emit anywhere else in this repo (JassInstance::
    //  Construct/Run and friends - docs/targets/JassVM.md).
    ~JassThreadLocal();
    //  0x6F455A10 - `ScalarDeletingDestructor(unsigned int flags)`, the
    //  same shape Pathfinding/pathclient_scalardtor.cpp already
    //  reconstructs: real destructor, then `SMemFree` when bit 0 of
    //  `flags` is set.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  0x6F454E30 - resolve-or-create the NativeFunc record for `name` in
    //  this thread state, handing its address back through `*outHandle`.
    //  This is the same shipped address as JASSVARSCOPE::Declare
    //  (src/Jass/jassdeclare.cpp) - see jassregisternative.cpp's own note
    //  for why. jassregisternative.cpp's ResolveNativeFuncRecord calls that
    //  real definition directly (reinterpreting `this` as a JASSVARSCOPE*)
    //  rather than through a member of this class, so there is no
    //  declaration for it here any more.

    //  0x6F449BC0 - `((void**)m_scripts)[index]`.  `retn 4`.
    void* GetScript(unsigned int index);
    //  0x6F449BD0 - `m_handleArrayData[index]`.  `retn 4`, the same
    //  one-stack-argument accessor shape as GetScript() just above,
    //  against the +0x90 array instead of the +0x48 one.
    JassInstance* GetInstanceAtSlot(unsigned int index);
    //  0x6F44D8D0 - park `obj` at `index` of the +0x88 handle array (or
    //  auto-assign the next free one off the +0x98 free-slot stack, or by
    //  growing the array, when `index` is 0), and stamp `obj`'s own
    //  +0x2850 with whichever index it landed at.  `retn 8`; the assigned
    //  (or already-held) index, in eax, though every caller in this
    //  session's own closure discards it.
    unsigned int AssignInstanceSlot(JassInstance* obj, unsigned int index);

    // +0x00 - ??_7JassThreadLocal@@6B@
    unsigned int  m_threadAlloc;   // +0x04 - TSGrowableArray<SJassThread*>::m_alloc
    unsigned int  m_threadArrayN;  // +0x08 - ::m_count (allocated/constructed slots, not the live count)
    SJassThread** m_threads;       // +0x0C - ::m_data
    unsigned int  m_threadChunk;   // +0x10 - ::m_chunk
    unsigned int  m_threadCount;   // +0x14 - live pushed-thread count (see header note)
    //  TSHashTable<NativeFunc,HASHKEY_STR> - jasshashtablectors.h.
    SJassNativeFuncTable m_natives;         // +0x18, 0x28 bytes
    unsigned int  m_scriptAlloc;   // +0x40 - TSGrowableArray<JassScript*>::m_alloc
    unsigned int  m_scriptArrayN;  // +0x44 - ::m_count
    void*         m_scripts;       // +0x48 - ::m_data (JassScript* array; JassScript not reconstructed here)
    unsigned int  m_scriptChunk;   // +0x4C - ::m_chunk
    //  +0x50..+0x5F: untouched gap ahead of the next hash table.
    char          m_reserved50[0x60 - 0x50];
    //  TSHashTable<Script2HandleReg,HASHKEY_NONE> - jasshashtablectors.h.
    SJassScript2HandleTable m_script2Handle; // +0x60, 0x28 bytes
    unsigned int  m_handleArrayAlloc; // +0x88 - TSGrowableArray<SJassThread*>::m_alloc (same instantiation as m_threads)
    unsigned int  m_handleArrayN;     // +0x8C - ::m_count
    JassInstance** m_handleArrayData; // +0x90 - ::m_data (SJassThread**/JassInstance**, the one real type - see the class note above)
    unsigned int  m_handleArrayChunk; // +0x94 - ::m_chunk
    //  +0x98/+0x9C/+0xA0/+0xA4 - jassvm-batch-3's own AssignInstanceSlot
    //  (jassthreadinstanceslot.cpp) resolves these four of the constructor's
    //  five individually-zeroed words as a second TSGrowableArray<void*> -
    //  a free-slot index stack over the +0x88 handle array - which is
    //  consistent with, not a contradiction of, "zeroed individually
    //  rather than through one SetCount call": an empty TSGrowableArray
    //  *is* four zeroed words.  +0xA8 stays unaccounted for either way.
    unsigned int  m_freeSlotAlloc;       // +0x98
    unsigned int  m_freeSlotCount;       // +0x9C
    unsigned int* m_freeSlotData;        // +0xA0
    unsigned int  m_freeSlotChunk;       // +0xA4
    unsigned int  m_fieldA8;             // +0xA8
};

//  Slot 5 of this thread's pointer array.
const unsigned int kThreadLocalJass = 0x05;

//  0x6F44B2F0 - park the running thread for `*seconds`.
void __fastcall JassThreadSleep(const CFloat* seconds, int kind);
//  0x6F44B3A0 - mark the running thread as starting a sync block.
void __fastcall JassThreadSyncStart();

//  0x6F449CE0 - `retn 0`, no arguments (not even __fastcall's ecx) -
//  decrement the running thread's own live-pushed count.  jassthreadpop.cpp.
void JassThreadPop();

//  0x6F449CF0 - `GetThreadLocalSlot(5)->GetScript(index)`, `retn 0`
//  (__fastcall, the index arrives in ecx and nothing is pushed).
void* __fastcall JassThreadGetScript(unsigned int index);

//  0x6F44D960 - `GetThreadLocalSlot(5)->AssignInstanceSlot(obj, index)` -
//  JassThreadLocal::AssignInstanceSlot (declared on the class itself,
//  above) against the *running* thread.  `retn 0` (__fastcall, both
//  arguments in registers).  Returns `unsigned int` (AssignInstanceSlot's
//  own assigned-index return), not `void` - jassthreadinstanceslot.cpp's
//  own comment already knew this called `AssignInstanceSlot`, which does
//  return one, but nothing in that session's own closure had a caller that
//  read `eax` back out of this wrapper to notice the header had it wrong.
//  0x6F44D9B0 (JassThreadSpawnChildInstance, jassinstancespawn.cpp) does:
//  it stores this call's own `eax` into the freshly spawned child's own
//  `+0x2850`, which is exactly `AssignInstanceSlot`'s "auto-assign" path
//  from `jassinstancebigthunks.cpp`'s own note - "the auto-assign arm
//  leaves it to its own caller" to stamp `+0x2850` itself, and this is that
//  caller.
unsigned int __fastcall JassThreadRegisterInstance(JassInstance* obj, unsigned int index);

#endif
