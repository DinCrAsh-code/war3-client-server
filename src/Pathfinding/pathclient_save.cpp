//============================================================================
//  0x6F49D450 - NIpse::CLrPath::Save, vtable slot 9.  Serializes every
//  field this class owns, in declaration order, through a
//  CDataStoreScratch.
//
//  Two helper leaves this function reaches are not reconstructed this
//  session, so they are given real naked-thunk redirects (CLAUDE.md's own
//  thunk shape, `mov eax, <addr>; jmp eax`) rather than called through a
//  raw-address function-pointer cast - a cast compiles to an *indirect*
//  call through a register loaded with the literal address in this
//  build (`mov eax, <addr>; call eax`, two instructions where the dump
//  has one direct `call`), which only happened not to matter for
//  Pathfinding/crlagent_ctor.cpp's own out-of-scope call. A real thunk
//  symbol gets a genuine direct `call SYM` instead, matching the dump:
//    - 0x6F49C7E0 - SavePointCache (fastcall: store in ecx, cache pointer
//      in edx, returns the store fluently) - marked THUNK in
//      agent_worktrees, not a reconstruction of that address;
//    - 0x6F4AEF70 - WritePathRefEntryOrSentinel - same fastcall shape as
//      WritePathRefEntry below, plus a `{-2,-2}` sentinel for a -1 pointer
//      value.
//============================================================================
#include "pathmove.h"
#include "spatialgrid.h"
#include "cdatastorescratch.h"
#include "CFloat.h"

//  0x6F49C7E0 - THUNK, not reconstructed this session.
__declspec(naked) CDataStoreScratch* __fastcall SavePointCache(CDataStoreScratch*, SPathPointCache*)
{
    __asm
    {
        mov eax, 06F49C7E0h
        jmp eax
    }
}

//  0x6F4AEF70 - THUNK, not reconstructed this session.
__declspec(naked) CDataStoreScratch* __fastcall WritePathRefEntryOrSentinel(CDataStoreScratch*, const void*)
{
    __asm
    {
        mov eax, 06F4AEF70h
        jmp eax
    }
}

//  0x6F6EFE00 - write one CFloat, Net/cdatastore_writecfloat.cpp.
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
//  0x6F4AEF40 - write one pathing-reference entry, Pathfinding/pathref_write.cpp.
void __fastcall WritePathRefEntry(CDataStoreScratch* store, const void* raw);

void CPathClient::Save(CDataStoreScratch* store)
{
    WriteCFloat(store, &m_position.m_x);
    WriteCFloat(store, &m_position.m_y);
    WriteCFloat(store, &m_target.m_x);
    WriteCFloat(store, &m_target.m_y);
    WriteCFloat(store, &m_goal.m_x);
    WriteCFloat(store, &m_goal.m_y);

    store = store->WriteDwordAlt2(m_flags);
    store = store->WriteDword(m_cooldown);
    store = store->WriteDword(m_retries);
    store = store->WriteDword(m_bestHandle);
    store = store->WriteDword(m_bestTypeTag);
    //  +0xB0 - not otherwise named on this class (pathmove.h's own
    //  m_reservedB0 gap); read/written the same Alt2/Alt3 way m_flags is.
    store = store->WriteDwordAlt2(*(unsigned int*)(m_reservedB0));
    WriteCFloat(store, &m_radius);
    store = store->WriteDword(m_typeMask);

    SPathPointCache* cachePtr = &m_cache[0];
    unsigned int* stampPtr = &m_stamp[0];
    unsigned short* classPtr = &m_sizeClass;
    unsigned int count = 2;
    do
    {
        unsigned int cursorVal = *(stampPtr - 2);
        store = SavePointCache(store, cachePtr);
        store = store->WriteDword(cursorVal);
        store = store->WriteDword(*stampPtr);
        store = store->WriteWord(*classPtr);
        cachePtr = (SPathPointCache*)((char*)cachePtr + 0x20);
        stampPtr = stampPtr + 1;
        classPtr = classPtr + 1;
    } while (--count);

    store = WritePathRefEntryOrSentinel(store, m_lanePrev);
    store = WritePathRefEntryOrSentinel(store, m_laneNext);

    WritePathRefEntry(store,
        (m_ignore != 0 && ((CGridRegistration*)m_ignore)->m_visitStamp != -1)
            ? m_ignore : 0);
    WritePathRefEntry(store,
        (m_targetNode != 0 && ((CGridRegistration*)m_targetNode)->m_visitStamp != -1)
            ? m_targetNode : 0);
}
