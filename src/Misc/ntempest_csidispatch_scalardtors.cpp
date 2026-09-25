//============================================================================
//  NIpse::CSiDispatch<T,U> (RTTI-confirmed via agent_worktrees/classes/
//  after the 2026-09-07 dump_agent_worktrees.py namespace/template regex
//  fix, docs/targets/NTempestNIpse_BaseChain.md) - four own-vtable-slot-0
//  scalar deleting destructors, each a simple two-call wrapper (same
//  ReleaseCMemBlockBase-then-Storm-free shape as this file's siblings)
//  around a distinct per-instantiation inner destructor that is NOT a
//  simple wrapper itself: each inner one carries a real
//  __except_handler4-shaped SEH frame (cookie XOR, `__CxxFrameHandler3`,
//  out-of-line catch funclets) around real teardown work of its own
//  (stamps this class's own vtable, tears down two nested sub-objects at
//  `this+0x04`/`this+0x24` - a `CPriorityQ<CSiRequest<T,U>*,CSiRequest<T,U>>`
//  at `+0x24` - then restamps `NTempest::CEntity`'s own vtable before the
//  shared base teardown) - the same unreproducible SEH-frame shape
//  docs/msvc-vc8-idioms.md already documents for several unrelated
//  functions in this binary, all built at this repo's fixed
//  `/GS- /EHs-c-` toolchain. Left as naked, real-signature thunks
//  (the `SPathRefArrayGrow::SetAlloc`-shaped idiom, Pathfinding/pathref_load.cpp)
//  rather than reconstructed, so the outer wrapper - which IS a plain,
//  reproducible reconstruction - can still call it by a real C++ name.
//============================================================================
#include "cellbuffer.h"
#include "storm.h"

namespace NIpse {

//  0x6F489470 - CSiDispatch<CPrBehavior,long>'s own inner teardown, SEH-framed.
struct CSiDispatch_CPrBehavior_InnerDtor
{
    void Run();
};

__declspec(naked) void CSiDispatch_CPrBehavior_InnerDtor::Run()
{
    __asm
    {
        mov     eax, 06F489470h
        jmp     eax
    }
}

//  0x6F46E3D0 - CSiDispatch<CAgentTimer,ulong>'s own inner teardown, SEH-framed.
struct CSiDispatch_CAgentTimer_InnerDtor
{
    void Run();
};

__declspec(naked) void CSiDispatch_CAgentTimer_InnerDtor::Run()
{
    __asm
    {
        mov     eax, 06F46E3D0h
        jmp     eax
    }
}

//  0x6F4894F0 - CSiDispatch<CProximityMap,ulong>'s own inner teardown, SEH-framed.
struct CSiDispatch_CProximityMap_InnerDtor
{
    void Run();
};

__declspec(naked) void CSiDispatch_CProximityMap_InnerDtor::Run()
{
    __asm
    {
        mov     eax, 06F4894F0h
        jmp     eax
    }
}

//  0x6F4893F0 - CSiDispatch<CPrRelation,long>'s own inner teardown, SEH-framed.
struct CSiDispatch_CPrRelation_InnerDtor
{
    void Run();
};

__declspec(naked) void CSiDispatch_CPrRelation_InnerDtor::Run()
{
    __asm
    {
        mov     eax, 06F4893F0h
        jmp     eax
    }
}

struct CSiDispatch_CPrBehavior
{
    //  0x6F4895A0 - vtable slot 0 of NIpse::CSiDispatch<NIpse::CPrBehavior,long> (0x6F9526D4).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CSiDispatch_CAgentTimer
{
    //  0x6F46E450 - vtable slot 0 of NIpse::CSiDispatch<CAgentTimer,ulong> (0x6F951858).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CSiDispatch_CProximityMap
{
    //  0x6F4895D0 - vtable slot 0 of NIpse::CSiDispatch<NIpse::CProximityMap,ulong> (0x6F9526DC).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CSiDispatch_CPrRelation
{
    //  0x6F489570 - vtable slot 0 of NIpse::CSiDispatch<NIpse::CPrRelation,long> (0x6F9526CC).
    void* ScalarDeletingDestructor(unsigned int flags);
};

void* CSiDispatch_CPrBehavior::ScalarDeletingDestructor(unsigned int flags)
{
    ((CSiDispatch_CPrBehavior_InnerDtor*)this)->Run();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CSiDispatch_CAgentTimer::ScalarDeletingDestructor(unsigned int flags)
{
    ((CSiDispatch_CAgentTimer_InnerDtor*)this)->Run();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CSiDispatch_CProximityMap::ScalarDeletingDestructor(unsigned int flags)
{
    ((CSiDispatch_CProximityMap_InnerDtor*)this)->Run();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CSiDispatch_CPrRelation::ScalarDeletingDestructor(unsigned int flags)
{
    ((CSiDispatch_CPrRelation_InnerDtor*)this)->Run();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
