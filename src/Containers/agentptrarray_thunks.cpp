//============================================================================
//  Redirect for the CAgentPtr<CGhostImage> array instantiation whose
//  SetCount this batch reconstructs (ghostimageagentptrcount.cpp) but whose
//  ComputeChunk and SetAlloc it does not.
//
//  CAgentPtr<CSelectable>'s own ComputeChunk/SetAlloc (0x6F378530 /
//  0x6F3796A0) used to be redirected here too; they are real now -
//  Containers/selectableptrarray.cpp - and the two redirects that stood in
//  for them here are gone with them.
//
//  Has to be an *explicit specialisation* and not a plain declaration: a
//  translation unit that calls a member of a class template and cannot see a
//  specialisation instantiates the generic body instead, and the generic
//  SetAlloc moves each element with the compiler's own memberwise copy - no
//  addref - which is a silently wrong body the linker may then pick over the
//  shipped one.  Redirecting it here means every caller reaches the real
//  shipped function.
//
//  Each `retn 4` is the shipped one.
//============================================================================
#include "agenttypedslots.h"
#include "ghostimageslot.h"
#include "storm.h"

#define AGENTPTR_ARRAY_THUNK(addr)   \
    {                                \
        __asm { mov eax, addr }      \
        __asm { jmp eax }            \
    }

//  0x6F2ABB10 / 0x6F2ACCA0 - CAgentPtr<CGhostImage>, tagged
//  ".?AV?$CAgentPtr@VCGhostImage@@@@".
template <>
__declspec(naked) unsigned int
TSGrowableArray<SCheckedGhostImageSlot>::ComputeChunk(unsigned int)
    AGENTPTR_ARRAY_THUNK(06F2ABB10h)

template <>
__declspec(naked) void
TSGrowableArray<SCheckedGhostImageSlot>::SetAlloc(unsigned int)
    AGENTPTR_ARRAY_THUNK(06F2ACCA0h)
