//============================================================================
//  0x6F2D6720 - read one saved unit reference back into an existing
//  SCheckedUnitSlot.
//
//  Was a `mov eax,<address> / jmp eax` redirect; sits right next to
//  ReadAgentRef (0x6F2D6790, agentrefread.cpp), which is the same shape:
//  two dwords off the stream (a handle and a type tag) through
//  LookupHandle, the object accepted only if it also carries the '+agl'
//  agile handle tag at +0x0C, and the actual assignment made against its
//  +0x54 rather than the object itself - the same field ReadAgentRef's own
//  header note already establishes.  The one real difference is what the
//  accepted object is assigned *into*: SCheckedUnitSlot::Assign
//  (agenttypedslots.h, 0x6F02A560) rather than CAgentPtr::AssignChecked, so
//  the candidate additionally has to carry CUnit's own '+w3u' agile type
//  before it is accepted.
//
//  The release-on-failure block is written out inline for the same reason
//  ReadAgentRef's is: SCheckedUnitSlot::Release() (agenttypedslots.h) drops
//  the reference but does not null the field, and the shipped code does
//  both.
//
//  Own translation unit: LookupHandle and SCheckedUnitSlot::Assign are both
//  real calls, and CCameraWar3::Load (below this slice) reaches this one
//  with a real call of its own.
//============================================================================
#include "agent.h"
#include "agentregistry.h"
#include "agenttypedslots.h"
#include "cdatastore.h"
#include "game.h"

CDataStore* __fastcall ReadUnitRef(CDataStore* store, SCheckedUnitSlot* out)
{
    unsigned int handle;
    unsigned int typeTag;
    store->ReadDwordAlt(&handle);
    store->ReadDwordAlt(&typeTag);

    SItemHandleObject* object =
        (SItemHandleObject*)LookupHandle(handle, (int)typeTag);

    if (object != 0 && object->m_kindTag == (int)kAgileHandleTag)
    {
        out->Assign((CAgent*)object->m_field54);
    }
    else
    {
        CAgent* held = out->m_value;
        if (held != 0)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
            out->m_value = 0;
        }
    }

    return store;
}
