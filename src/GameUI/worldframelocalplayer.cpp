//============================================================================
//  0x6F391960 - CWorldFrameWar3::LatchLocalPlayer.
//
//  Everything the world frame keeps about *whose* screen it is, taken in one
//  pass off the player-slot table and finished by recolouring every indicator
//  the frame owns.  The table itself is the lazily-created singleton at
//  g_unk6FAB65F4 (the same global JASS_PauseGame reads its acting slot out
//  of); a null one is created here, through the same registry acquire the
//  rest of the 0x6F007xxx layer uses, and written back.
//
//  Two readings this file fixes, both forced by the disassembly:
//
//    * +0x1F4 is a *word* (`mov [esi+1F4h], dx`, read back with `movzx`),
//      and +0x1F8 and +0x214 have their addresses taken as two separate
//      objects.  All three used to sit inside a 0x78-byte `m_texturePath`
//      block at +0x1C8, which cannot be true; gameui.h now splits it.
//    * the local player's record is fetched *twice*, and the second fetch
//      reloads the table out of +0x178 rather than keeping it in a register
//      (`mov ecx, [esi+178h]` at 0x6F3919AE).  Written as one local the
//      score drops by three, so both reads go through the member.
//
//  Own translation unit: five real calls out of it, and it is the only body
//  in its shipped module reached here.
//
//  Two instructions are not source-addressable and are the whole of the
//  shortfall: the shipped code forms the acquire's `6` as `lea ecx, [eax+6]`
//  off the null it has just tested, and its `0` second argument likewise.
//============================================================================
#include "gameui.h"

//  dword_6FAB65F4 - the player-slot table singleton.  Declared, never
//  defined: it is a funcmap.DATA global and the .mix binds it to the real
//  one (tools/link_check.py, cause 5).
extern void* g_unk6FAB65F4;

//----------------------------------------------------------------------------
//  The player-slot table as this file reads it.  Declared locally rather than
//  pulled out of Widget/widgetregistersubmit.h, the way every other caller of
//  IndexedArrayHolder_6F3A1650::At in this repo declares its own view: the
//  `At` body at 0x6F3A1650 is Misc/misc_field_getters.cpp's and is EXACT, and
//  a second naked redirect for it here would be two symbols for one address.
//----------------------------------------------------------------------------
struct IndexedArrayHolder_6F3A1650
{
    char           m_reserved00[0x28];
    //  +0x28 - the acting (local) player's slot, a word.
    unsigned short m_actingSlot;        // +0x28
    unsigned short m_reserved2A;
    //  +0x2C - which player slots are live, one bit each; the frame's first
    //  mask table is built from it.
    unsigned short m_livePlayers;       // +0x2C
    unsigned short m_reserved2E;
    char           m_reserved30[0x04];
    void*          m_field34;           // +0x34
    char           m_reserved38[0x388];
    void*          m_field3C0;          // +0x3C0

    int At(unsigned int index);
};

//  0x6F473170 - Misc/queryfield.cpp; the record's +0xF0 handle ref resolved
//  to the object behind it.
struct HandleRefFieldOwner_6F473170 { int __thiscall QueryField78(); };

//  0x6F0074F0 - create the registry singleton with this index and park it in
//  the registry's own slot.  `retn 0`, so both arguments are register ones;
//  the second is pushed as zero here and never read by the callee.  Still a
//  redirect (GameUI/gameui_thunks.cpp).
void* __fastcall AcquireGameSingleton(int index, int unused);

void CWorldFrameWar3::LatchLocalPlayer()
{
    void* table = g_unk6FAB65F4;
    if (table == 0)
    {
        table = AcquireGameSingleton(6, 0);
        g_unk6FAB65F4 = table;
    }

    m_pPlayerTable = table;
    m_field188 = ((IndexedArrayHolder_6F3A1650*)table)->m_field34;

    unsigned int slot = ((IndexedArrayHolder_6F3A1650*)table)->m_actingSlot;
    m_localPlayer = slot;
    m_localPlayerBit = (unsigned short)(1 << slot);

    //  The slot is re-read out of the table for the lookup rather than
    //  reused from `slot`: the shipped code issues a second
    //  `movzx ecx, word ptr [eax+28h]` there, and reusing the local drops it.
    m_localPlayerRecord = ((IndexedArrayHolder_6F3A1650*)table)->At(
        ((IndexedArrayHolder_6F3A1650*)table)->m_actingSlot);

    //  The second lookup, through the member and not through `table`: see
    //  the header note.
    HandleRefFieldOwner_6F473170* ref =
        (HandleRefFieldOwner_6F473170*)
            (((IndexedArrayHolder_6F3A1650*)m_pPlayerTable)->At(
                 ((IndexedArrayHolder_6F3A1650*)m_pPlayerTable)->m_actingSlot)
             + 0xF0);
    m_localPlayerObject = ref->QueryField78();

    BuildPlayerMaskTable(&m_playerMaskBySlot,
                         ((IndexedArrayHolder_6F3A1650*)m_pPlayerTable)
                             ->m_livePlayers,
                         0);
    BuildPlayerMaskTable(&m_playerMaskByRecord, m_localPlayerBit, 1);

    m_field32C = ((IndexedArrayHolder_6F3A1650*)m_pPlayerTable)->m_field3C0;

    RecolorIndicatorsForLocalPlayer();
}
