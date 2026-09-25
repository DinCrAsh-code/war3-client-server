//============================================================================
//  The JASS VM's own opcode-stream execution primitive - the concrete
//  answer to "how does it execute instructions": JassInstance::m_streamCursor
//  (+0x20, jassinstance.h) is the instruction pointer, and
//  ExecuteOpcodeStream (0x6F45E9D0, kept THUNK below) is the interpreter
//  loop that walks it, a 42-way `switch` on one opcode byte per step
//  (`movzx ecx, byte ptr [edi+3]; ...; jumptable 6F45EA57`), advancing the
//  cursor 4 bytes per decoded opcode.
//============================================================================
#ifndef JASSEXECUTESTREAM_H
#define JASSEXECUTESTREAM_H

#include "jassinstance.h"

//  0x6F45E9D0 - kept THUNK: real, unhooked shipped code.  1116
//  instructions, a genuine opcode-stream interpreter (not a
//  register/scheduling-only diff from something simpler - see
//  docs/targets/JassVM.md's own note on the even bigger flagship AST
//  evaluator, 0x6F464460, for the sibling case this fixed `/GS- /EHs-c-`
//  toolchain already gave up on for the same reason), but opens with the
//  identical push-0FFFFFFFFh/push-offset-SEH_.../mov-eax,large-fs:0
//  `_except_handler4` prologue and closes through `__CxxFrameHandler3`
//  that flagship function does - unreproducible from C++ source under
//  this build's fixed flags, per pipeline/CLAUDE.md's own SEH note next
//  to JassInstance::Construct/Run.  `retn 10h` (four stack args, `this`
//  in ecx) - confirmed from the real `retn` byte, not assumed.
//----------------------------------------------------------------------------
//  Declared on JassInstance itself (jassinstance.h) as ExecuteOpcodeStream -
//  stack args (cursor, arg2, stepBudget, arg4) in the order
//  RunOpcodeStream below pushes them.

#endif
