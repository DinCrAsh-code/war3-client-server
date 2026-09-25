//============================================================================
//  0x6F2F8E90 / 0x6F2F8F90 / 0x6F2F8FC0 / 0x6F2F9020 - the four per-player
//  queries CGameUI makes when it formats the pausing player's name.
//
//  All four start from the world-object singleton (`dword_6FAB65F4`) and
//  index its per-player slot array; none of them calls another, which is why
//  they share a translation unit.
//============================================================================
#include "gameui.h"
#include "gameuithunks.h"
#include "playercolor.h"        // g_unk6FAB65F4, and the slot's own layout

//----------------------------------------------------------------------------
//  0x6F3A1650 - IndexedArrayHolder_6F3A1650::At, already reconstructed and
//  EXACT (Misc/misc_field_getters.cpp).  Declared locally, the way every
//  other caller of it in this repo does: a second naked redirect at the same
//  address would be two symbols for one function and would cost that EXACT
//  body every call site it has.
//----------------------------------------------------------------------------
struct IndexedArrayHolder_6F3A1650
{
    char           m_reserved00[0x28];
    unsigned short m_actingSlot;    // +0x28
    int At(unsigned int index);
};

//  0x6F473170 - Misc/queryfield.cpp; the slot's own handle refs at +0xE0 and
//  +0xF0 answer "is this slot really taken".
struct HandleRefFieldOwner_6F473170 { int __thiscall QueryField78(); };

//  0x6F40B1C0 - Misc/trivialaccessors_04.cpp: the slot's +0x2A8 referee flag.
int __fastcall GetField0x2A8_6F40B1C0(const void* self);

//----------------------------------------------------------------------------
//  0x6F2F8E90 - the colour a player's name is drawn in.
//
//  A player whose slot's +0xF0 handle ref is set - i.e. one the UI is
//  deliberately not colouring - gets plain white; everyone else gets the
//  colour their palette entry holds.
//
//  The white constant goes through a named local whose *address* is taken
//  and then read back (`lea eax, [slot]` / `mov [slot], -1` / `mov eax,
//  [eax]`), which is what binding the returned temporary to a reference
//  produces and what a bare `return SPlayerColor(-1);` does not - see
//  docs/msvc-vc8-idioms.md, "Binding a temporary to a const reference
//  reuses the returned buffer".
//----------------------------------------------------------------------------
SPlayerColor __fastcall GetPlayerUiColor(unsigned int player)
{
    IndexedArrayHolder_6F3A1650* world =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;

    if (world != 0)
    {
        int slot = world->At(player);

        if (slot != 0
            && ((HandleRefFieldOwner_6F473170*)((char*)slot + 0xF0))
                   ->QueryField78() != 0)
        {
            //  Through a pointer, not returned directly: the shipped code
            //  forms the temporary's *address* and reads the value back
            //  through it (`lea` / `mov [slot], -1` / `mov eax, [eax]`),
            //  which a plain `return SPlayerColor(-1);` collapses into one
            //  store straight into the caller's buffer.
            SPlayerColor white(0xFFFFFFFFu);
            const SPlayerColor* through = &white;

            return *through;
        }
    }

    return *(const SPlayerColor*)GetPlayerColorValue(player);
}

//----------------------------------------------------------------------------
//  0x6F2F8F90 - the name a player is displayed under.
//
//  The fallback string is materialised into a register *before* the null
//  test, which is what one shared `return` of a file-scope constant gives.
//  It is the **empty string**, and that is read out of the binary rather
//  than guessed: IDA calls it `ValueName` (its own name for a string used as
//  a value-name argument elsewhere), `ida_query names ValueName` puts it at
//  0x6F87529C, and `ida_query bytes 0x6F87529C 24` shows four zero bytes
//  there.  Spelled as a literal and not given a DATA name, because a shared
//  constant that several modules pass is exactly what must not be named
//  (docs/msvc-vc8-idioms.md).
//----------------------------------------------------------------------------
static const char kDefaultPlayerName[] = "";

const char* __fastcall GetPlayerDisplayName(unsigned int player, int ensure)
{
    IndexedArrayHolder_6F3A1650* world =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
    const char* fallback = kDefaultPlayerName;

    if (world != 0)
    {
        int slot = world->At(player);

        if (slot != 0)
            return ((SPlayerSlot*)slot)->EnsureDisplayName(ensure);
    }

    return fallback;
}

//----------------------------------------------------------------------------
//  0x6F2F8FC0 - is the local player a referee?
//
//  The acting slot index is a *word* at +0x28 of the world object, which is
//  what the `movzx` says and what a plain dword field would not produce.
//----------------------------------------------------------------------------
int __fastcall IsLocalPlayerReferee()
{
    IndexedArrayHolder_6F3A1650* world =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;

    if (world != 0)
    {
        int slot = world->At(world->m_actingSlot);

        if (slot != 0)
            return GetField0x2A8_6F40B1C0((const void*)slot);
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F2F9020 - is the local player a full player, i.e. one whose slot is
//  really taken and who is *not* a referee?
//----------------------------------------------------------------------------
int __fastcall IsLocalPlayerFullPlayer()
{
    IndexedArrayHolder_6F3A1650* world =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;

    if (world != 0)
    {
        int slot = world->At(world->m_actingSlot);

        if (slot != 0
            && ((HandleRefFieldOwner_6F473170*)((char*)slot + 0xF0))
                   ->QueryField78() != 0
            && GetField0x2A8_6F40B1C0((const void*)slot) == 0)
            return 1;
    }

    return 0;
}
