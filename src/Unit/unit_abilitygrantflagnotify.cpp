//============================================================================
//  0x6F0D6410 / 0x6F0D6380 - one module: 0x6F0D6410 tail-jumps straight
//  into 0x6F0D6380 with no `call`, so they are one logical function laid
//  out across two addresses (0x6F0D6410 is the "or a flag bit in, then
//  always notify with activate=1" entry; 0x6F0D6380 is the general
//  "resolve the ability's owner and (de)notify" body every other caller
//  reaches directly).
//
//  Reached from SUnitAbilityHost::GrantFlag_6F29EE30's own
//  `carrier->GrantFlag_6F0D6410(arg0)` (Unit/unit_abilitygrantflag.cpp,
//  which declares the same SAbilityHandleCarrier class - re-declared here
//  with the extra fields this module's own body touches).
//
//  0x6F0D6380: `this` is the ability record itself (SAbilityHandleCarrier).
//  Its own +0x30 is a cached "owner" pointer - the same
//  SAttachmentTarget::Show/QueryOwner shape Widget/attachmentcounters.cpp
//  and Unit/unit_gameuiabilitylistenernotify.cpp's own 0x6F0D1B90 already
//  establish.  Once resolved: LookupHandle the owner's own {handle,typeTag}
//  pair at +0xC/+0x10 into a second object, insist it carries the 'lga+'
//  tag and has its own +0x20 gate clear (the identical
//  SItemHandleObject-shaped check GrantFlag_6F29EE30 makes), then OR bit
//  0x400 into the *owner's* own +0x60 flags.  If `activate` is nonzero,
//  tell the owner about this ability's own +0xCC flag word through
//  0x6F2AF7E0 (Widget/widget_ghostimagemaskclear.cpp) and the per-player
//  ability-count module at dword_6FAB65F4->m_ref34 through the
//  already-thunked SPlayerTableSubmitParts::ApplyUnitFogRadius
//  (0x6F409E00, Widget/playertableunitfogrefresh.cpp).  Either way,
//  republish a masked copy of +0xCC into the owner's own +0x2C flags.
//
//  0x6F0D6410: OR the caller's word-sized flag into `this`'s own +0xCC,
//  then tail-call the body above with activate forced to 1 - the shipped
//  `mov [esp+arg_0], 1` overwrite of its own single stack argument in
//  place before the `jmp`.
//============================================================================
#include "itemhandleresolve.h"   // SOptionalHandleRef, QueryHandleField-style resolve
#include "unit.h"                // CUnit, for the already-thunked ApplyUnitFogRadius call only
#include "../Widget/playertablefog.h"   // SPlayerTableSubmitParts::ApplyUnitFogRadius

//  Re-declared to match Widget/widget_ghostimagemaskclear.cpp's own
//  receiver exactly - neither file claims a real class for whatever object
//  this call tree resolves the owner to.
struct CGhostImageMaskOwner
{
    void ClearGhostImageRenderMask(unsigned short mask);   // 0x6F2AF7E0
};

class CHandleObject;
CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag);   // Game/game.h

//  Re-declared to match Widget/attachmentcounters.cpp's own
//  SAttachmentOwnerQuery exactly, so QueryOwner binds onto that real body.
struct SAttachmentOwnerQuery
{
    void* __thiscall QueryOwner();   // 0x6F472890, already reconstructed
};

//  Re-declared to match Unit/unit_abilitygrantflag.cpp's own
//  SAbilityHandleCarrier - same class name, same {handle,typeTag} pair -
//  plus the extra fields this module's own body touches.
struct SAbilityHandleCarrier
{
    char               m_reserved00[0x0C];
    SOptionalHandleRef m_handleRef;   // +0x0C/+0x10
    char               m_reserved14[0x30 - 0x14];
    void*              m_pOwnerCache; // +0x30 - SAttachmentTarget-shaped cache
    char               m_reserved34[0xCC - 0x34];
    unsigned int       m_flags0xCC;   // +0xCC

    void GrantFlag_6F0D6410(int arg0);
    void NotifyOwnerOfGrant(int activate);
};

//  The resolved owner object: a CUnit-shaped receiver for the two
//  out-of-scope notify calls, plus the two flags words this body itself
//  touches directly.
struct SAbilityOwner
{
    char           m_reserved00[0x0C];
    SOptionalHandleRef m_handleRef;   // +0x0C/+0x10 - resolved via LookupHandle
    char           m_reserved14[0x2C - 0x14];
    unsigned short m_flags2C;         // +0x2C
    char           m_reserved2E[0x60 - 0x2E];
    unsigned int   m_flags60;         // +0x60
};

extern void* g_unk6FAB65F4;   // dword_6FAB65F4, the world-object singleton

//----------------------------------------------------------------------------
//  0x6F0D6380
//----------------------------------------------------------------------------
void SAbilityHandleCarrier::NotifyOwnerOfGrant(int activate)
{
    SAbilityOwner* owner = (SAbilityOwner*)m_pOwnerCache;
    if (owner == 0)
        owner = (SAbilityOwner*)((SAttachmentOwnerQuery*)this)->QueryOwner();
    if (owner == 0)
        return;

    CHandleObject* resolved = LookupHandle(owner->m_handleRef.m_handle,
                                            owner->m_handleRef.m_typeTag);
    if (resolved == 0)
        return;

    SAbilityOwner* item = (*(unsigned int*)((char*)resolved + 0x0C) == 0x2B61676Cu)
                              ? (SAbilityOwner*)resolved : 0;
    if (item == 0)
        return;
    if (*(int*)((char*)item + 0x20) != 0)
        return;

    owner->m_flags60 |= 0x400;

    unsigned int flags = (unsigned short)m_flags0xCC;
    SPlayerTableSubmitParts* module =
        *(SPlayerTableSubmitParts**)((char*)g_unk6FAB65F4 + 0x34);
    if (activate != 0)
    {
        ((CGhostImageMaskOwner*)owner)->ClearGhostImageRenderMask((unsigned short)flags);
        module->ApplyUnitFogRadius((CUnit*)owner, flags, 0, 0);
    }

    owner->m_flags2C |= (unsigned short)(flags & 0xFFF);
}

//----------------------------------------------------------------------------
//  0x6F0D6410 - tail-jumps into the body above with `activate` forced to 1.
//----------------------------------------------------------------------------
void SAbilityHandleCarrier::GrantFlag_6F0D6410(int arg0)
{
    m_flags0xCC |= (unsigned int)(unsigned short)arg0;
    NotifyOwnerOfGrant(1);
}
