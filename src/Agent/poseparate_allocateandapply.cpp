//============================================================================
//  0x6F495C70 - NIpse::CPoSeparate_AllocateAndOptionallyApply. See
//  poseparate_pool.h - own translation unit so it stays a real `call`
//  into SPoSeparatePool::StampNew rather than being inlined into it.
//============================================================================
#include "poseparate_pool.h"
#include "agentbasechain_staticpools.h"

namespace NIpse {

CPoSeparate* __fastcall CPoSeparate_AllocateAndOptionallyApply(CPoSeparate** slot,
                                                                unsigned int applyArg,
                                                                int create)
{
    SPoSeparatePool* pool = (SPoSeparatePool*)CPoSeparate_GetStaticPool();
    CPoSeparate* obj = pool->StampNew(0, 0);
    *slot = obj;

    if (create != 0)
    {
        //  A genuine indirect vtable-slot-3 dispatch in the shipped
        //  stream (`mov edx,[eax]; mov eax,[edx+0xC]; call eax`), not a
        //  direct call into CPoSeparate::Apply - this object's own
        //  dynamic type is not provably CPoSeparate to the shipped
        //  compiler at this call site the way it is to ours (a freshly
        //  allocated, hand-stamped `void*` in the original source).
        typedef void (__thiscall *ApplyFn)(void*, unsigned int);
        void* vtbl = *(void**)obj;
        ApplyFn fn = (ApplyFn)*(void**)((char*)vtbl + 0xC);
        fn(obj, applyArg);
    }

    return obj;
}

}  // namespace NIpse
