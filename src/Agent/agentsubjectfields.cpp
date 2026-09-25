//============================================================================
//  0x6F4A7920 and 0x6F4A7940 - two four-byte getters in the 0x6F4A79xx
//  module, the only things CAgent::Handler_eip and CAgent::Handler_oep
//  (agent_bighandlers.cpp) ever do with the subject they are handed.
//
//  Neither is in any dump in `asm/`, so neither is scored and neither has a
//  funcmap row; both were read straight out of the live image with
//  `ida_query bytes`, and the bytes are quoted below so the next reader does
//  not have to ask again.  They are reconstructed rather than redirected
//  because there is nothing in two instructions a transcription can get
//  wrong, and a two-instruction thunk to a two-instruction function is not
//  worth the placeholder.
//
//      0x6F4A7920  8B 41 0C C3     mov eax,[ecx+0Ch] / retn
//      0x6F4A7930  89 51 0C C3     mov [ecx+0Ch],edx / retn   (the setter,
//                                  not reached from this closure)
//      0x6F4A7940  8B 41 10 C3     mov eax,[ecx+10h] / retn
//
//  Own translation unit: both handlers must reach them with real calls, and
//  they are their own module.
//============================================================================
#include "agentsubjectfields.h"

void* __fastcall AgentSubjectGetField0x0C(void* subject)
{
    return *(void**)((char*)subject + 0x0C);
}

//----------------------------------------------------------------------------
//  Declared with a second, ignored parameter on purpose.  Both call sites
//  zero edx before the call (`xor edx,edx`) even though the shipped
//  four-byte body never reads it - the residue of whatever the
//  whole-program build inlined this out of - and a one-argument declaration
//  cannot produce that instruction.  The parameter is unnamed and unread
//  here for exactly the same reason it is unread there.
//----------------------------------------------------------------------------
void* __fastcall AgentSubjectGetField0x10(void* subject, int)
{
    return *(void**)((char*)subject + 0x10);
}
