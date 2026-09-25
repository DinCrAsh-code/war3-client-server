//============================================================================
//  0x6F472010 and 0x6F472040 - two more of CAgent's vtable-slot-6 handlers
//  (agent_slot18.cpp).  The same eleven instructions twice over at two
//  addresses, which is what this image being linked without /OPT:ICF leaves
//  standing (agentdefaults.cpp).
//
//  Each reads two fields off the payload the dispatcher handed it, parks
//  one in a static word, and hands the other - with its own sign bit as a
//  third argument - to the notify helper at 0x6F471FD0 along with a static
//  record.  Nothing in these three vtables says what any of the three
//  quantities is.
//
//  Own translation unit: slot 6 reaches both with real calls, and both
//  reach the notify helper with one.
//============================================================================
#include "agent.h"

//  What the dispatcher's payload looks like from here - only the two fields
//  these two handlers read.
struct SAgentSlot18Payload
{
    char m_reserved00[0x14];
    int  m_field14;   // +0x14
    char m_reserved18[0x30];
    int  m_field48;   // +0x48
};

//  0x6F471FD0 - deliver a value through the object's own embedded observer
//  (+0x14), pinned by a temporary reference exactly the way
//  CObserver::Method_0x14 pins its own.  Reconstructed in
//  agentnotifytarget.cpp; declared, not defined, here so both handlers
//  below reach it with real calls.  A real thiscall member: three stack
//  arguments (`retn 0Ch`) with only `this` in ecx, and it forwards the
//  callback's own return value, which neither caller reads.
struct SAgentNotifyTarget
{
    int Notify(int value, void* record, unsigned int sign);
};

extern void*        g_agentNotifyRecordBC4;   // off_6FA83BC4
extern unsigned int g_agentNotifyWordBCC;     // dword_6FA83BCC

void CAgent::Handler_lcf(void* payload)
{
    SAgentSlot18Payload* p = (SAgentSlot18Payload*)payload;
    g_agentNotifyWordBCC = (unsigned int)p->m_field48;

    int value = p->m_field14;
    ((SAgentNotifyTarget*)this)->Notify(value, &g_agentNotifyRecordBC4,
                                        (unsigned int)value >> 31);
}

void CAgent::Handler_ltr(void* payload)
{
    SAgentSlot18Payload* p = (SAgentSlot18Payload*)payload;
    g_agentNotifyWordBCC = (unsigned int)p->m_field48;

    int value = p->m_field14;
    ((SAgentNotifyTarget*)this)->Notify(value, &g_agentNotifyRecordBC4,
                                        (unsigned int)value >> 31);
}
