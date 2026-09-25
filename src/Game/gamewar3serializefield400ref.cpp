//============================================================================
//  0x6F3A0990 - identical body, its own real address.  See
//  gamewar3serializegameslotref.cpp's own header comment for why this is
//  its own translation unit.  Called from CGameWar3::Save
//  (gamewar3save.cpp) for `m_field400`.
//============================================================================
#include "cdatastorescratch.h"
#include "agent.h"

CDataStoreScratch* __fastcall SerializeField400Ref(CDataStoreScratch* stream,
                                                     CAgent* const* ref)
{
    CAgent* agent = *ref;

    stream->WriteDword(agent ? agent->m_handle   : (unsigned int)-1);
    stream->WriteDword(agent ? agent->m_typeTag  : (unsigned int)-1);

    return stream;
}
