//============================================================================
//  Net::NetProviderLTCP - vtable slots 10 (0x6F661A90), 11 (0x6F661AA0),
//  12 (0x6F661AC0) and 13 (0x6F661AD0) - one address neighbourhood, the
//  same module.  Identical shape to netproviderloop_addrslots.cpp's own
//  quartet: each overwrites `ecx` from its own stack argument before doing
//  anything, so `this` is provably unread, and each is a thin __thiscall
//  wrapper around the same sockaddrinet.cpp free functions that class's
//  slots 10/11/12 already reach.
//============================================================================
#include "netproviderltcp.h"

int __fastcall AddrFieldsEqual(const void* a, const void* b, int* outFlag);   // sockaddrinet.cpp
unsigned int __fastcall GetAddrIPAndPort(const void* addr, unsigned short* outPort); // sockaddrinet.cpp
void __fastcall BuildSockAddr(unsigned int addr, unsigned short port, void* out);    // sockaddrinet.cpp

namespace Net
{

//  0x6F661A90 - `mov eax,64h / retn`, no arguments at all.
int NetProviderLTCP::GetFixedCode()
{
    return 0x64;
}

//  0x6F661AA0 - forwards straight to AddrFieldsEqual.
int NetProviderLTCP::AddressesEqual(const void* a, const void* b, int* outFlag)
{
    return AddrFieldsEqual(a, b, outFlag);
}

//  0x6F661AC0 - forwards to GetAddrIPAndPort with no out-port.
unsigned int NetProviderLTCP::GetAddressValue(const void* addr)
{
    return GetAddrIPAndPort(addr, 0);
}

//  0x6F661AD0 - read `srcAddr`'s raw address field, then build a fresh
//  struct at `out` from it and `port`.
void NetProviderLTCP::BuildAddress(void* out, const void* srcAddr, unsigned short port)
{
    unsigned int addrValue = GetAddrIPAndPort(srcAddr, 0);
    BuildSockAddr(addrValue, port, out);
}

}  // namespace Net
