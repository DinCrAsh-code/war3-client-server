//============================================================================
//  0x6F3B5470 - jass-instance-targeting-D's own round.  The direct bridge
//  between "a script chain just finished compiling" and "there is now a
//  running JassInstance for it": construct a brand-new root JassInstance
//  for the script slot a compile pass just populated, then immediately run
//  (or, on a fresh instance, compile-and-run) its opcode stream.
//
//  This is CGameWar3's own reload driver's (sub_6F3AEF80,
//  docs/notes/jass-runtime-instance-targeting.md) one caller of this
//  function - `sub_6F3AEF80` feeds it `[this+4]`, the exact value
//  `LoadAndCompileCommonScripts` (jassvmbin2_sehthunks.cpp/
//  jscriptbin3_scriptloaderlogic.cpp) just returned, and stores what THIS
//  function returns straight into `CGameWar3::m_currentJassInstanceSlot`
//  (`CGameWar3+0x08`, gamewar3.h) - the exact field `ExecuteFunc`'s own
//  resolve chain (jassruntimeexecutefunc.cpp/jassresolvefunctionhandle.cpp)
//  reads on every call.  That write site is the answer to the open
//  question `docs/notes/jass-runtime-recompile-safety.md` and
//  `docs/notes/jass-runtime-execute-function.md` both independently
//  flagged and could not themselves find - see the write-up above for the
//  full trace.
//============================================================================
#include "jassinstance.h"
#include "jassthreadstate.h"

//  jassvmbatch4_thunks.cpp - `retn 0`, `__fastcall(scriptIndex in ecx)`.
//  Allocates+constructs (flag `1`) a brand-new root JassInstance from the
//  JassThreadLocal::m_scripts record at `scriptIndex`, auto-assigns it a
//  fresh JassThreadLocal instance-handle slot, and returns THAT slot index
//  (not the instance pointer - see this function's own updated comment).
unsigned int __fastcall Jass_ctor_SEH_6F455760(unsigned int scriptIndex);

//  jassrunorcompilestream.cpp - `retn 10h`.  Resolves `lookupKey` back to a
//  JassInstance via LookupInstanceBySlot and either resumes its already-
//  compiled opcode stream or compiles-and-runs one for the first time.
int __fastcall RunOrCompileStream(unsigned int lookupKey, void* param0,
                                   void* arg0, void* arg4, int resumeAllowed,
                                   void* argC);

//----------------------------------------------------------------------------
//  0x6F3B5470 - `retn 0`, `__fastcall(scriptIndex in ecx)`.  Returns the
//  new instance's own slot index on success, `0` on failure (either
//  Jass_ctor_SEH_6F455760 itself failing - an allocation failure this
//  closure never actually reaches in practice - or RunOrCompileStream not
//  returning exactly `1`).
//
//  The `sub eax,1 / neg eax / sbb eax,eax / not eax / and eax,esi` tail is
//  the classic "collapse to an all-1s/all-0s mask, then AND" idiom for
//  `result == 1 ? esi : 0` without a conditional branch - reproduced here
//  as the equivalent ternary, which is how docs/msvc-vc8-idioms.md already
//  catalogues this exact shape elsewhere in this repo.
//
//  The hardcoded "config" name (as opposed to, say, the map/script's own
//  name) is transcribed as-is from the dump; nothing in this closure
//  explains why this one caller always passes that literal string rather
//  than a name derived from the script that was just compiled - flagged
//  in the write-up rather than guessed at further.
//----------------------------------------------------------------------------
unsigned int __fastcall GetOrRunJassInstanceForScript(unsigned int scriptIndex)
{
    unsigned int slot = Jass_ctor_SEH_6F455760(scriptIndex);
    if (slot == 0)
        return 0;

    int result = RunOrCompileStream(slot, (void*)"config", 0, (void*)1, 1,
                                     (void*)0x493E0);
    return (result == 1) ? slot : 0;
}
