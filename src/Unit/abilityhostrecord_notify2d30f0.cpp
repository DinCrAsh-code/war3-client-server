//============================================================================
//  0x6F2D30F0 - SAbilityHostRecord::NotifyResolvedAt0x254 (abilityhostrecord.h).
//
//  Resolve the +0x254 handle-ref pair; if it resolves and m_field40 is
//  set, notify the resolved object through 0x6F2DE1D0 and hand back
//  whatever it returns - not otherwise identified (out of this agent's
//  own worklist slice), so its receiver gets the same minimal,
//  locally-scoped treatment Unit/unit_abilityhostnotify.cpp uses for its
//  own out-of-scope callees.  Three separate tests share the same false
//  exit in the shipped code (a single `xor eax,eax`/`retn`), which needs
//  the explicit shared-label shape docs/msvc-vc8-idioms.md's "Shared
//  exits need goto" documents.
//============================================================================
#include "abilityhostrecord.h"
#include "itemhandleresolve.h"

struct SNotify_6F2DE1D0
{
    void* Notify(void* arg0, void* field40);   // 0x6F2DE1D0, retn 8
};

#define NOTIFY_THUNK(addr)       \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }
__declspec(naked) void* SNotify_6F2DE1D0::Notify(void*, void*) NOTIFY_THUNK(0x6F2DE1D0)
#undef NOTIFY_THUNK

void* SAbilityHostRecord::NotifyResolvedAt0x254(void* arg0)
{
    int typeTag = *(int*)((char*)this + 0x258);
    unsigned int handle = *(unsigned int*)((char*)this + 0x254);
    if ((typeTag & (int)handle) == -1)
        goto notResolved;
    {
        void* resolved = QueryHandleField0x54(
            (SOptionalHandleRef*)((char*)this + 0x254));
        if (resolved == 0)
            goto notResolved;
        if (m_field40 == 0)
            goto notResolved;
        return ((SNotify_6F2DE1D0*)resolved)->Notify(arg0, m_field40);
    }
notResolved:
    return 0;
}
