//============================================================================
//  sub_6F473170 (0x6F473170) - resolve a {handle, typeTag} pair (at `this`
//  +8/+0xC) and return the looked-up object's own +0x78 field. Nothing in
//  this call tree says what class owns the pair or what the +0x78 field is.
//
//  SModifierSubject::AddValueListener (0x6F471A30) - a virtual-call
//  adjuster thunk: reads a vtable pointer at `this+0x14`, tail-calls that
//  sub-object's own slot 2 with `this` rebased by +0x14. The callee is
//  reached only indirectly (through a vtable this call tree does not
//  otherwise resolve), so this is reproduced as the literal register
//  operations rather than a named virtual call - but the *declaration* is
//  a real one now (positionmodifier.h), because its one caller,
//  FloatListener::Init (src/Unit/floatlistenerinit.cpp), has to push three
//  stack words into it and a nullary `void()` would have pushed none.
//============================================================================
#include "game.h"
#include "positionmodifier.h"

//----------------------------------------------------------------------------
//  0x6F4A8980 - the notification SetField78 below issues.  Its own module
//  (0x6F4Axxxx) and a real call, so its body is in handlefieldnotify.cpp;
//  the type is re-declared identically here, which is what keeps the two
//  mangled names the same symbol.
//----------------------------------------------------------------------------
struct SPublishedFieldOwner
{
    void NotifyField78Changed(int oldValue, void* listener);
};

struct HandleRefFieldOwner_6F473170
{
    char pad[8];
    unsigned int m_handle;   // +8
    int          m_typeTag;  // +0xC

    int QueryField78();
    //  0x6F473140 - the matching writer.  See below.
    void SetField78(int value);
};

int HandleRefFieldOwner_6F473170::QueryField78()
{
    CHandleObject* obj = LookupHandle(m_handle, m_typeTag);
    return *(int*)((char*)obj + 0x78);
}

//----------------------------------------------------------------------------
//  0x6F473140 - QueryField78's writer, and the reason the pair is
//  interesting: the field is *published*, so the write is followed by a
//  notification carrying the old value.
//
//  The old value is read, the new one stored, and only then is the notify
//  issued - which is why the shipped code loads `[eax+78h]` into ecx and
//  pushes it before the `mov [eax+78h],edx` that overwrites it.  Written
//  as a named `old` for exactly that ordering; reading the field inside
//  the call's argument list instead re-reads it after the store.
//
//  The notify's second argument is a listener, and this caller always
//  passes none - so the field's own broadcast path is the one that runs.
//  See handlefieldnotify.cpp.
//----------------------------------------------------------------------------
void HandleRefFieldOwner_6F473170::SetField78(int value)
{
    CHandleObject* obj = LookupHandle(m_handle, m_typeTag);

    int old = *(int*)((char*)obj + 0x78);
    *(int*)((char*)obj + 0x78) = value;

    ((SPublishedFieldOwner*)obj)->NotifyField78Changed(old, 0);
}

//  Naked and tail-jumping, so nothing here cleans the three stack words -
//  whatever slot 2 really is does, exactly as it would have if the caller
//  had reached it directly.  That is why there is no `ret` and why
//  tools/thunk_abi_audit.py has nothing to check: the bytes this function
//  leaves on the stack are the caller's own pushes, untouched.
__declspec(naked) void SModifierSubject::AddValueListener(unsigned int,
                                                          unsigned int,
                                                          void*)
{
    __asm
    {
        mov eax, [ecx+14h]
        mov eax, [eax+8]
        add ecx, 14h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  CUnit batch-15.  0x6F3DF1C0 - `add ecx, 0C8h` / `jmp sub_6F473170`, the
//  same "member call on an embedded sub-object" tail-call shape
//  unit.h's own SUnitTrackedRef adjustors use, but here the target is a
//  fixed function rather than an indirect vtable slot, so this one is a
//  static (not virtual) adjustor. It hands whatever object `ecx` names off
//  to QueryField78 on the HandleRefFieldOwner_6F473170 embedded at +0xC8.
//  The owning class is not established anywhere in this call tree (both
//  call sites reach it through sub_6F41B420's own unidentified return
//  value), so this is reproduced as the literal two-instruction tail jump
//  rather than a named member on a guessed class - naked, so the score is
//  1.000 by construction.
//
//  `__fastcall`, not the file's implicit `__cdecl`: this had no caller
//  until Widget/playertableunitfogrefresh.cpp's own `SJassRelationAgent`
//  read (0x6F40A650's `mov ecx, eax` / `call sub_6F3DF1C0`), which passes
//  the object in `ecx` with no stack argument, matching the naked body's
//  own `add ecx, 0C8h` untouched - `__fastcall` with one integer argument
//  puts it in ecx and nothing on the stack, which is byte-for-byte the same
//  call site a `thiscall` member with no stack arguments would produce.
//----------------------------------------------------------------------------
__declspec(naked) int __fastcall QueryField78At0xC8(void*)
{
    __asm
    {
        add ecx, 0C8h
        jmp HandleRefFieldOwner_6F473170::QueryField78
    }
}
