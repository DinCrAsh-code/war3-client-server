//============================================================================
//  CDataRecycler's four remaining vtable slots (1-3, plus the DeleteSelf
//  override at slot 0) - see cdatarecycler.h's own class-level notes for
//  the class this session confirmed vs. vtable_classes.json's own working
//  name for it ("Net::CDataStoreRecycler") - the real RTTI name is
//  CDataRecycler, already established by cdatarecyclerctor.cpp's own
//  vtable stamp.
//============================================================================
#include "cdatarecycler.h"

void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);
void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename, int logline, unsigned int flags);
void* __stdcall SMemReAlloc(void* ptr, unsigned int amount, const char* logfilename, int logline, unsigned int flags);

//  Truncated the same way aE_1 already is (actionqueuerecord_resize.cpp) -
//  IDA's own listing comment cuts the string short and the full text is
//  not reached by this call tree; the exact bytes make no difference to
//  the score (both sides canonicalise the operand to SYM).
static const char aEDrive1TempBui25[] = "e:\\...";

void* CDataRecycler::DeleteSelf(unsigned int flags)
{
    this->~CDataRecycler();
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

void* CDataRecycler::Alloc(unsigned int amount, unsigned int* outAmount, const char* fname, int line)
{
    void* result;
    if (amount)
    {
        if (fname)
            result = SMemAlloc(amount, fname, line, 0);
        else
            result = SMemAlloc(amount, aEDrive1TempBui25, 0xF7, 0);
    }
    else
    {
        result = 0;
    }
    if (outAmount)
        *outAmount = amount;
    return result;
}

void* CDataRecycler::Realloc(void* ptr, unsigned int amount, unsigned int* outAmount, const char* fname, int line)
{
    void* result;
    if (fname)
        result = SMemReAlloc(ptr, amount, fname, line, 0);
    else
        result = SMemReAlloc(ptr, amount, aEDrive1TempBui25, 0x119, 0);
    if (outAmount)
        *outAmount = amount;
    return result;
}

void CDataRecycler::Free(void* ptr, const char* fname, int line)
{
    if (!ptr)
        return;
    if (fname)
        SMemFree(ptr, fname, line, 0);
    else
        SMemFree(ptr, aEDrive1TempBui25, 0x108, 0);
}

//============================================================================
//  0x6F4C6710/0x6F4C66E0 - the two Treiber-stack primitives
//  (cdatarecycler_stackprims.cpp) every function below drives directly.
//============================================================================
void* __stdcall PopFromStack(void** headPtr, int linkOffset);
void __stdcall PushOntoStack(void** headPtr, void* node, int linkOffset);
long __fastcall InterlockedIncrementAt(volatile long* target);
long __fastcall InterlockedDecrementAt(volatile long* target);

typedef void (__thiscall* CDataRecyclerFreeSlot)(void*, void*, const char*, int);
typedef void* (__thiscall* CDataRecyclerReallocSlot)(void*, void*, unsigned int, unsigned int*, const char*, int);

//----------------------------------------------------------------------------
//  0x6F4C6820 - vtable slot 1, Purge.  Drains m_field10 (the chunk stack,
//  each node's field4 a pointer this object's own vtable slot 4 - Free -
//  knows how to release) node by node, then abandons m_field14 (the
//  free-link-node stack) outright - the nodes threaded there are carved
//  out of the raw SMemAlloc blocks that m_field0C tracks, not separately
//  allocated, so freeing every m_field0C block (the second loop, real
//  `SMemFree(ptr, "delete", -1, 0)` calls) already reclaims them.  See
//  ReleaseChunk below for how a block ends up on m_field0C in the first
//  place.
//----------------------------------------------------------------------------
void CDataRecycler::Purge()
{
    void* node = PopFromStack(&m_field10, 0);
    while (node)
    {
        CDataRecyclerFreeSlot freeSlot = (CDataRecyclerFreeSlot)(*(void***)this)[4];
        freeSlot(this, *((void**)node + 1), 0, 0);
        node = PopFromStack(&m_field10, 0);
    }

    m_field14 = 0;

    node = PopFromStack((void**)&m_field0C, 0);
    while (node)
    {
        SMemFree(node, "delete", -1, 0);
        node = PopFromStack((void**)&m_field0C, 0);
    }
}

//----------------------------------------------------------------------------
//  0x6F4C6890 - pop one node off the chunk stack (m_field10) and hand its
//  two payload words back through the out-parameters, returning the node
//  itself onto the free-link-node stack (m_field14) for reuse.  The
//  trailing two stack parameters are real (retn 0x10) but unread on every
//  path this call tree exercises.
//----------------------------------------------------------------------------
void CDataRecycler::TryPopChunk(void** outPtr, unsigned int* outSize, int, int)
{
    void* node = PopFromStack(&m_field10, 0);
    if (node)
    {
        InterlockedIncrementAt((volatile long*)&m_limit);
        *outPtr = *((void**)node + 1);
        *outSize = *((unsigned int*)node + 2);
        PushOntoStack(&m_field14, node, 0);
    }
    else
    {
        *outPtr = 0;
        *outSize = 0;
    }
}

//----------------------------------------------------------------------------
//  0x6F4C68F0 - NetClient's own receive-chunk release helper
//  (agent-networking-wave4-2026-08-29, netclient_chunkpool.cpp's own
//  AcquireChunk is the other half). Atomically decrement m_limit (the live
//  outstanding-chunk budget); if that goes negative, undo it and hand the
//  chunk straight to this object's own vtable slot 4 (Free) instead of
//  pooling it. Otherwise pop a spare link node off m_field14 (refilling it
//  from one new SMemAlloc'd block via SetupNodeChain, above, when it is
//  empty), fill in the node's two payload words, and push it onto the
//  chunk stack (m_field10) for AcquireChunk to hand back out later.
//----------------------------------------------------------------------------
void CDataRecycler::ReleaseChunk(void* ptr, unsigned int size, const char* fname, int line)
{
    if (InterlockedDecrementAt((volatile long*)&m_limit) < 0)
    {
        InterlockedIncrementAt((volatile long*)&m_limit);
        CDataRecyclerFreeSlot freeSlot = (CDataRecyclerFreeSlot)(*(void***)this)[4];
        freeSlot(this, ptr, fname, line);
        return;
    }

    void* node = PopFromStack(&m_field14, 0);
    while (!node)
    {
        unsigned int allocSize = 4 + m_count * 3 * 4;
        void* block = SMemAlloc(allocSize, ".\\CDataRecycler.cpp", 0x60, 0);
        PushOntoStack((void**)&m_field0C, block, 0);
        SetupNodeChain(block, &m_field14);
        node = PopFromStack(&m_field14, 0);
    }

    *((void**)node + 1) = ptr;
    *((unsigned int*)node + 2) = size;
    PushOntoStack(&m_field10, node, 0);
}
