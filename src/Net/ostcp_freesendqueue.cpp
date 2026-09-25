//============================================================================
//  OsNet::OUTPUTLIST::FreeAll (0x6F6DF340) - drain the send queue, releasing
//  every buffer and the payload hanging off it.
//
//  Its own translation unit so ~TCPCONN keeps the call.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//  The module's own filename, which is what Storm's allocator gets where it
//  expects __FILE__.
static const char kW32OsTcpCpp[] = ".\\W32\\OsTcp.cpp";

//  ...except for the buffer object itself, which is freed the way every
//  `delete` in Storm's container code is: with the element type's RTTI
//  descriptor name in the filename slot and -2 for the line.  The string in
//  the binary is `.?AUOUTPUT@OsNet@@`, which is where this file's idea of what
//  the struct is called comes from.
static const char kOutputTypeName[] = ".?AUOUTPUT@OsNet@@";

//----------------------------------------------------------------------------
//  0x6F6DF340.
//
//  Walked from the tail with the raw link value and a signed test, the same
//  way UnlinkAll walks: unlinking the tail is the one operation the link
//  encoding makes cheap, and `<= 0` is the end-of-list test, not a null check.
//
//  The payload is only released when the buffer claims a capacity *and* holds
//  a pointer - a buffer whose second allocation failed has one and not the
//  other, and this is what keeps that case from freeing a null.
//----------------------------------------------------------------------------
void OUTPUTLIST::FreeAll()
{
    for (;;)
    {
        int tail = TailLink();
        if (tail <= 0)
            break;

        OUTPUT* buf = (OUTPUT*)tail;
        if (buf->m_capacity != 0 && buf->m_data != 0)
            SMemFree(buf->m_data, kW32OsTcpCpp, 0x190, 0);

        buf->m_link.Unlink();
        SMemFree(buf, kOutputTypeName, -2, 0);
    }
}

}  // namespace OsNet
