//============================================================================
//  0x6F495940 - CPathTrace::ResolveTargetHandle: resolve the
//  {m_targetHandle, m_targetTypeTag} pair through LookupHandle - a plain
//  forward, the same shape as SHandleRefAt0x68Owner::ResolveHandle
//  (misc_handle_resolvers.cpp).
//============================================================================
#include "game.h"

CGridRegistration* CPathTrace::ResolveTargetHandle()
{
    return (CGridRegistration*)LookupHandle(m_targetHandle, m_targetTypeTag);
}
