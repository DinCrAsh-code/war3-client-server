//============================================================================
//  0x6F2AD7E0 - CWidget vtable[0x98] (index 38): tell the players who can
//  see this widget that something about it changed.
//
//  Self-contained - every call it makes is a virtual one, so this whole
//  target is one function.
//
//  What it establishes is the meaning of CWidget's two 16-bit fields at
//  +0x2C and +0x2E, which the CWidget constructor zeroes and nothing had
//  read back until now: they are **player bitmasks**, one bit per player,
//  and they are ANDed against a third mask the player-slot table carries at
//  its own +0x2C (the set of players that currently exist).  The recovered
//  set is then narrowed twice more - once by whatever vtable[0xF8] decides,
//  and once by removing the widget's own owning player, whose index
//  vtable[0xEC] returns - before vtable[0x110] is told about the rest.
//
//  Which of +0x2C and +0x2E is "can see" and which is "has seen" this call
//  tree cannot say; it only reads them.  Both are held as 16-bit values
//  throughout - every test is a `test ax,ax` on the low word, never a full
//  dword - so a mask is at most 16 players wide, which matches
//  ResolvePlayerColorSlot's own `player >= 16` cutoff (playercolor.cpp).
//============================================================================
#include "widget.h"

//  dword_6FAB65F4 - the player slot table.  Its +0x2C is a 16-bit mask of
//  the players that exist; see playercolor.h for why this symbol keeps the
//  name itemhandlemain.h gave it rather than getting a second one.
extern void* g_unk6FAB65F4;

typedef int  (__thiscall *FilterPlayersFn)(void*, unsigned int, unsigned int);
typedef int  (__thiscall *GetOwningPlayerFn)(void*);
typedef void (__thiscall *NotifyPlayersFn)(void*, unsigned int, int);

//  A member, not the `__fastcall F(CWidget*)` free function a previous
//  session wrote: it is vtable slot 38, and a vtable entry has to name a
//  member or the emitted vtable references a symbol nothing defines.  The
//  two spellings compile to the same code - `this` in ecx either way - so
//  only the mangled name changes.
void CWidget::NotifyWidgetVisibilityChanged()
{
    CWidget* self = this;
    void* vself = self;

    unsigned short existing = *(unsigned short*)((char*)g_unk6FAB65F4 + 0x2C);
    unsigned short maskA = self->m_playerMaskA;
    unsigned short maskB = self->m_playerMaskB;

    //  Zero-extended into full registers once and used as 32-bit values
    //  from here on, which is what the shipped `movzx` pair does.
    unsigned int live = (unsigned short)(existing & maskA);
    unsigned int notify = (unsigned short)(maskB & live);

    if ((unsigned short)(maskB & maskA) == 0)
        return;

    if (((FilterPlayersFn)(*(void***)vself)[0xF8 / 4])(vself, live, notify) == 0)
        return;

    if ((unsigned short)(notify & live) == 0)
        return;

    int owner = ((GetOwningPlayerFn)(*(void***)vself)[0xEC / 4])(vself);
    if (owner >= 0)
        notify &= ~(1 << owner);

    if ((unsigned short)notify != 0)
        ((NotifyPlayersFn)(*(void***)vself)[0x110 / 4])(vself, notify, 0);
}
