//============================================================================
//  See jassscriptframepool.h.
//============================================================================
#include "jassscriptframepool.h"

//----------------------------------------------------------------------------
//  0x6F45A040 - kept THUNK, see jassscriptframepool.h's own note.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall ReleaseLocalSlot(void* /*slot*/)
{
    __asm
    {
        mov eax, 06F45A040h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F45C0D0 - `retn 4`.  See jassframepool.h's own note on why this is
//  a member (`this` = the pool) rather than a free function.
//----------------------------------------------------------------------------
void* JassFramePool::AcquireFrame(int zeroMemory)
{
    void* cached = m_frameListHead;
    if (cached)
    {
        m_frameListHead = *(void**)cached;
        m_liveFrameCount += 1;
        return (char*)cached + 4;
    }

    //  "AUFrame" - Storm's own default per-class tag when a caller does
    //  not supply one (see dataallocator.h's own Alloc() note); line -2
    //  matches the constant every other Alloc() call in this closure
    //  passes for the same reason.
    void* raw = Alloc(zeroMemory, "AUFrame", -2);
    void* body = (char*)raw + 4;

    //  The shipped code tests `body` (raw+4), not `raw` itself, for
    //  null - equivalent in practice (Alloc() never actually returns
    //  null in this closure; see dataallocator.h's own ConstructCWidget
    //  note for the same never-observed-but-checked shape) and
    //  reproduced here rather than "corrected" to testing raw, since the
    //  point of this reconstruction is the shipped instruction sequence.
    if (body != 0)
    {
        JassScriptFrame* frame = (JassScriptFrame*)body;
        frame->m_next = 0;
        frame->m_prevlink = 0;
        frame->m_localCount = 0x20;
        frame->m_scriptData.Construct();
        //  Construct() (0x6F450450) is a naked redirect to real, unhooked
        //  shipped code (jasshashtablectors2.cpp), so whether it stamps
        //  its own vtable internally is unknown from this reconstruction's
        //  own vantage point - but the dump shows this caller stamping it
        //  again regardless, so that explicit store is reproduced here too
        //  rather than assumed redundant.
        *(void**)&frame->m_scriptData = (void*)kScriptDataTableVftable;
    }

    m_liveFrameCount += 1;
    return body;
}

//  0x6F44CFC0 - still TODO, not attempted this session.  A bare `this`-only
//  view of SScriptDataTable, declared just deep enough to reach it as a
//  real __thiscall member call (one stack argument, a constant 0) rather
//  than a free function - the only way this compiler accepts that calling
//  convention outside a class (JassFuncAddrTableView, jassinstance.h, is
//  the same trick for the same reason).
struct SScriptDataTableDestroyView : public SScriptDataTable
{
    //  0x6F44CFC0 - kept THUNK: a real, 40-plus-instruction function
    //  (walks and releases the table's own bucket list), not attempted
    //  this session - redirected purely so this closure's own caller
    //  links and behaves.
    void Destroy(int zero);
};

__declspec(naked) void SScriptDataTableDestroyView::Destroy(int /*zero*/)
{
    __asm
    {
        mov eax, 06F44CFC0h
        jmp eax
    }
}

//  unk_6FAB7278 - jassmoduleteardown.cpp, the "AUScriptData"-tagged pool.
extern CDataAllocator g_jassScriptDataPool;

//----------------------------------------------------------------------------
//  0x6F45ACE0 - `retn 0`.
//----------------------------------------------------------------------------
void __fastcall ReleaseScriptFrameContents(JassScriptFrame* frame)
{
    ((SScriptDataTableDestroyView*)&frame->m_scriptData)->Destroy(0);

    for (unsigned int i = 0; i < frame->m_localCount; i++)
    {
        void* slot = frame->m_locals[i];
        ReleaseLocalSlot(slot);
        g_jassScriptDataPool.Free(slot, 0, 0);
    }

    frame->m_localCount = 0;
    frame->Unlink();
}
