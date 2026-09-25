//============================================================================
//  0x6F29EE30 - CUnit, `this` in ecx, one stack int, `retn 4`.
//
//  Looks up the fixed-rawcode ability 0x42707276 through
//  SUnitAbilityHost::FindAbility (0x6F0787D0, unit_findability.cpp,
//  reused verbatim - same class, same call), then reads a
//  {handle, typeTag} pair eight bytes into whatever it finds and resolves
//  that through LookupHandle (0x6F03FA30, Game/game.h).  If the resolved
//  object's own kind tag is the 'lga+' (0x2B61676C) constant
//  jasssetitemposition_native.cpp's chain already checks against, and its
//  SItemHandleObject::m_pendingFlag is clear, the ability itself is told
//  about the caller's argument through a member this file does not
//  reconstruct (sub_6F0D6410 - see below) and the function returns.
//
//  Otherwise: fetch a second, unrelated singleton (0x6F29B630 - its own
//  `__except_handler4`-shaped SEH frame, the same unreproducible shape
//  docs/msvc-vc8-idioms.md already catalogues for GetGameUI, so it stays a
//  redirect), dispatch through its own vtable slot 0xD5 (`+0x354`) with
//  (this, arg), then tell `this` about the singleton through a second
//  unreconstructed member (sub_6F079CA0).
//
//  Neither the fixed rawcode, the singleton's real class, nor slot 0xD5's
//  real target are established by this call tree - no vtable dump for
//  whatever 0x6F29B630 returns is committed, so the dispatch is judged
//  "unknown" by vtable_dispatch_audit.py rather than checked; what is
//  certain is the exact instruction shape, reproduced field for field.
//============================================================================
#include "game.h"
#include "itemhandleresolve.h"   // SOptionalHandleRef
#include "unit.h"   // CUnit::NotifyAbilityManagerAndGameUI (0x6F079CA0), a real reconstruction now

//  Re-declared to match unit_findability.cpp's own definition exactly -
//  same class, same member signature - so MSVC mangles this call onto
//  that file's real body instead of inventing a second symbol the linker
//  never joins (tools/link_check.py's first defect class).
struct SUnitAbilityHost
{
    void* FindAbility(int rawcode, int a, int b, int c, int d);

    void GrantFlag_6F29EE30(int arg0);
};

//  0x6F29B630 - a lazily-constructed singleton behind the same
//  `__except_handler4`-shaped SEH frame GetGameUI (0x6F300710) carries;
//  kept a redirect for the same reason.  `retn 0`, no arguments.
void* GetAbilityTemplateSingleton_6F29B630();

//  The {handle, typeTag} pair FindAbility's return carries eight bytes in -
//  a second, per-ability instance of the same pair shape
//  itemhandleresolve.h already names, at an offset SUnitAbility (this
//  function's own candidate struct in unit_findability.cpp) does not
//  itself list because nothing there reads it.
struct SAbilityHandleCarrier
{
    char               m_reserved00[0x0C];
    SOptionalHandleRef m_handleRef;   // +0x0C/+0x10

    //  0x6F0D6410 (tail-jumps into 0x6F0D6380) - a real reconstruction now,
    //  defined in Unit/unit_abilitygrantflagnotify.cpp (its own module,
    //  address-adjacent, not this one's).  `retn 4`.
    void GrantFlag_6F0D6410(int arg0);
};

void SUnitAbilityHost::GrantFlag_6F29EE30(int arg0)
{
    void* ability = FindAbility(0x42707276, 0, 0, 1, 1);
    if (ability != 0)
    {
        SAbilityHandleCarrier* carrier = (SAbilityHandleCarrier*)ability;
        CHandleObject* obj = LookupHandle(carrier->m_handleRef.m_handle,
                                           carrier->m_handleRef.m_typeTag);
        if (obj != 0)
        {
            SItemHandleObject* item = (obj->m_kindTag == 0x2B61676C)
                                           ? (SItemHandleObject*)obj : 0;
            if (item != 0)
            {
                if (item->m_pendingFlag == 0)
                {
                    carrier->GrantFlag_6F0D6410(arg0);
                    return;
                }
            }
        }
    }

    void* singleton = GetAbilityTemplateSingleton_6F29B630();
    typedef void (__thiscall *Slot354Fn)(void*, void*, int);
    ((Slot354Fn)(*(void***)singleton)[0x354 / 4])(singleton, this, arg0);
    ((CUnit*)this)->NotifyAbilityManagerAndGameUI(singleton);
}

//----------------------------------------------------------------------------
//  Thunks for the callees this file declares and calls but does not
//  reconstruct.  See the notes above each declaration for why.
//----------------------------------------------------------------------------
#define ABILITY_THUNK(addr)      \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

__declspec(naked) void* GetAbilityTemplateSingleton_6F29B630()             ABILITY_THUNK(0x6F29B630)
