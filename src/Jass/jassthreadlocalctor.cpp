//============================================================================
//  0x6F453930 - JassThreadLocal::JassThreadLocal().  Stamps the class's own
//  vftable, zeroes every member this constructor touches, constructs the
//  two TSHashTable members through their redirected Construct() (see
//  jasshashtablectors.h for why those stay redirects), then reserves the
//  two SJassThread*-array members - +0x04 to 128 slots, +0x88 to 1 - and
//  the JassScript*-array member at +0x40 to 1 slot, nulling each array's
//  first element right after.
//
//  Each reserve is TSGrowableArray<T>::SetCount(n)'s own body
//  (tsarray.inl) written out by hand rather than called: SetCount itself is
//  never a standalone symbol in the shipped stream, only its two callees
//  (ComputeChunk/SetAlloc) are, which is what a small template member
//  looks like once /Ob2 inlines its caller - and for a pointer T the
//  "construct every new slot" loop SetCount's body also has emits nothing
//  (a bare placement `new (&m_data[i]) T` on a pointer default-initialises
//  to garbage, not zero, so the compiler drops the whole loop), which is
//  exactly the shape below skips by never writing it.
//
//  The +0x88 array is reserved to 1 slot and has its element 0 nulled
//  *twice* in the shipped stream - once immediately, once again after the
//  +0x40 (JassScript*) array's own reserve, with nothing else touching it
//  in between.  Transcribed exactly as it appears: nothing in this dump
//  explains why, and "fix" here means guessing, not fixing.
//============================================================================
#include "jassthreadstate.h"
#include "storm.h"

//  The four-word TSGrowableArray<SJassThread*> header lives inline in
//  JassThreadLocal (jassthreadstate.h's own note); reinterpret the four
//  contiguous fields as the real template type to reach its already
//  out-of-line ComputeChunk/SetAlloc (jassinstancearray.cpp).
typedef TSGrowableArray<SJassThread*> SJassThreadArray;
typedef TSGrowableArray<JassScript*>  SJassScriptArray;

//  SetCount(n)'s own body (tsarray.inl) for a pointer element type, where
//  the per-slot construction loop is empty.
template <class ARRAY>
static void ReserveArray(ARRAY& array, unsigned int count)
{
    if (count > array.m_alloc)
    {
        unsigned int chunk = array.m_chunk;
        if (chunk == 0)
            chunk = array.ComputeChunk(count);

        unsigned int alloc = count;
        unsigned int over = count % chunk;
        if (over != 0)
            alloc = count + (chunk - over);

        array.SetAlloc(alloc);
    }

    array.m_count = count;
}

JassThreadLocal::JassThreadLocal()
{
    *(void**)this = (void*)kJassThreadLocalVftable;

    SJassThreadArray& threads = reinterpret_cast<SJassThreadArray&>(m_threadAlloc);
    threads.m_alloc = 0;
    threads.m_count = 0;
    threads.m_data  = 0;
    threads.m_chunk = 0;
    m_threadCount = 0;

    m_natives.Construct();

    SJassScriptArray& scripts = reinterpret_cast<SJassScriptArray&>(m_scriptAlloc);
    scripts.m_alloc = 0;
    scripts.m_count = 0;
    scripts.m_data  = 0;
    scripts.m_chunk = 0;

    m_script2Handle.Construct();

    SJassThreadArray& handleArray = reinterpret_cast<SJassThreadArray&>(m_handleArrayAlloc);
    handleArray.m_alloc = 0;
    handleArray.m_count = 0;
    handleArray.m_data  = 0;
    handleArray.m_chunk = 0;
    m_freeSlotAlloc = 0;
    m_freeSlotCount = 0;
    m_freeSlotData = 0;
    m_freeSlotChunk = 0;
    m_fieldA8 = 0;

    ReserveArray(threads, 0x80);
    m_threads[0] = 0;

    ReserveArray(handleArray, 1);
    m_handleArrayData[0] = 0;

    ReserveArray(scripts, 1);
    scripts.m_data[0] = 0;

    ReserveArray(handleArray, 1);
    m_handleArrayData[0] = 0;
}

//----------------------------------------------------------------------------
//  0x6F455820 - see the header's own note: kept a naked thunk of its own
//  because of its `__except_handler4`-framed SEH prologue.
//----------------------------------------------------------------------------
__declspec(naked) JassThreadLocal::~JassThreadLocal()
{
    __asm
    {
        mov eax, 06F455820h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F455A10 - real destructor call, then free `this` when bit 0 of
//  `flags` is set.
//----------------------------------------------------------------------------
void* JassThreadLocal::ScalarDeletingDestructor(unsigned int flags)
{
    this->~JassThreadLocal();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}
