//============================================================================
//  0x6F0F0EA0 - CAbilityInventory::ApplyPowerup: hand `item` to whoever owns
//  this ability (SAttachmentOwnerQuery::QueryOwner, cached at +0x30 the same
//  way ability.h's CAbility caches its own m_field30) and, when notifying,
//  play the pickup sound only if the recipient the effect just applied to is
//  the *locally controlled* one - the acting player-slot word at
//  g_unk6FAB65F4+0x28 compared against the recipient's own vtable slot 25
//  answer.
//
//  Own translation unit: SItemPowerupSource::ApplyPowerupEffect (0x6F2B9600,
//  itemapplypowerupeffect.cpp) is a real out-of-line call, and
//  PlayInterfaceSound is Storm's own (this batch's jassnatives_thunks.cpp
//  already declares the shape `PlayInterfaceSound(const char*)` - see
//  funcmap's own `sub_6F32F980` row).
//
//  ApplyPowerupEffect's `this` is `item`, not this inventory - the shipped
//  call site loads ecx from ApplyPowerup's own item argument right before
//  the call, not from anything on `this`.  Its own `notify` argument
//  (ApplyPowerup's second parameter) is never forwarded; the two `1`
//  constants and the `-1` are opaque past what the dump itself shows
//  (0x6F2B9600's own body never reads the middle one back), and
//  `[this+0xB8]` is past every field this call tree otherwise touches on
//  CAbilityInventory (itself only a partial view of CAbility - see
//  unitinventory.h), so it is read by raw offset rather than invented as a
//  named member.
//
//  The recipient's vtable slot 25 (0x64/4) answer is compared against the
//  *full* zero-extended acting-slot word, not truncated back down - the
//  shipped code widens with `movzx` once and never narrows the call's own
//  `eax` result before the `cmp`.
//============================================================================
#include "unitinventory.h"
#include "itemhandlemain.h"

struct SAttachmentOwnerQuery { void* __thiscall QueryOwner(); };

//  0x6F2B9600 - see itemapplypowerupeffect.cpp.  `this` is the item, not
//  the recipient; the recipient travels as the first stack argument.  A
//  local receiver type rather than a free `__thiscall` declaration, the
//  same shape SAttachmentOwnerQuery itself uses right above.
struct SItemPowerupSource
{
    int __thiscall ApplyPowerupEffect(void* recipient, int arg4, int arg8,
                                       int argC, int arg10);
};

//  PlayInterfaceSound - already declared for this batch in
//  jassnatives_thunks.cpp's own header set; redeclared here rather than
//  pulled in through an unrelated include.
int __fastcall PlayInterfaceSound(const char* name);

//  No explicit return value anywhere in the shipped body - the trailing
//  `retn 8` leaves whatever was last in `eax` (PlayInterfaceSound's own
//  return, or the vtable-slot-25 answer on the non-notify path), so this is
//  `void`, not the `int` its old naked-thunk declaration guessed.
void CAbilityInventory::ApplyPowerup(void* item, int notify)
{
    void* recipient = m_cachedOwner;
    if (!recipient)
        recipient = ((SAttachmentOwnerQuery*)this)->QueryOwner();

    int fieldB8 = *(int*)((char*)this + 0xB8);

    ((SItemPowerupSource*)item)->ApplyPowerupEffect(recipient, 1, -1, 1, fieldB8);

    if (notify)
    {
        typedef int (__thiscall *QuerySlot25Fn)(void*);
        int actingSlot = *(unsigned short*)((char*)g_unk6FAB65F4 + 0x28);
        QuerySlot25Fn slot25 = *(QuerySlot25Fn*)(*(void***)recipient + 25);
        if (actingSlot == slot25(recipient))
            PlayInterfaceSound("ItemGet");
    }
}
