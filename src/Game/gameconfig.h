//============================================================================
//  SGameConfig - the block of "how is this game set up" state that hangs off
//  the world object (`dword_6FAB65F4`) at +0x30.
//
//  Every one of the JASS game-type / game-speed / difficulty / placement
//  natives (0x6F3BAE10 - 0x6F3BAF20, src/Jass/jassnatives_gameconfig.cpp) is
//  two loads and one access: fetch the world object, follow its +0x30
//  pointer, and read or write one field.  The offsets below are exactly the
//  ones those nine functions touch and nothing else is named - the block is
//  much larger than 0x49 bytes and this call tree says nothing about the
//  rest of it.
//
//  Which field is which is fixed by the natives' own registered signatures
//  (docs/targets/jass-natives-registration-table.md), not guessed: the byte
//  `GetGameSpeed ()Hgamespeed;` reads is +0x47, the one
//  `GetGamePlacement ()Hplacement;` reads is +0x48 and the one
//  `GetGameDifficulty ()Hgamedifficulty;` reads is +0x46.
//============================================================================
#ifndef GAMECONFIG_H
#define GAMECONFIG_H

struct SGameConfig
{
    //  0x6F3DB270 - clamp a game speed into 0..2, latch it at +0x47 and
    //  push the matching tick scale into the game-mode block.  `retn 4`,
    //  `this` in ecx.  Own translation unit, Game/gameconfigspeed.cpp.
    void SetGameSpeed(int speed);

    char          m_reserved00[0x30];
    //  +0x30 - the bitmask of game types the map declares itself playable
    //  as.  `SetGameTypeSupported` ors a bit in or masks it out;
    //  `IsGameTypeSupported` tests it.
    unsigned int  m_gameTypesSupported;   // +0x30
    //  +0x34 - the game type actually selected.  `GetGameTypeSelected`
    //  hands it back with bit 3 cleared.
    unsigned int  m_gameTypeSelected;     // +0x34
    //  +0x38 - configuration flags.  Bit 0x4000 is the only one this family
    //  reads: set, it locks the game speed and `SetGameSpeed` does nothing.
    unsigned int  m_flags;                // +0x38
    char          m_reserved3C[0x0A];
    unsigned char m_difficulty;           // +0x46
    unsigned char m_speed;                // +0x47
    unsigned char m_placement;            // +0x48
};

//  dword_6FAB65F4 again - the same world object every other view of it in
//  this repo casts (Widget/playercolor.h, Jass/jassnatives.h,
//  Item/itemhandlemain.h).  One `extern`, one cast per view, so funcmap.py's
//  DATA table keeps exactly one C++ name for the address.
extern void* g_unk6FAB65F4;

struct SWorldGameConfigOwner
{
    char         m_reserved00[0x30];
    SGameConfig* m_pGameConfig;   // +0x30
};

#define g_pGameConfig (((SWorldGameConfigOwner*)g_unk6FAB65F4)->m_pGameConfig)

//  0x6F53E890 - push a game speed through to the simulation: latch the
//  clamped speed in the game-mode block and store the tick scale that goes
//  with it.  `__fastcall` with the speed in ecx and a bare `retn`, so no
//  stack arguments at all.  Own translation unit, Game/gamespeedscale.cpp.
void __fastcall ApplyGameSpeedScale(unsigned int speed);

#endif
