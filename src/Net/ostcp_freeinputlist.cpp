//============================================================================
//  0x6F6E0B60 - TCPMGR::FreeInputList.  Drain the loopback input list,
//  unlinking each record from *both* the lists it is on before freeing it.
//
//  The element type comes out of the free: the tag is the RTTI descriptor
//  `.?AUINPUT@LOOPCONN@OsNet@@`, i.e. `struct OsNet::LOOPCONN::INPUT`, not a
//  filename.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

void TCPMGR::FreeInputList()
{
    for (;;)
    {
        LOOPINPUT* entry = (LOOPINPUT*)m_inputs.TailLink();
        if ((int)entry <= 0)
            break;

        entry->m_link2.Unlink();
        entry->m_link.Unlink();
        SMemFree(entry, ".?AUINPUT@LOOPCONN@OsNet@@", -2, 0);
    }
}

}  // namespace OsNet
