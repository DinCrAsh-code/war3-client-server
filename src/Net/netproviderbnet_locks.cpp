#include <windows.h>
//============================================================================
//  Net::NetProviderBNET - vtable slots 7 (0x6F65DE00) and 33 (0x6F65DFE0).
//  Two small critical-section-guarded dispatchers around this class's own
//  +0x6D0 CRITICAL_SECTION (the dtor's own body, sub_6F65E9B0, confirms the
//  class name/vtable symbol and that +0x6D0 is one of two CS members this
//  class owns).
//============================================================================

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  0x6F6587B0 - the real logging/assert sink this Notify (below) forwards
//  into; `code` unused (this function's own SEH-framed, 78-instruction
//  body is out of batch-E-net-misc-1's scope), retn 8 confirmed off its
//  own dump tail (`add esp, 2Ch` / `retn 8`), two plain stdcall stack
//  dwords - `this` is never read in Notify's own body, so this is a free
//  function, not a member.
void __stdcall LogNotify_6F6587B0(int category, int* code);
__declspec(naked) void __stdcall LogNotify_6F6587B0(int, int*)
{
    __asm { mov eax, 0x6F6587B0 }
    __asm { jmp eax }
}

//  0x6F65ABE0 - slot 7's own callee once +0x7D4 flips from 0 to 1 for the
//  first time; `retn 4` (one stack int) even though `this` is declared
//  (the shipped body never reads ecx - copies `code` to a local and
//  forwards it, by address, straight into LogNotify_6F6587B0 above with a
//  fixed category of 9).
class NetProviderBnetFirstTimeNotify
{
public:
    void Notify(int code);
};
void NetProviderBnetFirstTimeNotify::Notify(int code)
{
    int codeCopy = code;
    LogNotify_6F6587B0(9, &codeCopy);
}

namespace Net
{

class NetProviderBNET
{
public:
    //  0x6F65DE00 - vtable slot 7.  Under the +0x6D0 lock, the first call
    //  ever made (guarded by +0x7D4) tells 0x6F65ABE0 "code 1"; every call
    //  after that is a no-op past the lock. `retn 0` - the shipped body
    //  ends in a tail `jmp` straight into LeaveCritSec.
    void Method_7();

    //  0x6F65DFE0 - vtable slot 33.  Under the same +0x6D0 lock, if
    //  +0x7D0 is set, forward three of this call's five stack arguments
    //  (the second and fourth are read into local registers but never
    //  used further - the same "declared but unused" leading/trailing-
    //  argument shape this class's other overrides show elsewhere) through
    //  the inner +0x2B4 interface's own vtable+0x10 slot.
    void Method_33(int arg0, int arg4, int arg8, int argC, int arg10);
};

void NetProviderBNET::Method_7()
{
    EnterCritSec((CRITICAL_SECTION*)((char*)this + 0x6D0));

    int* firstTime = (int*)((char*)this + 0x7D4);
    if (*firstTime == 0)
    {
        *firstTime = 1;
        ((NetProviderBnetFirstTimeNotify*)this)->Notify(1);
    }

    LeaveCritSec((CRITICAL_SECTION*)((char*)this + 0x6D0));
}

void NetProviderBNET::Method_33(int arg0, int arg4, int arg8, int argC, int arg10)
{
    (void)arg8;
    (void)argC;

    EnterCritSec((CRITICAL_SECTION*)((char*)this + 0x6D0));

    if (*(int*)((char*)this + 0x7D0) != 0)
    {
        void* inner = *(void**)((char*)this + 0x2B4);
        typedef void (__thiscall *InnerSlot0x10Fn)(void*, int, int, int);
        InnerSlot0x10Fn fn = (InnerSlot0x10Fn)(*(void***)inner)[0x10 / 4];
        fn(inner, arg0, arg4, arg10);
    }

    LeaveCritSec((CRITICAL_SECTION*)((char*)this + 0x6D0));
}

}  // namespace Net
