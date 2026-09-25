//============================================================================
//  0x6F472A00 / 0x6F4726A0 / 0x6F4725D0 - the three large handlers CAgent's
//  vtable slot 6 (agent_slot18.cpp) dispatches to, named for the FourCC
//  selector that reaches each ('+lkl`', '+oep`', '+eip`').
//
//  All three were `mov eax,<address> / jmp eax` redirects, left "out of
//  scope" by the session that reconstructed the surrounding module.  They
//  are the big cousins of agent_handlers.cpp's Handler_lcf/Handler_ltr:
//  each fills in a *statically allocated* notify record - already built and
//  vtable-stamped by the game's own startup code, so only the payload words
//  are written here - and hands it to the notify helper at 0x6F471FD0
//  (SAgentNotifyTarget::Notify, agentnotifytarget.cpp) along with a value
//  and that value's own sign bit.  What is new in these three is what they
//  do *before* that: they resolve an agent reference first, and the
//  reference is a real object with a shipped vtable.
//
//  None of the three can reach 1.000: each is wrapped in the
//  `__except_handler4`-shaped SEH frame docs/msvc-vc8-idioms.md documents
//  as unreproducible at this repo's fixed `/GS- /EHs-c-` ("An
//  `__except_handler4`-shaped frame this toolchain cannot reproduce"), and
//  that frame is ~20 instructions of prologue, epilogue, unwind-state
//  stores and funclets on top of every body here.  Handler_lkl's two
//  stack-built AgentRefListener/AgentRef objects and Handler_eip's and
//  Handler_oep's stack CAgentPtr are exactly the "a local with a destructor
//  plus a call the compiler cannot prove will not throw" case that frame
//  exists for.
//
//  Own translation unit: the three do not call each other, but every leaf
//  below them must be reached with a real call.
//============================================================================
#include "agent.h"
#include "agentref.h"
#include "agentsubjectfields.h"

//----------------------------------------------------------------------------
//  What the dispatcher's payload looks like from here.  The same object
//  agent_handlers.cpp's two small handlers read, with two more words named:
//  the {handle,typeTag} pair at +0x14/+0x18 that Handler_lkl copies into
//  its listener, which is the same pair +0x14 alone is read as elsewhere.
//----------------------------------------------------------------------------
struct SAgentSlot18Payload
{
    char         m_reserved00[0x14];
    int          m_field14;    // +0x14
    int          m_field18;    // +0x18
    char         m_reserved1C[0x48 - 0x1C];
    int          m_field48;    // +0x48
};

//  0x6F471FD0 - agentnotifytarget.cpp.  Declared exactly as
//  agent_handlers.cpp declares it so both TUs name the same symbol.
struct SAgentNotifyTarget
{
    int Notify(int value, void* record, unsigned int sign);
};

//  0x6F4806C0 - misc_handle_resolvers.cpp: resolve the {handle,typeTag}
//  pair at this+0x08/+0x0C and read the resolved object's +0x40.  Declared,
//  not defined, here.
struct SHandleRefAt0x08Owner { void* __thiscall ResolveAndGetField0x40(); };

//----------------------------------------------------------------------------
//  What Handler_eip and Handler_oep reach through the subject's +0x10
//  object: the record at its +0x30, whose +0x54 is the agent to publish and
//  whose +0x20 is the same "on its way out, do not use me" flag every other
//  reader in this family gates on (SAgileAgent::m_retired in agentquery.h,
//  the pending-flag guard in itemhandleresolve.h).
//----------------------------------------------------------------------------
struct SAgentRefRecord
{
    char  m_reserved00[0x20];
    int   m_field20;   // +0x20
    char  m_reserved24[0x54 - 0x24];
    void* m_field54;   // +0x54
};

//  The three static notify records, each named for its own address the way
//  agent_notify.cpp names its four.  '+lkl`' writes the payload's +0x48
//  into the record's +0x08 and the resolved AgentRef's +0x54 into its
//  +0x10 - the same (record, +0x08, +0x10) triple Handler_lcf/Handler_ltr
//  and the four NotifyMessage bodies all use.
extern void*        g_agentNotifyRecordBD4;   // off_6FA83BD4
extern unsigned int g_agentNotifyWordBDC;     // dword_6FA83BDC
extern void*        g_agentNotifyWordBE4;     // dword_6FA83BE4
extern void*        g_agentNotifyRecordBE8;   // off_6FA83BE8   '+eip`'
extern unsigned int g_agentNotifyWordBF0;     // dword_6FA83BF0
extern void*        g_agentNotifyWordBF8;     // dword_6FA83BF8
extern void*        g_agentNotifyRecordBFC;   // off_6FA83BFC   '+oep`'
extern void*        g_agentNotifyWordC0C;     // dword_6FA83C0C
extern int          g_agentNotifyWordC10;     // dword_6FA83C10

//----------------------------------------------------------------------------
//  0x6F472A00 - '+lkl`'.
//
//  Build a listener over the payload's own {handle,typeTag} pair, resolve it
//  to some other object and copy *that* object's {handle,typeTag} pair at
//  +0x14/+0x18 into a second reference, resolve that in turn to its +0x54,
//  and publish the answer alongside the payload's +0x48 in the static
//  record before the ordinary notify.
//
//  Both references start at the all-ones sentinel and only get a real pair
//  when their source is non-null, which is what the two `if`s are; the
//  shipped code shares the single `or eax,-1` across all four sentinel
//  stores, and this build does too.
//
//  Note that the payload is null-tested for the listener and then read
//  again, unguarded, for the record and the notify value.  That asymmetry
//  is the shipped code's (`test esi,esi` / `jz`, then `mov eax,[esi+48h]`
//  unconditionally) and is reproduced rather than tidied.
//----------------------------------------------------------------------------
void CAgent::Handler_lkl(void* payload, void*)
{
    AgentRefListener listener;
    listener.m_reserved04 = 0;
    listener.m_handle  = 0xFFFFFFFFu;
    listener.m_typeTag = -1;
    listener.m_vtable  = &g_vftAgentRefListener;

    SAgentSlot18Payload* p = (SAgentSlot18Payload*)payload;
    if (p != 0)
    {
        listener.m_handle  = (unsigned int)p->m_field14;
        listener.m_typeTag = p->m_field18;
    }

    AgentRef ref;
    ref.m_reserved04 = 0;
    ref.m_handle  = 0xFFFFFFFFu;
    ref.m_typeTag = -1;
    ref.m_vtable  = &g_vftAgentRef;

    void* resolved = ((SHandleRefAt0x08Owner*)&listener)->ResolveAndGetField0x40();
    if (resolved != 0)
    {
        ref.m_handle  = *(unsigned int*)((char*)resolved + 0x14);
        ref.m_typeTag = *(int*)((char*)resolved + 0x18);
    }

    g_agentNotifyWordBE4 = ref.QueryField0x54();
    g_agentNotifyWordBDC = (unsigned int)p->m_field48;

    int value = p->m_field14;
    ((SAgentNotifyTarget*)this)->Notify(value, &g_agentNotifyRecordBD4,
                                        (unsigned int)value >> 31);
}

//----------------------------------------------------------------------------
//  0x6F4726A0 - '+oep`'.
//
//  Handler_eip's near-twin, and the differences between the two are all
//  real: this one null-tests the +0x10 object before reading its +0x30,
//  publishes the resolved agent and a literal zero into its record's
//  +0x10/+0x14 (not the +0x08/+0x10 pair every other record in this family
//  uses) *unconditionally*, and then - only when the agent resolved -
//  notifies **the resolved agent** rather than `this`, and follows the
//  notify with a second post through that agent's own embedded observer at
//  +0x14 (0x6F471A40, agentrefhelpers.cpp).
//
//  The two record stores sit outside the `if` on purpose: the shipped code
//  tests the pointer, stores both words, and only then branches
//  (`test ecx,ecx` / two `mov`s / `jz`), so a run that resolves nothing
//  still clears the record.
//----------------------------------------------------------------------------
void CAgent::Handler_oep(void* payload, void* subject)
{
    void* holder = AgentSubjectGetField0x0C(subject);
    void* owner  = AgentSubjectGetField0x10(subject, 0);

    if (owner != 0)
    {
        SAgentRefRecord* rec = *(SAgentRefRecord**)((char*)owner + 0x30);
        if (rec->m_field20 == 0)
        {
            CAgentPtr agent(0);

            //  Read before the assignment, which is where the shipped code
            //  reads it (`mov eax,[edi+54h]` / `mov esi,[esi+14h]`, then the
            //  call): `holder` is dead afterwards and the value has to
            //  survive AssignChecked either way.
            int value = *(int*)((char*)holder + 0x14);
            agent.AssignChecked((CAgent*)rec->m_field54);

            g_agentNotifyWordC0C = agent.m_ptr;
            g_agentNotifyWordC10 = 0;

            if (agent.m_ptr != 0)
            {
                SAgentSlot18Payload* p = (SAgentSlot18Payload*)payload;
                ((SAgentNotifyTarget*)agent.m_ptr)->Notify(
                    value, &g_agentNotifyRecordBFC,
                    (unsigned int)p->m_field14 >> 31);
                agent.m_ptr->PostEventToSelfObserver((unsigned int)value, 0);
            }
        }
    }
}

//----------------------------------------------------------------------------
//  0x6F4725D0 - '+eip`'.
//
//  The subject (the handler's *second* argument, not the payload) carries
//  two objects at +0x0C and +0x10; the second one's +0x30 is the record
//  whose +0x54 is the agent to publish.  A record whose +0x20 is set is on
//  its way out and the whole body is skipped.
//
//  The published agent goes through a real CAgentPtr, which is what the
//  stack object with the inlined release-if-last tail is: constructed empty
//  and then assigned through the type-checked AssignChecked, exactly as
//  CAgent::Method_0x54 (agent_slot21.cpp) does with its own.
//
//  The notify's *value* and its *sign bit* come from two different objects -
//  the value from the +0x0C object's +0x14, the sign from the payload's own
//  +0x14 - which is the one thing here that does not follow the pattern the
//  four small handlers set, and it is what the shipped `mov eax,[eax+14h]` /
//  `mov ecx,[ebx+14h]` pair says.
//
//  Unlike Handler_oep below, the +0x10 object is *not* null-tested before
//  its +0x30 is read.  That asymmetry between two otherwise near-identical
//  bodies is the shipped code's.
//----------------------------------------------------------------------------
void CAgent::Handler_eip(void* payload, void* subject)
{
    void* holder = AgentSubjectGetField0x0C(subject);
    void* owner  = AgentSubjectGetField0x10(subject, 0);

    SAgentRefRecord* rec = *(SAgentRefRecord**)((char*)owner + 0x30);
    if (rec->m_field20 == 0)
    {
        CAgentPtr agent(0);
        agent.AssignChecked((CAgent*)rec->m_field54);

        SAgentSlot18Payload* p = (SAgentSlot18Payload*)payload;
        g_agentNotifyWordBF0 = (unsigned int)p->m_field48;
        g_agentNotifyWordBF8 = agent.m_ptr;

        ((SAgentNotifyTarget*)this)->Notify(*(int*)((char*)holder + 0x14),
                                            &g_agentNotifyRecordBE8,
                                            (unsigned int)p->m_field14 >> 31);
    }
}
