//============================================================================
//  Net::NetProviderLOOP - vtable slots 10 (0x6F660F00), 11 (0x6F660F10),
//  12 (0x6F660F30), 13 (0x6F660F40) and 41 (0x6F660F90) - one address
//  neighbourhood, the same module.  Slots 10/11/12/13 all overwrite `ecx`
//  from their own stack argument before doing anything, so `this` is
//  provably unread in every one of them; each is a thin __thiscall wrapper
//  around a free function from sockaddrinet.cpp.
//============================================================================
#include "netproviderloop.h"

int __fastcall AddrFieldsEqual(const void* a, const void* b, int* outFlag);   // sockaddrinet.cpp
unsigned int __fastcall GetAddrIPAndPort(const void* addr, unsigned short* outPort); // sockaddrinet.cpp
void __fastcall BuildSockAddr(unsigned int addr, unsigned short port, void* out);    // sockaddrinet.cpp

namespace Net
{

//  0x6F660F00 - `mov eax,64h / retn`, no arguments at all.
int NetProviderLOOP::GetFixedCode()
{
    return 0x64;
}

//  0x6F660F10 - forwards straight to AddrFieldsEqual.
int NetProviderLOOP::AddressesEqual(const void* a, const void* b, int* outFlag)
{
    return AddrFieldsEqual(a, b, outFlag);
}

//  0x6F660F30 - forwards to GetAddrIPAndPort with no out-port.
unsigned int NetProviderLOOP::GetAddressValue(const void* addr)
{
    return GetAddrIPAndPort(addr, 0);
}

//  0x6F660F40 - read `srcAddr`'s raw address field, then build a fresh
//  struct at `out` from it and `port`.
void NetProviderLOOP::BuildAddress(void* out, const void* srcAddr, unsigned short port)
{
    unsigned int addrValue = GetAddrIPAndPort(srcAddr, 0);
    BuildSockAddr(addrValue, port, out);
}

//  0x6F660F90 - ping vtable+0x90 (the same NetProvider::Method_0x90 slot
//  netprovider_slot2.cpp/netprovider_slot6.cpp already reach) with "code 2",
//  then return a fixed 0xF - the same const-answer shape netprovider_
//  stubs.cpp's own ConstAnswer_SlotNN family uses, plus the one side effect.
int NetProviderLOOP::NotifySubsystem(int, int, int, int)
{
    typedef void (__thiscall *Method0x90Fn)(void*, int, int, int);
    ((Method0x90Fn)(*(void***)this)[0x90 / 4])(this, 2, 0, 0);
    return 0xF;
}

}  // namespace Net
