//============================================================================
//  See jassexecutestream.h.
//============================================================================
#include "jassexecutestream.h"

//----------------------------------------------------------------------------
//  0x6F45E9D0 - kept THUNK, see jassexecutestream.h's own note.
//----------------------------------------------------------------------------
__declspec(naked) void* JassInstance::ExecuteOpcodeStream(void* /*cursor*/,
                                                            int /*arg2*/,
                                                            int /*stepBudget*/,
                                                            int /*arg4*/)
{
    __asm
    {
        mov eax, 06F45E9D0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F45FD00 - `retn 0`.  Return type is `int`, not `void` - see
//  jassinstance.h's own note above this declaration.
//----------------------------------------------------------------------------
int JassInstance::RunOpcodeStream()
{
    return (int)(long)ExecuteOpcodeStream(m_streamCursor, 0, 0x493E0, 0);
}
