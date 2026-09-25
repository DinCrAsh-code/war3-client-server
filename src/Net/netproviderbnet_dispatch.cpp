//============================================================================
//  Net::NetProviderBNET - vtable slots 40 (0x6F65E610) and 41
//  (0x6F65E5F0).  Both forward into already-reconstructed base-class
//  bodies once a small piece of their own logic has run.
//============================================================================

//  netprovider_assert.cpp - AssertSlot40's real body already covers this
//  exact address (0x6F65B8D0), reached from the base class's own vtable
//  slot 40 there and directly by BNET's override here.
namespace Net
{
class NetProvider
{
public:
    void AssertSlot40(int a0, unsigned char a4, const void* record, const void* bigStruct, int a16);
};
}

//  0x6F68A600 - `__fastcall(ecx=handle, edx=aC)` plus one stack pointer,
//  `retn 4`. Thunked - below this session's own scope.
extern "C" __declspec(naked) void __fastcall
NetProviderBnetSlot40Sub(void* handle, void* aC, void* outPtr)
{
    __asm { mov eax, 0x6F68A600 }
    __asm { jmp eax }
}

void __fastcall EnterCritSec(struct _RTL_CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(struct _RTL_CRITICAL_SECTION* cs);

//  0x6F65B870 / 0x6F65AC40 - same callees netproviderbnet_descriptorforward.cpp
//  and netproviderbnet_recv.cpp already declare; reused by local alias
//  classes (their own real definitions live in those files) rather than
//  re-defined here.
class NetProviderBnetDescriptorForward
{
public:
    void Forward(int code, void* dataPtr);
};
class NetProviderBnetRecvFail
{
public:
    void Fail(int a0, int a4);
};

//  0x6F68A0C0 - fastcall(handle), plain `retn`. Out of scope for this
//  batch (not one of its own worklist addresses).
extern "C" __declspec(naked) void __fastcall CloseOsHandle_6F68A0C0(void*)
{
    __asm { mov eax, 0x6F68A0C0 }
    __asm { jmp eax }
}

//  0x6F65AC70 - thiscall(this) + one stack arg, retn 4 (the call site
//  never sets edx, only pushes the one stack int - a plain thiscall
//  member, not a fastcall). Out of scope for this batch.
class NetProviderBnetRecvFail2
{
public:
    void Fail(int a0);
};
__declspec(naked) void NetProviderBnetRecvFail2::Fail(int)
{
    __asm { mov eax, 0x6F65AC70 }
    __asm { jmp eax }
}

//  0x6F65E030 - slot 41's own prep call; `this` in ecx, `retn 0`.  Also
//  reached by slot 17's own thunked body (netproviderbnet_session.cpp) -
//  a distinct local alias here, same address.  Real body: under the
//  object's own +0x6D0 critical section, a 7-case switch on
//  (this->0x6E8 - 1) (states 1..7; states 2 and 3 fall straight to the
//  shared cleanup with no work of their own, matching the shipped jump
//  table's own default arm) runs whichever per-state teardown step
//  applies, then unconditionally resets 0x6E8 and 0x7D0 to 0 before
//  leaving the lock.
class NetProviderBnetPrep
{
public:
    void Prep();
};
void NetProviderBnetPrep::Prep()
{
    char* self = (char*)this;
    EnterCritSec((struct _RTL_CRITICAL_SECTION*)(self + 0x6D0));

    switch (*(int*)(self + 0x6E8))
    {
    case 1:
    {
        //  Zero an 0x20-byte scratch buffer and forward it through this
        //  object's own vtable slot 0x98 with a fixed code (2).
        unsigned char scratch[0x20];
        for (unsigned i = 0; i < sizeof(scratch); i += 4)
            *(unsigned int*)(scratch + i) = 0;

        typedef void (__thiscall *VtableSlot98Fn)(void*, unsigned int, void*);
        (*(VtableSlot98Fn**)this)[0x98 / 4](this, 2, scratch);
        break;
    }
    case 4:
        ((NetProviderBnetDescriptorForward*)this)->Forward(2, self + 0x6F0);
        break;
    case 5:
    {
        typedef void (__thiscall *VtableSlot9CFn)(void*, unsigned int, unsigned int, void*);
        (*(VtableSlot9CFn**)this)[0x9C / 4](this, 2, 0, self + 0x6F0);
        break;
    }
    case 6:
        CloseOsHandle_6F68A0C0(*(void**)(self + 0x6A0));
        ((NetProviderBnetRecvFail*)this)->Fail(2, 0);
        break;
    case 7:
        CloseOsHandle_6F68A0C0(*(void**)(self + 0x6A0));
        ((NetProviderBnetRecvFail2*)this)->Fail(2);
        break;
    default:
        break;
    }

    *(int*)(self + 0x6E8) = 0;
    *(int*)(self + 0x7D0) = 0;
    LeaveCritSec((struct _RTL_CRITICAL_SECTION*)(self + 0x6D0));
}

//  0x6F65D670 - NetProvider::Method_0x90's own real body (vtable slot 36,
//  netprovider_method0x90.cpp only reconstructs its small callees, not
//  Method_0x90 itself - a large SEH-cookie-guarded function this session
//  did not attempt). `this` in ecx, three stack ints, `retn 0Ch`. Thunked.
class NetProviderBnetMethod0x90
{
public:
    void Call(int a0, int a4, int a8);
};
__declspec(naked) void NetProviderBnetMethod0x90::Call(int, int, int)
{
    __asm { mov eax, 0x6F65D670 }
    __asm { jmp eax }
}

namespace Net
{

class NetProviderBNET
{
public:
    //  0x6F65E610 - vtable slot 40.  `retn 14h`.
    void Method_40(int a0, int a4, void* a8, void* aC, int a10);

    //  0x6F65E5F0 - vtable slot 41.  `retn 8`.
    void Method_41(int arg0, int arg4);
};

void NetProviderBNET::Method_40(int a0, int a4, void* a8, void* aC, int a10)
{
    if (a0 == 1)
    {
        if ((*(int*)((char*)aC + 0xB4) & 0x1000) == 0)
        {
            void* handle = *(void**)((char*)this + 0x6A0);
            NetProviderBnetSlot40Sub(handle, aC, (char*)aC + 0x20);
        }
    }
    ((Net::NetProvider*)this)->AssertSlot40(a0, (unsigned char)a4, a8, aC, a10);
}

void NetProviderBNET::Method_41(int arg0, int arg4)
{
    ((NetProviderBnetPrep*)this)->Prep();
    ((NetProviderBnetMethod0x90*)this)->Call(arg0, arg4, 0);
}

}  // namespace Net
