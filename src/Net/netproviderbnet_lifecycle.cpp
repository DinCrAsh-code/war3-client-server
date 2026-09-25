//============================================================================
//  Net::NetProviderBNET - vtable slots 0 (0x6F65F950, DeleteSelf), 1
//  (0x6F6604D0, Initialize), 2 (0x6F660DE0, Shutdown) and 3 (0x6F660E40).
//  Unlike NetProviderLTCP/NetProviderLOOP's own Initialize/Shutdown (a
//  module-wide critical section plus a shared OsNetInitialize/Shutdown
//  refcount), BNET's own bring-up/tear-down goes through this class's own
//  private +0x6A0 OS-transport handle instead - Battle.net's provider owns
//  its own connection object rather than sharing the LAN/loopback socket
//  layer's global init.
//============================================================================

//  0x6F65E9B0 - ~NetProviderBNET().  Carries the same __except_handler4-
//  shaped SEH frame (`push -1`/`push offset SEH_...`/`large fs:0` prologue)
//  docs/msvc-vc8-idioms.md documents as unreproducible at this build's
//  fixed /GS- /EHs-c-, and its own body confirms the class's vtable symbol
//  (`??_7NetProviderBNET@Net@@6B@`) and that it owns two CRITICAL_SECTIONs
//  (+0x67C and +0x6D0). Left a thunk to the real, unhooked body - the same
//  reasoning netproviderltcp_deleteself.cpp/netproviderloop_deleteself.cpp
//  already give for their own base destructors.
class NetProviderBnetDtor
{
public:
    void Destroy();
};
__declspec(naked) void NetProviderBnetDtor::Destroy()
{
    __asm { mov eax, 0x6F65E9B0 }
    __asm { jmp eax }
}

//  0x6F689490 - Initialize's own OS-transport bring-up: `__fastcall(ecx =
//  configStruct, edx = &this->+0x6A0)`, `retn 0`. Writes the resulting
//  handle/status through the out-pointer and returns the same status in
//  eax. Genuinely deep OS-layer machinery (the 0x6F68xxxx/0x6F69xxxx
//  neighbourhood none of this sweep's 31 slots otherwise reach) - thunked.
extern "C" __declspec(naked) int __fastcall
NetProviderBnetOsInit(void* configStruct, void* outHandle)
{
    __asm { mov eax, 0x6F689490 }
    __asm { jmp eax }
}

//  0x6F6602D0 - real now in netprovider_copyconfig.cpp
//  (Net::CopyProviderConfigTarget::CopyConfig) - one shipped address shared
//  by all three providers' own Initialize; see that file's own header.
namespace Net
{
class CopyProviderConfigTarget
{
public:
    void CopyConfig(void* configStruct);
};
}
typedef Net::CopyProviderConfigTarget NetProviderBnetInitTarget;

//  0x6F65DDB0 - shared by Shutdown and Method_3 below: `this` in ecx,
//  `retn 0`. Thunked - below this session's own scope.
class NetProviderBnetShutdownHelper
{
public:
    void Notify();
};
__declspec(naked) void NetProviderBnetShutdownHelper::Notify()
{
    __asm { mov eax, 0x6F65DDB0 }
    __asm { jmp eax }
}

//  0x6F6894E0 - Shutdown's own OS-transport teardown: `this` in ecx (the
//  address of this->+0x6A0 itself, i.e. an OS-transport object, not
//  NetProviderBNET), `retn 0`. Thunked.
class NetProviderBnetOsHandle
{
public:
    void Teardown();       // 0x6F6894E0
};
__declspec(naked) void NetProviderBnetOsHandle::Teardown()
{
    __asm { mov eax, 0x6F6894E0 }
    __asm { jmp eax }
}
//  0x6F6895A0 - Method_3's own forward into the same +0x6A0 OS-transport
//  object: `__fastcall(ecx=handle, edx=arg)`, `retn 0` (no stack args at
//  all - both the handle and the argument arrive in registers). Thunked.
extern "C" __declspec(naked) void __fastcall
NetProviderBnetMethod3Forward(void* handle, int arg)
{
    __asm { mov eax, 0x6F6895A0 }
    __asm { jmp eax }
}

//  0x6F65BE40 - Net::NetProvider::Method_0x2 ("code 4" through vtable+0x90,
//  already reconstructed - netprovider_slot2.cpp).
namespace Net { class NetProvider { public: int Method_0x2(); }; }

//  0x6F62B390/0x6F62B430 - RegisterEventHandler/UnregisterEventHandler,
//  already reconstructed (src/Frame/eventregister.cpp,
//  src/Frame/eventunregister.cpp).
void __fastcall RegisterEventHandler(int slot, void* handler, int a, float when);
void __fastcall UnregisterEventHandler(int slot, void* handler, void* a, int flags);

//  0x6F65AAF0 - PostStatusTarget::PostProviderStatus, already reconstructed
//  in src/Misc/unreconstructed_thunks.cpp (a naked thunk there too).
class PostStatusTarget
{
public:
    void PostProviderStatus(int);
};

//  0x6F660DA0 - the periodic-timer callback both Method_3's registration
//  and Shutdown's cancellation name by address (`offset sub_6F660DA0`),
//  never called directly from this call tree.
//  Taken by address only (never called from this call tree), so a raw
//  address literal is used rather than an extern declaration with nothing
//  to define it (link_check.py's own "not a data global ... nothing on
//  the real link line defines it either" bucket) - the same
//  `(void*)0x6F...` idiom src/GameUI/gameuicursorreset.cpp already uses
//  for a shipped-data-literal address.
#define NetProviderBnetPeriodicCallback ((void*)0x6F660DA0)

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

namespace Net
{

class NetProviderBNET
{
public:
    //  0x6F65F950 - vtable slot 0.  The classic MSVC "scalar deleting
    //  destructor".
    void* DeleteSelf(unsigned int flags);

    //  0x6F6604D0 - vtable slot 1.
    unsigned int Initialize(void* configStruct, int* outStatus);

    //  0x6F660DE0 - vtable slot 2.
    void Shutdown();

    //  0x6F660E40 - vtable slot 3.
    void Method_3(int arg0);
};

void* NetProviderBNET::DeleteSelf(unsigned int flags)
{
    ((NetProviderBnetDtor*)this)->Destroy();
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

unsigned int NetProviderBNET::Initialize(void* configStruct, int* outStatus)
{
    *outStatus = 0;

    if (*(int*)((char*)this + 0x698) != 0)
        return 6;

    int result = NetProviderBnetOsInit(configStruct, (char*)this + 0x6A0);
    if (result == 1)
    {
        ((NetProviderBnetInitTarget*)((char*)this + 0x18))->CopyConfig(configStruct);
        *(int*)((char*)this + 0x698) = result;
        *outStatus = *(int*)((char*)this + 0x6A0);
    }
    return result;
}

//  0x6F660DE0 - vtable slot 2.  `retn 0`; no callers in this call tree
//  observe a return value, so the shipped `eax = 5`-on-already-down path
//  is preserved as dead-store-free control flow only.
void NetProviderBNET::Shutdown()
{
    ((Net::NetProvider*)this)->Method_0x2();
    ((NetProviderBnetShutdownHelper*)this)->Notify();

    if (*(int*)((char*)this + 0x698) != 0)
    {
        if (*(int*)((char*)this + 0x69C) != 0)
        {
            UnregisterEventHandler(0x19, (void*)NetProviderBnetPeriodicCallback,
                                   (void*)-1, 0);
            *(int*)((char*)this + 0x69C) = 0;
        }
        //  `lea ecx,[esi+6A0h]` - the address of the +0x6A0 slot itself,
        //  not the pointer value stored there (Method_3's own +0x6A0 use
        //  below dereferences it instead - two different call shapes at
        //  the same offset, reproduced exactly as the dump shows each).
        ((NetProviderBnetOsHandle*)((char*)this + 0x6A0))->Teardown();
        *(int*)((char*)this + 0x698) = 0;
    }
}

void NetProviderBNET::Method_3(int arg0)
{
    if (*(int*)((char*)this + 0x698) != 0)
    {
        if (*(int*)((char*)this + 0x69C) == 0)
        {
            RegisterEventHandler(0x19, (void*)NetProviderBnetPeriodicCallback, 0, 0.0f);
            *(int*)((char*)this + 0x69C) = 1;
        }
        ((NetProviderBnetShutdownHelper*)this)->Notify();
        //  `mov ecx,[esi+6A0h]` - dereferences the +0x6A0 slot (the OS
        //  handle value Initialize's own out-pointer wrote), unlike
        //  Shutdown's own `lea` above.
        void* osHandle = *(void**)((char*)this + 0x6A0);
        NetProviderBnetMethod3Forward(osHandle, arg0);
    }
    else
    {
        ((PostStatusTarget*)this)->PostProviderStatus(5);
    }
}

}  // namespace Net
