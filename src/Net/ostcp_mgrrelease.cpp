//============================================================================
//  0x6F6E4090 - TCPMGR::Release.  Drop one reference; the last one destructs
//  the manager and hands the block back to Storm under the tag "delete",
//  which is what a Storm-allocated `operator delete` passes in the __FILE__
//  slot.
//============================================================================
#include "ostcp.h"

namespace OsNet
{


void TCPMGR::Release()
{
    if (InterlockedDecrement(&m_refcount) == 0 && this != 0)
    {
        Destruct();
        SMemFree(this, "delete", -1, 0);
    }
}

}  // namespace OsNet
