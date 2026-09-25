//============================================================================
//  A further batch of small, self-contained leaves with no owning class
//  otherwise identified - same convention as misc_leaves_batch3.cpp/
//  misc_leaves_batch4.cpp. Both are reached (as a callee of a callee) from
//  CItem's own vtable closure:
//    - sub_6F2AAF70 is tail-jumped to from CWidget::NotifyTargetLost
//      (0x6F2AB310, widget_targetlost.cpp - itself still a thunk);
//    - sub_6F472090/sub_6F4727B0 are tail-jumped to from
//      CAgent::Handler_lkl (0x6F472A00, agent_bighandlers.cpp).
//  None of those three currently calls its own reconstruction by name in
//  its own C++ (each is close enough to its target already, via a
//  different instruction shape, to be scored IDENTICAL without literally
//  reaching these addresses) - wiring them in for real is separate work.
//
//  0x6F4A7920/0x6F4A7940 (the other two addresses this batch originally
//  carried) are NOT here: they already had real reconstructions -
//  AgentSubjectGetField0x0C/AgentSubjectGetField0x10,
//  src/Agent/agentsubjectfields.cpp, already called from
//  CAgent::Handler_eip/Handler_oep - that this batch duplicated because
//  the address wasn't grepped for first. Fixed by giving funcmap.py
//  entries to the existing functions instead of keeping the duplicate.
//============================================================================
#include "refcnt.h"

//  0x6F2AAF70 - two instructions: reduce some other, unidentified
//  TRefCnt-derived object back to a bare TRefCnt by explicitly invoking the
//  (empty) base destructor - the same "call an empty ~TRefCnt() and get
//  only the vtable stamp" idiom refcnt_deleteself.cpp's own
//  TRefCnt::DeleteSelf documents for 0x6F00C080. `this` arrives in ecx and
//  is never null-tested (unlike CAgent's/CObserver's own equivalents),
//  because it's written to before any test would be reached, matching that
//  same file's own reasoning.
struct SNotifyTargetLostPayload : public TRefCnt
{
    void __thiscall ResetToTRefCnt();
};
void __thiscall SNotifyTargetLostPayload::ResetToTRefCnt()
{
    this->TRefCnt::~TRefCnt();
}

//  0x6F472090 / 0x6F4727B0 - identical two-instruction shape to the above,
//  two different (unidentified) classes reached from CAgent::Handler_lkl's
//  own '+lkl' dispatch.
struct SAgentLklTargetA : public TRefCnt
{
    void __thiscall ResetToTRefCnt();
};
void __thiscall SAgentLklTargetA::ResetToTRefCnt()
{
    this->TRefCnt::~TRefCnt();
}

struct SAgentLklTargetB : public TRefCnt
{
    void __thiscall ResetToTRefCnt();
};
void __thiscall SAgentLklTargetB::ResetToTRefCnt()
{
    this->TRefCnt::~TRefCnt();
}
