//============================================================================
//  0x6F47FD90 - not itself a vtable slot in this batch's own dump (reached
//  only from slot 12's own tail-jmp, Agent/cagentbaseabs_slot12.cpp): acquire
//  a fresh delegate from this CAgentBaseAbs's own type pool (this+0x10's own
//  agile type id, via AgentBase_InvokeTypePoolSlot0,
//  Agent/cagentbaseabs_typepoolslot0.cpp) and install it as the +0x54
//  delegate through SetDelegate (Agent/cagentbaseabs_module_batch2.cpp).
//
//  Own translation unit, same reasoning as its two callees' own file
//  headers: both calls below are one-liners this compiler's /Ob2 would
//  gladly inline into a single, larger body, which the shipped dump's own
//  two real `call`s (sub_6F46DCE0, then sub_6F47FCE0) show did not happen
//  for the real binary.
//============================================================================

//  Declared only (no definition visible here) so this compiler cannot
//  inline either call - see this file's own header comment.
void* __fastcall AgentBase_InvokeTypePoolSlot0(unsigned int typeIdRaw);

class CAgent;

class CAgentBaseAbs
{
public:
    void SetDelegate(CAgent* newDelegate);
    void BindDelegateFromField10();
};

void CAgentBaseAbs::BindDelegateFromField10()
{
    unsigned int typeId = *(unsigned int*)((char*)this + 0x10);
    void* pooled = AgentBase_InvokeTypePoolSlot0(typeId);
    SetDelegate((CAgent*)pooled);
}
