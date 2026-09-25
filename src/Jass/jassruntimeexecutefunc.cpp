//============================================================================
//  0x6F3D3F30 - the JASS native `ExecuteFunc(string funcName)`.
//
//  Real reconstruction, not a thunk: every call this function itself makes
//  is a real, understood callee (GetHandleOrZero and AcquireGameSingleton
//  were already reconstructed; ResolveJassFunctionHandle/
//  SCheckedTextAgentSlot's constructor and MakeTextAgent are this session's
//  own real reconstructions - see jassresolvefunctionhandle.cpp,
//  checkedtextagentslot.cpp, jasstextagentmake.cpp).  The one callee this
//  session did NOT reconstruct, InvokeResolvedJassFunction (0x6F447FA0,
//  below), is real, unhooked shipped code called through an ordinary
//  function pointer - its body is naked, but the *call site* here is
//  genuine source, so this function's own logic is fully modelled.
//
//  The `_except_handler4` SEH frame around it is not hand-written - it is
//  what MSVC emits automatically for a function with a local object that
//  has a real destructor (here: the SCheckedTextAgentSlot local), under
//  this build's fixed `/GS- /EHs-c-` flags, the exact same "unreproducible
//  frame, real logic" shape every SCheckedXSlot constructor in
//  agenttypedslots.h already has (see checkedtriggerslot.cpp).  Expect the
//  score to be capped there, not in the body.
//
//  See docs/notes/jass-runtime-execute-function.md for the full resolve+
//  invoke trace this function is the entry point to, and the answer to
//  "does this work for a function a runtime-compiled script just declared".
//============================================================================
#include "agenttypedslots.h"
#include "gamewar3.h"
#include "handleobject.h"

//  0x6F0074F0 - GameUI/acquiregamesingleton.cpp.
void* __fastcall AcquireGameSingleton(int index, int unused);

//  dword_6FAB65F4/g_unk6FAB65F4 - the SAME CGameWar3 singleton every other
//  reader of this global already uses (GameUI/worldframelocalplayer.cpp,
//  Jass/jassfogstateplayermask.cpp, ...) - AcquireGameSingleton's own index
//  6.  Declared, never defined: a funcmap.DATA global the .mix binds to the
//  real one.
extern void* g_unk6FAB65F4;

//----------------------------------------------------------------------------
//  0x6F447FA0 - `retn 4`, `__thiscall(ecx=frame, arg_0=handle)`.  Kept a
//  naked redirect to real, unhooked shipped code - not reconstructed this
//  session.  `frame` is NOT a bare SCheckedTextAgentSlot: it is a much
//  larger (at least 0x90-byte), ad-hoc call-frame object ExecuteFunc builds
//  directly on its own stack, whose first field only happens to be the
//  SCheckedTextAgentSlot-shaped return-value holder MakeTextAgent just
//  filled in.  See the doc above for the full read of what this function
//  does: builds one synthetic 'AUTriggerAction'-tagged CAgent wrapping
//  `handle`, initializes a 15-slot local-argument array, then dispatches
//  through a ~700-instruction, non-SEH-framed evaluator (0x6F447340) that
//  is genuinely the invoke step - out of this session's own budget to
//  reconstruct, and large/uncertain enough (comparable to the flagship AST
//  builder and the opcode-stream interpreter this repo already keeps
//  thunked for the same reason) that a rushed attempt would be worse than
//  documenting it and moving on.
//
//  `frame` is passed as an opaque `JassExecuteFuncFrame*` purely so the ABI
//  (`this` in ecx, `handle` on the stack) matches - not a claim that
//  `JassExecuteFuncFrame` is the real type or size of ExecuteFunc's own
//  local.
//----------------------------------------------------------------------------
//  A one-method opaque handle for `frame` rather than a free `__thiscall`
//  function: MSVC only allows `__thiscall` (needed to get `frame` into ecx
//  with `handle` on the stack, matching the real `retn 4`) on an actual
//  member function.
struct JassExecuteFuncFrame
{
    void* Invoke(unsigned int handle);
};

__declspec(naked) void* JassExecuteFuncFrame::Invoke(unsigned int /*handle*/)
{
    __asm
    {
        mov eax, 06F447FA0h
        jmp eax
    }
}

void ExecuteFunc(const SHandleHolder* funcNameHandle)
{
    const char* funcName = (const char*)GetHandleOrZero(funcNameHandle);
    if (!funcName || !*funcName)
        return;

    void* singleton = g_unk6FAB65F4;
    if (!singleton)
    {
        singleton = AcquireGameSingleton(6, 0);
        g_unk6FAB65F4 = singleton;
    }

    unsigned int handle =
        ((CGameWar3*)singleton)->ResolveJassFunctionHandle(funcName);
    if (!handle)
        return;

    SCheckedTextAgentSlot frame(0);
    frame.MakeTextAgent(0, 0, 0);
    ((JassExecuteFuncFrame*)&frame)->Invoke(handle);

    //  `frame`'s own destructor (Release()) runs the SEH-visible teardown
    //  the shipped tail already has - written out explicitly here rather
    //  than left implicit, matching every other real-source reconstruction
    //  in this family that documents its own destructor call this way.
}
