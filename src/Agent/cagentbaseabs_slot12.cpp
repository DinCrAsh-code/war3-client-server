//============================================================================
//  slot 12 / +0x30 (0x6F47FDC0) - the shipped body is a bare
//  `jmp sub_6F47FD90`, i.e. this slot *is*
//  CAgentBaseAbs::BindDelegateFromField10 (Agent/cagentbaseabs_binddelegate.cpp)
//  under a second address/symbol.
//
//  Own translation unit: with the callee's real body visible in the same
//  TU, this compiler's /Ob2 inlines the whole thing into a much larger body
//  instead of the plain tail `jmp` the shipped dump actually has - declared
//  only here, so nothing can be inlined, and a bare forwarding call in tail
//  position compiles to that same `jmp`.
//============================================================================
class CAgentBaseAbs
{
public:
    void BindDelegateFromField10();
    void Slot12_BindDelegate();
};

void CAgentBaseAbs::Slot12_BindDelegate()
{
    BindDelegateFromField10();
}
