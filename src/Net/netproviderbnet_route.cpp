//============================================================================
//  Net::NetProviderBNET - vtable slots 14 (0x6F65DEA0, RouteToPort), 15
//  (0x6F65DED0, BindPort) and 16 (0x6F65DF20, FreePort).  Same three deep
//  OS-transport addresses NetProviderLTCP's own quartet already reaches
//  (netproviderltcp_route.cpp/bindport.cpp/freeport.cpp) - reused directly
//  rather than re-thunked, since a shipped free function reached at the
//  same address from two different classes is still one function.
//============================================================================

unsigned int __fastcall GetAddrIPAndPort(const void* addr, unsigned short* outPort); // sockaddrinet.cpp
void __fastcall LtcpRouteDeliver(int addrValue, const void* addr, int arg4, int arg8, const void* data, unsigned int size); // netproviderltcp_route.cpp
int __fastcall TryBindPort(unsigned short port, int target, int arg8);   // netproviderltcp_bindport.cpp
void __fastcall FreeBoundPort(unsigned short port, int zero);            // netproviderltcp_freeport.cpp

namespace Net
{

class NetProviderBNET
{
public:
    void RouteToPort(const void* addr, int arg4, int arg8, const void* data, unsigned int size);
    unsigned short BindPort(unsigned short port, unsigned short limit, int target);
    void FreePort(unsigned short port);
};

//  0x6F65DEA0 - vtable slot 14.  Same "reuse the caller's own addr stack
//  slot as out-port scratch" idiom netproviderltcp_route.cpp's own header
//  documents.
void NetProviderBNET::RouteToPort(const void* addr, int arg4, int arg8, const void* data, unsigned int size)
{
    unsigned int addrValue = GetAddrIPAndPort(addr, (unsigned short*)&addr);
    LtcpRouteDeliver(addrValue, addr, arg4, arg8, data, size);
}

//  0x6F65DED0 - vtable slot 15.
unsigned short NetProviderBNET::BindPort(unsigned short port, unsigned short limit, int target)
{
    for (unsigned short candidate = port; candidate < limit; ++candidate)
    {
        if (TryBindPort(candidate, target, 0) != 0)
            return candidate;
    }
    return 0;
}

//  0x6F65DF20 - vtable slot 16.
void NetProviderBNET::FreePort(unsigned short port)
{
    FreeBoundPort(port, 0);
}

}  // namespace Net
