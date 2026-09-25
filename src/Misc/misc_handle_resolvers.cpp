//============================================================================
//  A batch of small handle-resolve leaves, all reached from the
//  CMissileThunderBolt impact-handler closures.  Same two shapes seen
//  already in itemhandleresolve.cpp/handleref_field54_guarded.cpp:
//  the sentinel-gated {handle,typeTag}-at-a-fixed-offset accessor, and a
//  plain LookupHandle-then-read-a-field forward.  None of these has its
//  owning class otherwise identified in this closure.
//============================================================================
#include "game.h"
#include "itemhandleresolve.h"

//  0x6F2BBA80 - {handle,typeTag} pair at this+0xA0/+0xA4.
struct SHandleRefAt0xA0Owner { void* __thiscall GetField0x54IfSet(); };
void* __thiscall SHandleRefAt0xA0Owner::GetField0x54IfSet()
{
    int typeTag = *(int*)((char*)this + 0xA4);
    unsigned int handle = *(unsigned int*)((char*)this + 0xA0);
    if ((typeTag & (int)handle) == -1)
        return 0;
    return QueryHandleField0x54((SOptionalHandleRef*)((char*)this + 0xA0));
}

//  0x6F2BC9F0 - same shape, {handle,typeTag} pair at this+0x2C/+0x30.
struct SHandleRefAt0x2COwner { void* __thiscall GetField0x54IfSet(); };
void* __thiscall SHandleRefAt0x2COwner::GetField0x54IfSet()
{
    int typeTag = *(int*)((char*)this + 0x30);
    unsigned int handle = *(unsigned int*)((char*)this + 0x2C);
    if ((typeTag & (int)handle) == -1)
        return 0;
    return QueryHandleField0x54((SOptionalHandleRef*)((char*)this + 0x2C));
}

//  0x6F497330 - resolve a {handle,typeTag} pair at this+0x68/+0x6C, return
//  the resolved object directly (a tail call to LookupHandle).
struct SHandleRefAt0x68Owner { void* __thiscall ResolveHandle(); };
void* __thiscall SHandleRefAt0x68Owner::ResolveHandle()
{
    return LookupHandle(*(unsigned int*)((char*)this + 0x68), *(int*)((char*)this + 0x6C));
}

//  0x6F4806C0 - resolve a {handle,typeTag} pair at this+0x8/+0xC, then read
//  the resolved object's own +0x40 field (no null check - trusts the
//  resolve).
struct SHandleRefAt0x08Owner
{
    void* __thiscall ResolveAndGetField0x40();
    //  0x6F476430 - CUnit vtable closure (cunit_agent2_worklist.md).  Same
    //  +0x8/+0xC pair, same trusting no-null-check resolve, but +0xCC
    //  instead of +0x40 and handed out through a caller-supplied pointer
    //  (`retn 4`) rather than returned in eax.
    void __thiscall QueryField0xCC(int* out);
};
void* __thiscall SHandleRefAt0x08Owner::ResolveAndGetField0x40()
{
    void* resolved = LookupHandle(*(unsigned int*)((char*)this + 8), *(int*)((char*)this + 0xC));
    return *(void**)((char*)resolved + 0x40);
}
void __thiscall SHandleRefAt0x08Owner::QueryField0xCC(int* out)
{
    int value = *(int*)((char*)LookupHandle(*(unsigned int*)((char*)this + 8),
                                            *(int*)((char*)this + 0xC)) + 0xCC);
    *out = value;
}

//  0x6F471AB0 - fire a fixed observer event (0x40190065 - the same code
//  the ground-truth pipeline notes record for frame [4]'s global-mechanism
//  fire) through this object's own vtable slot 2, forwarding two stack
//  arguments through unchanged.
typedef void (__thiscall *VTableSlot2FireFn)(void* self, unsigned int code, void* arg1, void* arg2);
struct SVTableSlot2Owner { void __thiscall FireEvent40190065(void* arg1, void* arg2); };
void __thiscall SVTableSlot2Owner::FireEvent40190065(void* arg1, void* arg2)
{
    VTableSlot2FireFn fn = (VTableSlot2FireFn)(*(void***)this)[2];
    fn(this, 0x40190065, arg1, arg2);
}
