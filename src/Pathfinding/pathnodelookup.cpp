//============================================================================
//  0x6F49D2F0 - resolve the client's recorded best node back to an object.
//
//  A tail jump into the global handle table with the client's own stored
//  handle/tag pair.  Its result is only ever tested against null, which is
//  how the -1 sentinel CPathClient::SetBestNode writes for "no node" reads
//  back as "nothing there".
//============================================================================
#include "pathmove.h"

CHandleObject* CPathClient::ResolveBestNode() const
{
    return LookupHandle(m_bestHandle, m_bestTypeTag);
}
