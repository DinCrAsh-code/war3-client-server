//============================================================================
//  0x6F454750 / 0x6F454420 - JASS VM/compiler core internals (jassvm-core-A).
//  The module's own teardown hub: shuts down the running thread's
//  JassThreadLocal (if any) through its own vtable slot 0 and clears TLS
//  slot 5, then falls into the shared tail every one of the module's ten
//  CDataAllocator pools gets released through - a real inter-function code
//  share in the shipped binary (0x6F454680 sits *before* 0x6F454750's own
//  start, and this build's toolchain does no link-time /OPT:ICF - see
//  cdatastore.h's own note - so this reconstruction cannot reproduce the
//  literal shared jump target, only the identical instruction sequence it
//  jumps into).
//============================================================================
#include "jassthreadstate.h"
#include "gamecontext.h"      // GetThreadLocalSlot/SetThreadLocalSlot
#include "dataallocator.h"
#include "jassframepool.h"    // JassFramePool/g_jassFramePool - shared with
                               // jassscriptframepool.cpp (AcquireScriptFrame,
                               // 0x6F45C0D0), which reaches this same global
                               // one field further, +0x18.

void __fastcall SetThreadLocalSlot(unsigned int slot, void* value);

extern CDataAllocator g_jassNodePool;              // unk_6FAB72A8
extern CDataAllocator g_jassScriptDataPool;         // unk_6FAB7278
extern CDataAllocator g_jassHandlePool;             // unk_6FAB730C
extern CDataAllocator g_jassFuncAddrPool;           // unk_6FAB7320
extern CDataAllocator g_jassFuncAddr2HandlePool;    // unk_6FAB72E4
extern CDataAllocator g_jassFuncAddr2NamePool;      // unk_6FAB72D0
extern CDataAllocator g_jassSymbolPool;             // unk_6FAB7334
extern CDataAllocator g_jassHashedStringPool;       // unk_6FAB7348
extern CDataAllocator g_jassString2HandlePool;      // unk_6FAB72F8
extern CDataAllocator g_jassNativeFuncPool;         // unk_6FAB72BC

//----------------------------------------------------------------------------
//  0x6F452EB0 - unlink one frame off g_jassFramePool's own +0x14 list
//  before it is freed.  Kept a naked redirect: an `__except_handler4`-
//  shaped SEH frame this fixed /GS- /EHs-c- toolchain cannot emit, and
//  generic doubly-linked-list bookkeeping, not JASS VM logic.
//----------------------------------------------------------------------------
struct JassFrameNode
{
    JassFrameNode* m_next;   // +0x00
    int            m_prevOffsetOrHead;   // +0x04 - TSLink's own encoding

    //  0x6F452EB0 - `retn 0`.
    void Unlink();
};

__declspec(naked) void JassFrameNode::Unlink()
{
    __asm
    {
        mov eax, 06F452EB0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F454420 - `retn 0`.  Walk g_jassFramePool's own +0x14 list, unlinking
//  and freeing each frame back to the pool, then release the pool itself.
//  `this` is g_jassFramePool.
//----------------------------------------------------------------------------
static void ReleaseFrameListAndPool(JassFramePool* pool)
{
    JassFrameNode* node = (JassFrameNode*)pool->m_frameListHead;
    while (node != 0)
    {
        node->Unlink();
        pool->Free(node, 0, 0);
        node = (JassFrameNode*)pool->m_frameListHead;
    }

    pool->ReleaseAllBlocks(".\\Frame.cpp", -2, 1);
}

//----------------------------------------------------------------------------
//  0x6F454750 - `retn 0`.  See this file's own header note on the shared
//  tail.
//----------------------------------------------------------------------------
void JassModuleTeardown()
{
    JassThreadLocal* jass = (JassThreadLocal*)GetThreadLocalSlot(kThreadLocalJass);
    if (jass != 0)
    {
        typedef void (__thiscall *VtableSlot0Fn)(void*, int);
        void** vtable = *(void***)jass;
        ((VtableSlot0Fn)vtable[0])(jass, 1);
    }

    SetThreadLocalSlot(kThreadLocalJass, 0);

    //  The shared tail (0x6F454680 in the shipped binary).
    g_jassNodePool.ReleaseAllBlocks(".\\Jass.cpp", 0x28C, 1);
    ReleaseFrameListAndPool(&g_jassFramePool);
    g_jassScriptDataPool.ReleaseAllBlocks("AUScriptData", -2, 1);
    g_jassHandlePool.ReleaseAllBlocks("AUJassHandle", -2, 1);
    g_jassFuncAddrPool.ReleaseAllBlocks("AUFuncAddr", -2, 1);
    g_jassFuncAddr2HandlePool.ReleaseAllBlocks("AUFuncAddr2HandleReg", -2, 1);
    g_jassFuncAddr2NamePool.ReleaseAllBlocks("AUFuncAddr2NameReg", -2, 1);
    g_jassSymbolPool.ReleaseAllBlocks("AUSymbol", -2, 1);
    g_jassHashedStringPool.ReleaseAllBlocks("AUHashedString", -2, 1);
    g_jassString2HandlePool.ReleaseAllBlocks("AUString2HandleReg", -2, 1);
    g_jassNativeFuncPool.ReleaseAllBlocks("AUNativeFunc", -2, 1);
}
