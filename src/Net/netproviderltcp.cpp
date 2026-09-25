//============================================================================
//  Net::NetProviderLTCP::Method_0xC - vtable+0xC (method_index 3 in
//  user_knowledge.json's vtable dump), 0x6F6639D0. This is the vtable
//  method the session set out to understand: it (re)starts the LAN TCP
//  listener thread, registering ContainsW3GSBytePacketCheck
//  (w3gs_packetcheck.cpp) as the thread's own packet-dispatch callback -
//  the entry point through which every W3GS packet this provider ever sees
//  first reaches game code.
//============================================================================
#include "netproviderltcp.h"

//  w3gs_packetcheck.cpp - the callback this method hands to the listener
//  thread it spawns.  Only its address is taken (below), but the declaration
//  still has to match the definition exactly: it is a C++ function, so a
//  different parameter list mangles to a different symbol and the link fails
//  against a function that is right there in the build.  This used to read
//  `(void*, void*, void*, void*)` and did precisely that.
namespace Net { class NetProviderLTCP; }
int __stdcall ContainsW3GSBytePacketCheck(void* arg0, void* arg4, int selector,
                                          Net::NetProviderLTCP* provider,
                                          void* arg10, int msgId, int* pOutMsgId);

//  threadstarttrampoline.cpp
void __fastcall SpawnListenerThread(int origEcx, int origEdx, int stackSize, void* callback, void* owner);

//  sub_6F65AAF0 - CDataStore-backed status-post helper (push 1; call
//  sub_6F658030), reached only from the "not enabled" branch below. Not
//  reconstructed this session (a separate, self-contained recycled-event
//  write path - see docs/targets/NetProviderLTCP__Method_0xC.md); declared
//  only so this call site compiles.
//  sub_6F65AAF0 itself never sets ecx - it relies on the caller having left
//  `this` in ecx already (Method_0xC's else-branch never touches ecx after
//  `mov esi, ecx`), so it is genuinely a one-register-plus-one-stack-arg
//  (`__thiscall`-shaped) function, not a 2-register `__fastcall` one -
//  modelled the same way threadstarttrampoline.cpp's NetThreadStart is.
struct PostStatusTarget
{
    void PostProviderStatus(int code);
};

namespace Net
{

void NetProviderLTCP::Method_0xC(int unusedArg)
{
    if (m_enabled != 0)
    {
        StopListenerThread();
        m_threadRunning = 1;
        m_threadDoneEvent.Reset();
        SpawnListenerThread(0, 0x17E0, 0x17E0, (void*)&ContainsW3GSBytePacketCheck, this);
        return;
    }

    ((PostStatusTarget*)this)->PostProviderStatus(5);
}

}  // namespace Net
