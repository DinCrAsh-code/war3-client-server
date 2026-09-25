//============================================================================
//  Five converting constructors of the "type-checked smart pointer to a
//  CAgent-derived object" family agenttypedslots.h documents - the same
//  three statements (`m_value = 0; __try { Assign(candidate); } __finally
//  { if (AbnormalTermination()) Release(); }`) every member of that family
//  already has, reached from CUnit's own vtable-closure BFS
//  (cunit_agent4_worklist.md) and assigned to this agent (cunit-agent4).
//
//      0x6F077AC0  SAgent4CheckedSlot_6F077AC0(CAgent*)  -> Assign 0x6F075DF0
//      0x6F0E7410  SAgent4CheckedSlot_6F0E7410(CAgent*)  -> Assign 0x6F0DEB90
//      0x6F1A4C40  STypeCheckedRefHolder(CAgent*)        -> Set    0x6F19B800
//      0x6F280D80  SAgent4CheckedSlot_6F280D80(CAgent*)  -> Assign 0x6F275530
//      0x6F281080  SAgent4CheckedSlot_6F281080(CAgent*)  -> Assign 0x6F275930
//
//  0x6F19B800 is already reconstructed (Unit/unit_fixedtypeidgetter.cpp,
//  STypeCheckedRefHolder::Set) - that file's own header even names this
//  address as the one that reaches it ("Reached only through 0x6F1A4C40,
//  an __except_handler4-shaped SEH wrapper this toolchain cannot
//  reproduce ... left `pending` on its own account"), so the struct is
//  redeclared here (Set()'s own signature has to match exactly for the
//  mangled name to bind to the same symbol; a local redeclaration is the
//  same idiom Unit/unitordermake.cpp already uses for
//  SOptionalHandleRefResolver) with the constructor and Release() this
//  agent adds. The other four Assign addresses are out of this agent's own
//  60-address worklist - each is a different agile-type record's own
//  Assign, belonging to a sibling slice of the same 480-function CUnit BFS
//  split - and are redirected with a naked "Agent4"-prefixed thunk rather
//  than reconstructed here (src/Item/abilityfieldflagclosure_thunks.cpp's
//  own established convention), `retn 4` read directly off each one's own
//  raw_asm.
//
//  All five score the same low, already-established ceiling every
//  constructor in this family does (11 of 43 for SCheckedItemSlot's own
//  converting constructor) for the identical, already-documented reason:
//  docs/msvc-vc8-idioms.md, "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce" - the dump's own single-`SEH_<addr>`/
//  cookie-XOR-esp/`__CxxFrameHandler3` frame is not one this repo's fixed
//  `/GS- /EHs-c-` compiler emits for a `__try`/`__finally`, which instead
//  gets the older, EBP-framed, cookie-less `__except_handler3` shape - see
//  that doc section for the full explanation and the hand-tested compiler-
//  flag matrix behind it.
//============================================================================
#include "agent.h"
#include <excpt.h>   // AbnormalTermination()

//  See src/Item/abilityfieldflagclosure_thunks.cpp for the established
//  naked-redirect convention this follows.  "Agent4" infix so a sibling
//  slice's own worklist cannot pick an identical mangled name for the very
//  same out-of-scope address.
#define AGENT4_THUNK(sig, addr)         \
    __declspec(naked) sig               \
    {                                    \
        __asm { mov eax, addr }         \
        __asm { jmp eax }                \
    }

static void ReleaseHeld(CAgent* held)
{
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

//----------------------------------------------------------------------------
//  0x6F077AC0.
//----------------------------------------------------------------------------
struct SAgent4CheckedSlot_6F077AC0
{
    SAgent4CheckedSlot_6F077AC0(CAgent* candidate);
    void __thiscall Assign(CAgent* candidate);   // 0x6F075DF0 - out of scope
    void Release() { ReleaseHeld(m_value); }
    CAgent* m_value;
};
AGENT4_THUNK(void __thiscall SAgent4CheckedSlot_6F077AC0::Assign(CAgent*), 0x6F075DF0)

SAgent4CheckedSlot_6F077AC0::SAgent4CheckedSlot_6F077AC0(CAgent* candidate)
{
    m_value = 0;
    __try
    {
        Assign(candidate);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//----------------------------------------------------------------------------
//  0x6F0E7410.
//----------------------------------------------------------------------------
struct SAgent4CheckedSlot_6F0E7410
{
    SAgent4CheckedSlot_6F0E7410(CAgent* candidate);
    void __thiscall Assign(CAgent* candidate);   // 0x6F0DEB90 - out of scope
    void Release() { ReleaseHeld(m_value); }
    CAgent* m_value;
};
AGENT4_THUNK(void __thiscall SAgent4CheckedSlot_6F0E7410::Assign(CAgent*), 0x6F0DEB90)

SAgent4CheckedSlot_6F0E7410::SAgent4CheckedSlot_6F0E7410(CAgent* candidate)
{
    m_value = 0;
    __try
    {
        Assign(candidate);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//----------------------------------------------------------------------------
//  0x6F1A4C40 - STypeCheckedRefHolder's own converting constructor.  See
//  Unit/unit_fixedtypeidgetter.cpp for Set() itself (0x6F19B800, already
//  reconstructed - this is the caller that file's own header names).
//----------------------------------------------------------------------------
struct STypeCheckedRefHolder
{
    STypeCheckedRefHolder(CAgent* candidate);
    void* __thiscall Set(void* candidate);   // 0x6F19B800
    void Release() { ReleaseHeld((CAgent*)m_ref); }
    void* m_ref;   // +0x00
};

STypeCheckedRefHolder::STypeCheckedRefHolder(CAgent* candidate)
{
    m_ref = 0;
    __try
    {
        Set(candidate);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//----------------------------------------------------------------------------
//  0x6F280D80.
//----------------------------------------------------------------------------
struct SAgent4CheckedSlot_6F280D80
{
    SAgent4CheckedSlot_6F280D80(CAgent* candidate);
    void __thiscall Assign(CAgent* candidate);   // 0x6F275530 - out of scope
    void Release() { ReleaseHeld(m_value); }
    CAgent* m_value;
};
AGENT4_THUNK(void __thiscall SAgent4CheckedSlot_6F280D80::Assign(CAgent*), 0x6F275530)

SAgent4CheckedSlot_6F280D80::SAgent4CheckedSlot_6F280D80(CAgent* candidate)
{
    m_value = 0;
    __try
    {
        Assign(candidate);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//----------------------------------------------------------------------------
//  0x6F281080.
//----------------------------------------------------------------------------
struct SAgent4CheckedSlot_6F281080
{
    SAgent4CheckedSlot_6F281080(CAgent* candidate);
    void __thiscall Assign(CAgent* candidate);   // 0x6F275930 - out of scope
    void Release() { ReleaseHeld(m_value); }
    CAgent* m_value;
};
AGENT4_THUNK(void __thiscall SAgent4CheckedSlot_6F281080::Assign(CAgent*), 0x6F275930)

SAgent4CheckedSlot_6F281080::SAgent4CheckedSlot_6F281080(CAgent* candidate)
{
    m_value = 0;
    __try
    {
        Assign(candidate);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}
