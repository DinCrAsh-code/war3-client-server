//============================================================================
//  0x6F4C8780 - a pure forwarding thunk through vtable slot 4 (`+0x10`):
//  `this` is passed on unchanged and the call becomes a tail jump.  Nothing
//  else in this call tree reaches whatever the four slots ahead of it are,
//  so they stay unnamed placeholders only there to put Forward() at the
//  right offset.
//
//  `Forward()` is typed `int` (not `void`): sub_6F6F1A20 elsewhere in this
//  dump does `call CallVSlot4 / sub eax, 1`, reading a value out of eax
//  right after the call - since this is a genuine tail `jmp`, not a
//  `call`+`ret`, whatever the real vtable target leaves in eax reaches that
//  caller regardless of what type this thunk claims to return, but the type
//  has to say `int` for that caller's own C++ to compile against it. The
//  tail-jmp shape itself does not depend on the return type either way (an
//  `int`-returning tail call and a `void`-returning one compile to the same
//  instructions here), so this is a free correction, not a risk to
//  CallVSlot4's own already-EXACT score.
//============================================================================
#include "vslot4thunk.h"

int __fastcall CallVSlot4(IVSlot4Target* self)
{
    return self->Forward();
}
