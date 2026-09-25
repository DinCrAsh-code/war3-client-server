//============================================================================
//  0x6F2E52F0 - CUnit::NotifyAbilityHostChanged.  `this` in ecx, no stack
//  arguments, `retn 0`.
//
//  Bails immediately on a null `this`, on flag bit 0x100 of +0x5C already
//  set, and on vtable slot 69 (+0x114, `Method_0x114`) reporting bit 0x10.
//  Then, if the unit's own classification (vtable slot 59,
//  `GetOwningPlayerIndex`) reads back 0xC and flag bit 4 of +0x248 is not
//  yet set, it kicks a bookkeeping call (0x6F2E51D0 - out of scope, its own
//  callee closure runs into a whole other module).
//
//  From there: look up the fixed-rawcode ability 0x41676D62 through
//  SUnitAbilityHost::FindAbility (0x6F0787D0, unit_findability.cpp, reused
//  verbatim), and if found, tell the unit about it through a second
//  out-of-scope call (0x6F2E5030).
//
//  Then resolve the *owning player's own* handle-ref pair
//  (SPlayerScore::m_ref2D4, playerscore.h) through QueryHandleField0x54
//  (0x6F4786B0, itemhandleresolve.h, already declared) and, if that
//  resolves, notify the resolved object of this unit (0x6F2DBB90 - its own
//  `__except_handler4`-shaped SEH frame, kept a redirect for the usual
//  reason) and, the first time only (flag bit 4 of +0x5C), toggle the
//  ability chain's own listener registration (0x6F28E2C0 -
//  NotifyAbilityChainListenerToggle(1), unit_abilitychainlistentoggle.cpp,
//  a real reconstruction now - see unit.h).
//
//  If the owning player's own pair does *not* resolve, and an ability was
//  found above, walk every one of the twelve player slots instead and, for
//  each whose own handle-ref pair resolves, tell it about the ability
//  through 0x6F2AB3E0.
//
//  Four of the five numbered callees stay unreconstructed redirects: each
//  is outside this function's own dump (0x6F2E51D0/0x6F2E5030/0x6F2AB3E0
//  reach further modules of their own; 0x6F2DBB90 carries the same
//  unreproducible SEH frame `docs/msvc-vc8-idioms.md` already catalogues).
//  Every `retn` below is read off its own `ida_query calltree_asm` answer,
//  not guessed.
//============================================================================
#include "unit.h"
#include "playerscore.h"

//  Re-declared to match Misc/misc_field_getters.cpp exactly, so the calls
//  below bind to that definition rather than inventing a second symbol
//  (tools/link_check.py's first defect class).
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//  dword_6FAB65F4 - the world-object singleton (Unit/unit_publishposition.cpp
//  and friends).  Declared, never defined: the real game's global.
extern void* g_unk6FAB65F4;

//  0x6F082470 - already reconstructed as GetConstant_6F082470
//  (Misc/misc_rawcode_getters.cpp).
extern unsigned int GetConstant_6F082470();

//  Re-declared to match unit_findability.cpp's own definition exactly -
//  same class, same member signature - so MSVC mangles this call onto
//  that file's real body instead of inventing a second symbol
//  (tools/link_check.py's first defect class).
struct SUnitAbilityHost
{
    void* FindAbility(int rawcode, int a, int b, int c, int d);
};

//  The five callees this function reaches but does not reconstruct - see
//  the file header for why each stays a redirect.  A local, minimal view
//  of `this` rather than five more declarations on CUnit itself
//  (unit.h): none of them touch a field, only the receiver pointer.
//
//  0x6F2AB3E0 is the exception: reconstructed for real as
//  SEventNotifyOrForwardHost::NotifyOrForwardEvent
//  (Misc/misc_notifyorforward_event.cpp, EXACT) - declared here as that
//  same struct/method rather than a sixth locally-invented name, so this
//  call binds to the one real body instead of manufacturing a second
//  naked thunk at an address this repo already has a definition for
//  (tools/link_check.py's first defect class).
struct CUnitAbilityNotifyCallees
{
    void Notify_6F2E51D0();             // 0x6F2E51D0, retn 0
    void Notify_6F2E5030();             // 0x6F2E5030, retn 0
    void Notify_6F2DBB90(void*);        // 0x6F2DBB90, retn 4
};

struct SEventNotifyOrForwardHost
{
    void NotifyOrForwardEvent(void* target, int flag);   // 0x6F2AB3E0
};

void CUnit::NotifyAbilityHostChanged()
{
    if (this == 0)
        return;
    if (m_flags5C & 0x100)
        return;
    if (Method_0x114() & 0x10)
        return;

    CUnitAbilityNotifyCallees* self = (CUnitAbilityNotifyCallees*)this;

    if (GetOwningPlayerIndex() == 0x0C && !(m_int248 & 4))
        self->Notify_6F2E51D0();

    SUnitAbilityHost* host = (SUnitAbilityHost*)this;
    void* ability = host->FindAbility(GetConstant_6F082470(), 1, 0, 1, 1);
    if (ability != 0)
        self->Notify_6F2E5030();

    IndexedArrayHolder_6F3A1650* world = (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
    SPlayerScore* owner = (SPlayerScore*)world->At(GetOwningPlayerIndex());
    void* ownerObj = 0;
    if ((*(int*)((char*)owner + 0x2D8) & *(int*)((char*)owner + 0x2D4)) != -1)
        ownerObj = QueryHandleField0x54((SOptionalHandleRef*)((char*)owner + 0x2D4));

    if (ownerObj == 0)
    {
        if (ability != 0)
        {
            for (unsigned int i = 0; i < 0x0C; i++)
            {
                SPlayerScore* player = (SPlayerScore*)
                    ((IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4)->At(i);
                void* obj = 0;
                if ((*(int*)((char*)player + 0x2D8) & *(int*)((char*)player + 0x2D4)) != -1)
                    obj = QueryHandleField0x54((SOptionalHandleRef*)((char*)player + 0x2D4));
                if (obj != 0)
                    ((SEventNotifyOrForwardHost*)self)->NotifyOrForwardEvent(obj, 1);
            }
        }
        return;
    }

    ((CUnitAbilityNotifyCallees*)ownerObj)->Notify_6F2DBB90(this);
    if (!(m_flags5C & 4))
    {
        m_flags5C |= 4;
        NotifyAbilityChainListenerToggle(1);
    }
}

//----------------------------------------------------------------------------
//  Thunks for the callees this file declares and calls but does not
//  reconstruct.  See the file header for why.
//----------------------------------------------------------------------------
#define NOTIFY_THUNK(addr)       \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

__declspec(naked) void CUnitAbilityNotifyCallees::Notify_6F2E51D0()            NOTIFY_THUNK(0x6F2E51D0)
__declspec(naked) void CUnitAbilityNotifyCallees::Notify_6F2E5030()            NOTIFY_THUNK(0x6F2E5030)
__declspec(naked) void CUnitAbilityNotifyCallees::Notify_6F2DBB90(void*)       NOTIFY_THUNK(0x6F2DBB90)
