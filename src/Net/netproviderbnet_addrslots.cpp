//============================================================================
//  Net::NetProviderBNET - vtable slots 10 (0x6F65DE40), 11 (0x6F65DE50),
//  12 (0x6F65DE70) and 13 (0x6F65DE80).  Same address-neighbourhood
//  quartet as netproviderltcp_addrslots.cpp/netproviderloop_addrslots.cpp's
//  own: each overwrites `ecx` from its own stack argument before doing
//  anything (so `this` is provably unread) and each is a thin wrapper
//  around the same sockaddrinet.cpp free functions those two classes'
//  own quartets already reach.  Only the fixed code constant (0xFA here,
//  vs. LTCP's 0x64) differs.
//============================================================================

namespace Net
{

class NetProviderBNET
{
public:
    int            GetFixedCode();                                             // slot 10
    int            AddressesEqual(const void* a, const void* b, int* outFlag); // slot 11
    unsigned int   GetAddressValue(const void* addr);                          // slot 12
    void           BuildAddress(void* out, const void* srcAddr, unsigned short port); // slot 13
};

}  // namespace Net

int __fastcall AddrFieldsEqual(const void* a, const void* b, int* outFlag);          // sockaddrinet.cpp
unsigned int __fastcall GetAddrIPAndPort(const void* addr, unsigned short* outPort); // sockaddrinet.cpp
void __fastcall BuildSockAddr(unsigned int addr, unsigned short port, void* out);    // sockaddrinet.cpp

//  0x6F65DE40 - `mov eax, 0FAh / retn`, no arguments at all.
int Net::NetProviderBNET::GetFixedCode()
{
    return 0xFA;
}

//  0x6F65DE50 - forwards straight to AddrFieldsEqual.
int Net::NetProviderBNET::AddressesEqual(const void* a, const void* b, int* outFlag)
{
    return AddrFieldsEqual(a, b, outFlag);
}

//  0x6F65DE70 - forwards to GetAddrIPAndPort with no out-port.
unsigned int Net::NetProviderBNET::GetAddressValue(const void* addr)
{
    return GetAddrIPAndPort(addr, 0);
}

//  0x6F65DE80 - read `srcAddr`'s raw address field, then build a fresh
//  struct at `out` from it and `port`.
void Net::NetProviderBNET::BuildAddress(void* out, const void* srcAddr, unsigned short port)
{
    unsigned int addrValue = GetAddrIPAndPort(srcAddr, 0);
    BuildSockAddr(addrValue, port, out);
}
