//============================================================================
//  0x6F62D3F0 - a second `jmp Event::CloseForward` thunk, distinct from
//  Event_CloseForward2 (0x6F62D3D0, eventcloseforward2.cpp) even though its
//  body is identical.  IDA's own name for it (`j_j_Event::Close`) says why:
//  it is a jmp-to-a-jmp-to-Event::Close chain the linker/compiler emitted a
//  second time for a different __finally scope table (sub_6F62E5B0's, in
//  this dump) that needed its own thunk at its own address.  Modelled the
//  same way as Event_CloseForward2 - a free function taking the object
//  explicitly, since nothing in the dump shows a `this` load ahead of the
//  jmp; it is reached with whatever `ecx` the unwind already set up.
//============================================================================
#include "event.h"

void __fastcall EventCloseForward_J(Event* self)
{
    self->CloseForward();
}
