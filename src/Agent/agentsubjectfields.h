//============================================================================
//  The two four-byte getters CAgent::Handler_eip and CAgent::Handler_oep
//  (agent_bighandlers.cpp) read their subject through.  See the .cpp.
//============================================================================
#ifndef AGENTSUBJECTFIELDS_H
#define AGENTSUBJECTFIELDS_H

//  0x6F4A7920 - `mov eax,[ecx+0Ch] / retn`.
void* __fastcall AgentSubjectGetField0x0C(void* subject);

//  0x6F4A7940 - `mov eax,[ecx+10h] / retn`.  The second parameter exists
//  only so this build emits the `xor edx,edx` both shipped call sites do;
//  the shipped four-byte body never reads edx.  See the .cpp.
void* __fastcall AgentSubjectGetField0x10(void* subject, int unused);

#endif
