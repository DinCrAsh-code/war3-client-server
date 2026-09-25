//============================================================================
//  0x6F4A5EC0 - SSlot18Arg::GetPayload, the two-instruction getter CAgent's
//  vtable slot 6 (agent_slot18.cpp) calls on its second argument before
//  dispatching.
//
//  Own translation unit so that call survives; the object it reads +0x0C
//  out of is not otherwise identified by anything in these three vtables.
//============================================================================

struct SSlot18Arg
{
    void* GetPayload();

    char  m_reserved00[0x0C];
    void* m_payload;    // +0x0C
};

void* SSlot18Arg::GetPayload()
{
    return m_payload;
}
