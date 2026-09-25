//============================================================================
//  CNetObserver::CNetObserver() / ::~CNetObserver() - 0x6F548EA0 / 0x6F545040.
//  See netobserver.h for the class layout this reconstructs field-for-field.
//============================================================================
#include "netobserver.h"
#include "fixedpoolheader.h"
#include "dataallocator.h"
#include "crtseh.h"
#include "storm.h"   // placement operator new(unsigned int, void*)

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  0x6F538390 - trivial: zero the one dword this slot owns and hand `this`
//  back.  Reconstructed for real rather than thunked (nothing left to
//  guess at - see netobserver.h's own field comment).
void __fastcall CNetObserverSlotElemCtor(void* self)
{
    *(int*)self = 0;
}

//  0x6F53F900 - the per-slot destructor: if the slot holds a non-null
//  pointer, walk the same self-linked-sentinel list ~CNetObserver()'s own
//  drain loop below walks, unlinking (but not posting/deleting/freeing)
//  whatever it finds.  By the time this runs (the `eh vector destructor
//  iterator` pass after that hand-written drain), every slot is already
//  back to null and this body never does anything live - left a naked
//  thunk since there is nothing left in this closure to cross-check a
//  from-scratch transcription against.
ADDR_THUNK(void __fastcall CNetObserverSlotElemDtor(void*), 0x6F53F900)

//  The RTTI type-descriptor-name string ~CNetObserver()'s own
//  ReleaseAllBlocks() call tags its pool release with (0x6FA89D6C) -
//  see CallbackObserver's own comment in netobserver.h.
static const char kCallbackObserverTag[] = ".?AVCallbackObserver@CNetObserver@@";

CNetObserver::CNetObserver()
{
    //  ecx = this throughout.  CNetObserver's own vtable stamp is now
    //  implicit (see netobserver.h - this class has a real virtual now,
    //  DeleteSelf); the two CObserver members are placement-constructed
    //  exactly like a real embedded-by-value member would be.
    new (m_observer1) CObserver();
    new (m_observer2) CObserver();

    //  0x6F4C1A10 - SFixedPoolHeader(elementSize=0x1C, capacity=8).
    new (m_pool) SFixedPoolHeader(0x1C, 8);

    m_slotCapacity = 0x40;

    //  eh vector constructor iterator(array, elementSize=4, count=0x40,
    //  ctor, dtor) - see netobserver.h's own field comment for how the
    //  (elementSize, count) pair was pinned down from the exact push
    //  order at the real call site, rather than assumed.
    EhVectorConstructorIterator(
        &m_slots[0], 4, 0x40,
        (void (__thiscall*)(void*))CNetObserverSlotElemCtor,
        (void (__thiscall*)(void*))CNetObserverSlotElemDtor);
}

//----------------------------------------------------------------------------
//  0x6F545040.  Drain every one of the 64 slots by hand (walking backward,
//  matching the shipped body's own iteration order): each slot either is
//  null or heads a singly-linked (next-only, own +0x0C) list of
//  CallbackObserver nodes, self-linked back to the slot's own sentinel
//  once empty.  For every node removed: post its own event id (+0x10) to
//  m_observer1, delete it through its own vtable (DeleteSelf(0) -
//  CallbackObserver's own override never actually frees the memory for
//  flags=0, see netobservercallback_deleteself.cpp), then return the node
//  to m_pool.
//
//  Once every slot is empty, release the pool's own backing blocks, run
//  the array's own (now-inert) `eh vector destructor iterator` pass, tear
//  down the pool header itself (a no-op body, see fixedpoolheaderdtor.cpp),
//  then destroy m_observer2 and m_observer1 in that order - the reverse of
//  how the constructor built them.
//
//  Same SEH-frame-cannot-reproduce ceiling as the constructor (see
//  netobserver.cpp's own history in
//  docs/notes/checksum-provider-registry.md) - this build's fixed
//  `/GS- /EHs-c-` never emits the `push -1`/`push offset SEH_<addr>`
//  prologue/epilogue the shipped body carries, so the surviving diff is
//  frame furniture, not a behavioural gap.
//----------------------------------------------------------------------------
CNetObserver::~CNetObserver()
{
    unsigned int* slot = &m_slots[0x40];
    for (unsigned int remaining = 0x40; remaining != 0; --remaining)
    {
        --slot;
        while (*slot != 0)
        {
            CObserver* head = (CObserver*)*slot;
            CObserver* node = *(CObserver**)((char*)head + 0xC);
            *(void**)((char*)head + 0xC) = *(void**)((char*)node + 0xC);
            if (node == head)
                *slot = 0;
            *(void**)((char*)node + 0xC) = 0;

            ((CObserver*)m_observer1)->PostEvent(
                *(unsigned int*)((char*)node + 0x10), node);
            node->DeleteSelf(0);
            ((CDataAllocator*)m_pool)->Free(node, 0, 0);
        }
    }

    ((CDataAllocator*)m_pool)->ReleaseAllBlocks(kCallbackObserverTag, -2, 1);

    EhVectorDestructorIterator(
        &m_slots[0], 4, 0x40,
        (void (__thiscall*)(void*))CNetObserverSlotElemDtor);

    ((SFixedPoolHeader*)m_pool)->~SFixedPoolHeader();
    ((CObserver*)m_observer2)->~CObserver();
    ((CObserver*)m_observer1)->~CObserver();
}
