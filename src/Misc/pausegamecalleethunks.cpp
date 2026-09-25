//============================================================================
//  The three redirects batch B of `JASS_PauseGame`'s closure needs and does
//  not own: each is a direct `call` out of a batch-B body into a function
//  batches F and G will reconstruct.  Nothing here is a reconstruction.
//
//  The declared argument bytes are the load-bearing part, as always: each
//  callee cleans its own stack, and the shipped `retn <n>` is written next
//  to it (tools/thunk_abi_audit.py).
//============================================================================
#include "config.h"
#include "stormsingletona.h"

#define PAUSEGAME_THUNK(addr)    \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

//  0x6F6F3570 is a real body now, in Config/configsource.cpp; 0x6F4D2D70 is
//  Render/spriteintersect.cpp and 0x6F762310 is
//  Render/stormsingletonpick.cpp.  All three redirects are gone: leaving one
//  behind after its body lands makes verify.py score the two-instruction
//  redirect instead of the body.
