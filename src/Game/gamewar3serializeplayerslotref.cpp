//============================================================================
//  0x6F3A0910 - identical body, its own real address.  See
//  gamewar3serializegameslotref.cpp's own header comment for why this is
//  its own translation unit rather than sharing SerializeGameSlotRef's.
//  Called from CGameWar3::Save (gamewar3save.cpp) once per live
//  `m_playerSlots[i]` entry, and from SerializeNestedRefTable
//  (gamewar3serializenestedreftable.cpp) for its own inner refs.
//============================================================================
#include "cdatastorescratch.h"
#include "agent.h"

CDataStoreScratch* __fastcall SerializePlayerSlotRef(CDataStoreScratch* stream,
                                                       CAgent* const* ref)
{
    CAgent* agent = *ref;

    stream->WriteDword(agent ? agent->m_handle   : (unsigned int)-1);
    stream->WriteDword(agent ? agent->m_typeTag  : (unsigned int)-1);

    return stream;
}
