//============================================================================
//  0x6F28B6A0 - CUnit::AssignRef304 (unit.h).  Release the notify on the
//  currently-resolved m_ref304 target, then chain-resolve a new value into
//  it from `source` (SOptionalHandleRefResolver::ResolveChained,
//  unitorder.h - already reconstructed).
//============================================================================
#include "unit.h"
#include "unitorder.h"

void CUnit::AssignRef304(const void* source)
{
    NotifyRef304Deactivated();
    ((SOptionalHandleRefResolver*)&m_ref304)->ResolveChained(source);
}
