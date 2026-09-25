//============================================================================
//  0x6F459180 / 0x6F459190 - the two-instruction accessor pair for
//  JassInstance's own +0x48 field, both reached from the same "spawn a
//  child instance" site in the 0x6F455250 closure: right after
//  construction (0x6F4549E0, jassvm-batch-1's own reconstruction) the
//  caller overwrites the constructor's own default of 1 with a value from
//  its own arg_C.  See jassinstance.h for what little this closure
//  resolves about the field itself.
//============================================================================
#include "jassinstance.h"
#include "jassthreadstate.h"

//  jassthreadinstanceslot.cpp - not exposed through jassthreadstate.h.
JassInstance* __fastcall JassThreadGetInstance(unsigned int index);

//  0x6F44B2E0 - jassthreadinstanceslot.cpp/jassrunorcompilestream.cpp.
JassInstance* __fastcall LookupInstanceBySlot(unsigned int lookupKey);

//  0x6F44B700 - `retn 4`, `__fastcall(index in ecx, funcPtr in edx)` plus one
//  stack argument (funcArg).  Resolves the instance at `index` through
//  LookupInstanceBySlot and stamps its +0x28A0/+0x28A4 pair directly -
//  jassinstance.h's own note on those two fields ("written directly by the
//  instance's own caller ... right after construction") is this function.
void __fastcall SetInstanceNativeFuncFields(unsigned int index, void* funcPtr, void* funcArg)
{
    JassInstance* instance = LookupInstanceBySlot(index);
    instance->m_nativeFuncPtr = funcPtr;
    instance->m_nativeFuncArg = funcArg;
}

//  0x6F459180
void JassInstance::SetField48(int value)
{
    m_field48 = value;
}

//  0x6F459190
int JassInstance::GetField48()
{
    return m_field48;
}

//----------------------------------------------------------------------------
//  0x6F44D9B0 - `retn 0` (__fastcall, the thread index arrives in ecx and
//  is forwarded unchanged, since this function never touches its own
//  parameter before passing it to JassThreadGetInstance).
//
//  Spawn a child of the calling thread's own JASS instance and auto-assign
//  it a fresh handle slot on the *same* thread - the driver
//  JassThreadRegisterInstance's own "auto-assign" path (index == 0) leaves
//  the `+0x2850` stamp to its caller (jassthreadinstanceslot.cpp's own
//  AssignInstanceSlot note); this is that caller, and the return value is
//  AssignInstanceSlot's own assigned index (jassthreadstate.h's own
//  updated declaration), read straight back out of `eax` into the child's
//  `+0x2850` rather than a second lookup.
//----------------------------------------------------------------------------
unsigned int __fastcall JassThreadSpawnChildInstance(unsigned int index)
{
    JassInstance* instance = JassThreadGetInstance(index);
    JassInstance* child = instance->CreateChildInstance();
    unsigned int assigned = JassThreadRegisterInstance(child, 0);
    *(unsigned int*)((char*)child + 0x2850) = assigned;
    return assigned;
}
