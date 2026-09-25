//============================================================================
//  Two naked redirects into Storm's own generic, SEH-guarded per-object
//  message-subscriber-list broadcast - see inputeventpump.h's own header
//  comment and docs/targets/InputEventPump.md for why the trace stops
//  here rather than at CFrame/CSimpleTop.
//============================================================================
#include "inputeventpump.h"

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }               \
    }

//  0x6F632460 - `__fastcall(ecx=handler, edx=msgType)` plus one stack
//  pointer to the payload (`retn 4`) per its own dump.
ADDR_THUNK(void __fastcall PostQueuedInputMessage(void* handler, int msgType,
                                                  const void* payload),
           0x6F632460)

//  0x6F630EB0 - a bare `jmp sub_6F62ABE0` in the shipped body (itself a
//  further `jmp sub_6F62CF80`, confirmed `retn` with 0 stack args - a
//  Storm-generic "signal every waiting handle" broadcast, `SetEvent`
//  through a small registered list, entirely unrelated to mouse routing).
//  No arguments, no stack cleanup either side.
ADDR_THUNK(void QuitBroadcastRelay(), 0x6F62ABE0)

#undef ADDR_THUNK
