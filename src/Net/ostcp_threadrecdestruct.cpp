//============================================================================
//  0x6F6DEA80 - TCPTHREADREC::Destruct.  Release the two OsTcp.cpp buffers
//  (lines 0x3E7 and 0x3E8), close the thread handle, release the address and
//  key blocks under their own RTTI tags, and unlink.
//
//  The `.?ATNETADDR@@` tag is a `T`, not a `U` or a `V`: MSVC's descriptor
//  marker for a *union*, so the address block is `union NETADDR`.
//============================================================================
#include "ostcp.h"

namespace OsNet
{


static const char kW32OsTcpCpp[] = ".\\W32\\OsTcp.cpp";

void TCPTHREADREC::Destruct()
{
    if (m_buffer != 0)
        SMemFree(m_buffer, kW32OsTcpCpp, 0x3E7, 0);

    if (m_keyBlock != 0)
        SMemFree(m_keyBlock, kW32OsTcpCpp, 0x3E8, 0);

    CloseHandle(m_thread);

    if (m_addr != 0)
        SMemFree(m_addr, ".?ATNETADDR@@", -2, 0);

    if (m_key != 0)
        SMemFree(m_key, "K", -2, 0);

    m_link.Unlink();
}

}  // namespace OsNet
