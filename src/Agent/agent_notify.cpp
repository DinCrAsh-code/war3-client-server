//============================================================================
//  0x6F4720D0 / 0x6F472100 / 0x6F472130 / 0x6F472160 - four of CAgent's
//  vtable-slot-6 handlers (agent_slot18.cpp), all the same eight
//  instructions with a different message id and a different static record.
//
//  Each fills in a *statically allocated* message object - already built
//  and vtable-stamped by the game's own startup code, so only the id and
//  the owner are written here - and hands it to this object's own vtable
//  slot 5 (+0x14, CObserver::Method_0x14, observer_slots.cpp) with the same
//  id as the first argument.  That is the same (msgId, SAgentMessage*) pair
//  CObserver::Method_0x10 forwards, which is what confirms slot 5's shape
//  from a second, independent direction.
//
//  The three words of each record are declared as three separate globals at
//  their own addresses rather than as one struct: IDA sees them that way
//  (`off_...`, `dword_...+8`, `dword_...+0x10`), the funcmap DATA table
//  keys off exactly those names, and a struct would make every operand a
//  `SYM+n` where the shipped code has a plain `SYM`.
//
//  Own translation unit: slot 6 reaches all four with real calls.
//============================================================================
#include "agent.h"
#include "observer.h"

//  vtable slot 5 (+0x14).
typedef int (__thiscall *Slot0x14Fn)(void* self, unsigned int msgId, void* msg);

//  The four static message records, each three named words.
extern void*        g_agentMsg65;       // off_6FA83C14  - the record itself
extern unsigned int g_agentMsg65Id;     // dword_6FA83C1C
extern void*        g_agentMsg65Owner;  // dword_6FA83C24
extern void*        g_agentMsg66;       // off_6FA83C28
extern unsigned int g_agentMsg66Id;     // dword_6FA83C30
extern void*        g_agentMsg66Owner;  // dword_6FA83C38
extern void*        g_agentMsg67;       // off_6FA83C3C
extern unsigned int g_agentMsg67Id;     // dword_6FA83C44
extern void*        g_agentMsg67Owner;  // dword_6FA83C4C
extern void*        g_agentMsg68;       // off_6FA83C50
extern unsigned int g_agentMsg68Id;     // dword_6FA83C58
extern void*        g_agentMsg68Owner;  // dword_6FA83C60

void CAgent::NotifyMessage65()
{
    g_agentMsg65Id = 0x40190065;
    g_agentMsg65Owner = this;
    ((Slot0x14Fn)(*(void***)this)[0x14 / 4])(this, 0x40190065, &g_agentMsg65);
}

void CAgent::NotifyMessage66()
{
    g_agentMsg66Id = 0x40190066;
    g_agentMsg66Owner = this;
    ((Slot0x14Fn)(*(void***)this)[0x14 / 4])(this, 0x40190066, &g_agentMsg66);
}

void CAgent::NotifyMessage67()
{
    g_agentMsg67Id = 0x40190067;
    g_agentMsg67Owner = this;
    ((Slot0x14Fn)(*(void***)this)[0x14 / 4])(this, 0x40190067, &g_agentMsg67);
}

void CAgent::NotifyMessage68()
{
    g_agentMsg68Id = 0x40190068;
    g_agentMsg68Owner = this;
    ((Slot0x14Fn)(*(void***)this)[0x14 / 4])(this, 0x40190068, &g_agentMsg68);
}
