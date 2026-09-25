//============================================================================
//  0x6F49D5F0 - NIpse::CLrPath::Load, vtable slot 10.  CLrPath::Save's own
//  counterpart (pathclient_save.cpp) - see that file for why the two
//  out-of-scope leaves below are real naked-thunk redirects rather than
//  raw-address function-pointer casts (a cast compiles to an indirect call
//  through a register loaded with the literal address in this build, not
//  the dump's own direct `call`):
//    - 0x6F49C850 - LoadPointCache (fastcall: store in ecx, cache pointer
//      in edx, returns the store fluently) - THUNK, not reconstructed;
//    - 0x6F4AF0C0 - ReadLaneLinkEntry (fastcall: store in ecx only,
//      returns the pointer or 0) - THUNK, not reconstructed.
//
//  The goal field is read fresh only when the store's own vtable slot 5
//  (offset 0x14) reports a value >= 0x178A - a hand-written indirect
//  dispatch (CLAUDE.md's own `vtable_dispatch_audit.py` subject) rather
//  than a new virtual added to the shared Net/cdatastore.h, since that
//  class's own vtable shape is depended on well beyond this one call site
//  and its real slot-5 override is not known.  Below that threshold the
//  target point is simply republished as the goal instead.
//============================================================================
#include "pathmove.h"
#include "spatialgrid.h"
#include "cdatastore.h"
#include "CFloat.h"

typedef int (__thiscall *StoreSlot5Fn)(CDataStore*);

//  0x6F49C850 - THUNK, not reconstructed this session.
__declspec(naked) CDataStore* __fastcall LoadPointCache(CDataStore*, SPathPointCache*)
{
    __asm
    {
        mov eax, 06F49C850h
        jmp eax
    }
}

//  0x6F4AF0C0 - THUNK, not reconstructed this session.
__declspec(naked) void* __fastcall ReadLaneLinkEntry(CDataStore*)
{
    __asm
    {
        mov eax, 06F4AF0C0h
        jmp eax
    }
}

//  0x6F6EED50 - read one CFloat, Net/cdatastore_readcfloat.cpp.
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//  0x6F4AF090 - read one pathing-reference entry back, already
//  reconstructed as a redirected thiscall member, Pathfinding/pathref_load.cpp.
struct SPathRefReader
{
    void* ReadEntry();
};

void CPathClient::Load(CDataStore* store)
{
    ReadCFloat(store, &m_position.m_x);
    ReadCFloat(store, &m_position.m_y);
    ReadCFloat(store, &m_target.m_x);
    ReadCFloat(store, &m_target.m_y);

    int slot5 = ((StoreSlot5Fn)(*(void***)store)[5])(store);
    if (slot5 >= 0x178A)
    {
        ReadCFloat(store, &m_goal.m_x);
        ReadCFloat(store, &m_goal.m_y);
    }
    else
    {
        m_goal = m_target;
    }

    store = store->ReadDwordAlt3((unsigned int*)&m_flags);
    store = store->ReadDword(&m_cooldown);
    store = store->ReadDword((unsigned int*)&m_retries);
    store = store->ReadDword(&m_bestHandle);
    store = store->ReadDword((unsigned int*)&m_bestTypeTag);
    store = store->ReadDwordAlt3((unsigned int*)m_reservedB0);
    ReadCFloat(store, &m_radius);

    unsigned int typeMask = 0;
    store = store->ReadDword(&typeMask);
    m_typeMask = (int)typeMask;

    SPathPointCache* cachePtr = &m_cache[0];
    unsigned int* cursorPtr = &m_cursor[0];
    unsigned short* classPtr = &m_sizeClass;
    unsigned int count = 2;
    do
    {
        store = LoadPointCache(store, cachePtr);
        store = store->ReadDword(cursorPtr);
        store = store->ReadDword(cursorPtr + 2);
        store = store->ReadWord(classPtr);
        cachePtr = (SPathPointCache*)((char*)cachePtr + 0x20);
        cursorPtr = cursorPtr + 1;
        classPtr = classPtr + 1;
    } while (--count);

    m_lanePrev = (CPathClient*)ReadLaneLinkEntry(store);
    m_laneNext = (CPathClient*)ReadLaneLinkEntry(store);
    m_ignore = (CGridRegistration*)((SPathRefReader*)store)->ReadEntry();
    m_targetNode = (CGridRegistration*)((SPathRefReader*)store)->ReadEntry();
}
