//============================================================================
//  0x6F53E890 - ApplyGameSpeedScale: turn a game speed (0, 1 or 2) into the
//  tick scale the simulation runs at, and publish both into the game-mode
//  block.  `__fastcall`, the speed in ecx, a bare `retn` - no stack
//  arguments.
//
//  The block is reached exactly the way IsGameModeOne (gamemode.cpp) reaches
//  it: slot 0x0D of this thread's pointer array, that context's cache at
//  +0x10, and the block itself at +0x08 of the cache.  It can be absent -
//  the shipped code tests it and does nothing if it is null, which is why
//  the clamp below sits *inside* the test rather than in front of it.
//
//  The clamp is the same pair of chained selects SGameConfig::SetGameSpeed
//  applies one call earlier (gameconfigspeed.cpp), repeated here on the
//  32-bit value; the shipped code does it twice too, and the second one is
//  what makes the table index provably in range.
//
//  dword_6FA89154 is that table: {3, 4, 5}, read as *unsigned* - which is
//  what the `test eax,eax` / `jge` / `fadd 4294967296.0` around the `fild`
//  says, MSVC's standard unsigned-to-floating conversion.  Divided by 5.0
//  it gives 0.6 / 0.8 / 1.0, so +0x22AC is a scale factor and not a period,
//  and +0x22A8 keeps the raw speed beside it.  The divisor is a `double`
//  (`fdiv` against a qword), so the arithmetic is done in double and only
//  the result is narrowed by the `fstp dword ptr` into the float field.
//
//  Two shapes in here are load-bearing and both were measured.  The clamp's
//  first select writes back into the *parameter* (`speed = ...`), which is
//  what keeps the value in the register it arrived in - a fresh local costs
//  a second register and pushes the block out of ecx into edx.  And the
//  table entry is named in a local before it is converted: read inside the
//  conversion expression, MSVC `fild`s straight out of the table
//  (`fild [eax*4+SYM]`) where the shipped code loads it into eax, spills it
//  to the stack and `fild`s from there.
//
//  Own translation unit: its one caller reaches it with a real `call`, and
//  the shipped build never inlined it.
//============================================================================
#include "gamecontext.h"

//  dword_6FA89154 - three ticks-per-something values, one per game speed.
//  Declared, never defined: it is a real global in the shipped image and
//  funcmap.py's DATA table is what binds this reference to its address.
extern unsigned int g_gameSpeedTicks[3];

//----------------------------------------------------------------------------
//  The block hanging off the game data cache at +0x08.  gamemode.cpp reads
//  a mode word at +0x610 out of the same object under its own name; this is
//  a second, deliberately separate view of it, because neither function's
//  call tree establishes anything between the fields the other one touches.
//----------------------------------------------------------------------------
struct SGameTickBlock
{
    char         m_reserved0000[0x22A8];
    unsigned int m_gameSpeed;    // +0x22A8
    float        m_tickScale;    // +0x22AC
};

struct SGameTickCacheHead
{
    char             m_reserved00[8];
    SGameTickBlock*  m_pTickBlock;   // +0x08
};

void __fastcall ApplyGameSpeedScale(unsigned int speed)
{
    CGameDataContext* context =
        (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    SGameTickCacheHead* cache = (SGameTickCacheHead*)context->m_pCache;

    SGameTickBlock* block = cache->m_pTickBlock;

    if (block != 0)
    {
        speed = (speed < 2) ? speed : 2;

        unsigned int index = (0 < speed) ? speed : 0;

        block->m_gameSpeed = index;

        unsigned int ticks = g_gameSpeedTicks[index];
        block->m_tickScale = (float)(ticks / 5.0);
    }
}
