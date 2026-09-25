//============================================================================
//  Base-shape CNetCommandXxx builders - the dispatcher's action-byte cases
//  whose command carries no payload beyond the standard header (see
//  netcommand_pause.cpp, the hand-verified exemplar this file's shape is
//  taken from, for the field-order/SEH-frame rationale). Each is its own
//  class so its constructor stamps its own distinct vftable; none of them
//  call each other so sharing this one TU carries no inlining risk.
//============================================================================
#include "netcommand.h"
#include "cdatastore.h"

//----------------------------------------------------------------------------
//  0x6F540380 - build a CNetCommandUnpause (wire type 0x0A0002) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandUnpause : public CNetCommandBase
{
public:
    CNetCommandUnpause(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0002, 2)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandUnpause_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandUnpause_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandUnpause cmd(ctx, sender);
    CNetCommandUnpause_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F540520 - build a CNetCommandGameSpeedIncrement (wire type 0x0A0004) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandGameSpeedIncrement : public CNetCommandBase
{
public:
    CNetCommandGameSpeedIncrement(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0004, 4)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandGameSpeedIncrement_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandGameSpeedIncrement_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandGameSpeedIncrement cmd(ctx, sender);
    CNetCommandGameSpeedIncrement_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F5405F0 - build a CNetCommandGameSpeedDecrement (wire type 0x0A0005) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandGameSpeedDecrement : public CNetCommandBase
{
public:
    CNetCommandGameSpeedDecrement(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0005, 5)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandGameSpeedDecrement_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandGameSpeedDecrement_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandGameSpeedDecrement cmd(ctx, sender);
    CNetCommandGameSpeedDecrement_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F540F00 - build a CNetCommandUnitRefreshSubGroup (wire type 0x0A001A) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandUnitRefreshSubGroup : public CNetCommandBase
{
public:
    CNetCommandUnitRefreshSubGroup(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A001A, 26)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandUnitRefreshSubGroup_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandUnitRefreshSubGroup_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandUnitRefreshSubGroup cmd(ctx, sender);
    CNetCommandUnitRefreshSubGroup_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F541320 - build a CNetCommandCheatDefeat (wire type 0x0A0022) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandCheatDefeat : public CNetCommandBase
{
public:
    CNetCommandCheatDefeat(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0022, 34)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandCheatDefeat_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandCheatDefeat_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatDefeat cmd(ctx, sender);
    CNetCommandCheatDefeat_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F5413F0 - build a CNetCommandCheatCooldown (wire type 0x0A0020) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandCheatCooldown : public CNetCommandBase
{
public:
    CNetCommandCheatCooldown(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0020, 32)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandCheatCooldown_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandCheatCooldown_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatCooldown cmd(ctx, sender);
    CNetCommandCheatCooldown_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F5415A0 - build a CNetCommandCheatFastBuild (wire type 0x0A0023) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandCheatFastBuild : public CNetCommandBase
{
public:
    CNetCommandCheatFastBuild(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0023, 35)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandCheatFastBuild_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandCheatFastBuild_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatFastBuild cmd(ctx, sender);
    CNetCommandCheatFastBuild_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F541670 - build a CNetCommandCheatFastDeath (wire type 0x0A0024) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandCheatFastDeath : public CNetCommandBase
{
public:
    CNetCommandCheatFastDeath(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0024, 36)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandCheatFastDeath_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandCheatFastDeath_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatFastDeath cmd(ctx, sender);
    CNetCommandCheatFastDeath_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F541740 - build a CNetCommandCheatFood (wire type 0x0A0025) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandCheatFood : public CNetCommandBase
{
public:
    CNetCommandCheatFood(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0025, 37)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandCheatFood_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandCheatFood_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatFood cmd(ctx, sender);
    CNetCommandCheatFood_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F541810 - build a CNetCommandCheatGod (wire type 0x0A0026) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandCheatGod : public CNetCommandBase
{
public:
    CNetCommandCheatGod(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0026, 38)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandCheatGod_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandCheatGod_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatGod cmd(ctx, sender);
    CNetCommandCheatGod_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F541A80 - build a CNetCommandCheatMana (wire type 0x0A0029) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandCheatMana : public CNetCommandBase
{
public:
    CNetCommandCheatMana(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0029, 41)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandCheatMana_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandCheatMana_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatMana cmd(ctx, sender);
    CNetCommandCheatMana_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F541B50 - build a CNetCommandCheatNoDefeat (wire type 0x0A002A) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandCheatNoDefeat : public CNetCommandBase
{
public:
    CNetCommandCheatNoDefeat(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A002A, 42)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandCheatNoDefeat_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandCheatNoDefeat_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatNoDefeat cmd(ctx, sender);
    CNetCommandCheatNoDefeat_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F541C20 - build a CNetCommandCheatNoVictory (wire type 0x0A002B) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandCheatNoVictory : public CNetCommandBase
{
public:
    CNetCommandCheatNoVictory(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A002B, 43)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandCheatNoVictory_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandCheatNoVictory_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatNoVictory cmd(ctx, sender);
    CNetCommandCheatNoVictory_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F541CF0 - build a CNetCommandCheatResearch (wire type 0x0A002C) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandCheatResearch : public CNetCommandBase
{
public:
    CNetCommandCheatResearch(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A002C, 44)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandCheatResearch_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandCheatResearch_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatResearch cmd(ctx, sender);
    CNetCommandCheatResearch_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F541F70 - build a CNetCommandCheatShowMap (wire type 0x0A002F) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandCheatShowMap : public CNetCommandBase
{
public:
    CNetCommandCheatShowMap(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A002F, 47)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandCheatShowMap_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandCheatShowMap_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatShowMap cmd(ctx, sender);
    CNetCommandCheatShowMap_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F542040 - build a CNetCommandCheatTechTree (wire type 0x0A0030) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandCheatTechTree : public CNetCommandBase
{
public:
    CNetCommandCheatTechTree(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0030, 48)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandCheatTechTree_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandCheatTechTree_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatTechTree cmd(ctx, sender);
    CNetCommandCheatTechTree_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F542110 - build a CNetCommandCheatUpgrade (wire type 0x0A0031) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandCheatUpgrade : public CNetCommandBase
{
public:
    CNetCommandCheatUpgrade(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0031, 49)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandCheatUpgrade_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandCheatUpgrade_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatUpgrade cmd(ctx, sender);
    CNetCommandCheatUpgrade_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F5421E0 - build a CNetCommandCheatVictory (wire type 0x0A0032) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandCheatVictory : public CNetCommandBase
{
public:
    CNetCommandCheatVictory(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0032, 50)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandCheatVictory_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandCheatVictory_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandCheatVictory cmd(ctx, sender);
    CNetCommandCheatVictory_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F542550 - build a CNetCommandPlayerEndCinematic (wire type 0x0A0061) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandPlayerEndCinematic : public CNetCommandBase
{
public:
    CNetCommandPlayerEndCinematic(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0061, 97)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandPlayerEndCinematic_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandPlayerEndCinematic_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandPlayerEndCinematic cmd(ctx, sender);
    CNetCommandPlayerEndCinematic_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F542F60 - build a CNetCommandShowSkill (wire type 0x0A0066) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandShowSkill : public CNetCommandBase
{
public:
    CNetCommandShowSkill(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0066, 102)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandShowSkill_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandShowSkill_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandShowSkill cmd(ctx, sender);
    CNetCommandShowSkill_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F543030 - build a CNetCommandShowSubmenu (wire type 0x0A0067) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandShowSubmenu : public CNetCommandBase
{
public:
    CNetCommandShowSubmenu(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0067, 103)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandShowSubmenu_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandShowSubmenu_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandShowSubmenu cmd(ctx, sender);
    CNetCommandShowSubmenu_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F5432B0 - build a CNetCommandReplayViewSpeedIncrement (wire type 0x0A0084) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandReplayViewSpeedIncrement : public CNetCommandBase
{
public:
    CNetCommandReplayViewSpeedIncrement(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0084, 132)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandReplayViewSpeedIncrement_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandReplayViewSpeedIncrement_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandReplayViewSpeedIncrement cmd(ctx, sender);
    CNetCommandReplayViewSpeedIncrement_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  0x6F543380 - build a CNetCommandReplayViewSpeedDecrement (wire type 0x0A0085) and fire it, unless
//  `suppressFire` is set (return immediately) or the reader's position has
//  already run past its declared length (ParseError instead).
//----------------------------------------------------------------------------

class CNetCommandReplayViewSpeedDecrement : public CNetCommandBase
{
public:
    CNetCommandReplayViewSpeedDecrement(int ctx, unsigned char sender)
        : CNetCommandBase(0x0A0085, 133)
    {
        m_ctx = ctx;
        m_sender = sender;
    }
};


void* __fastcall CNetCommandReplayViewSpeedDecrement_Attach(CDataStore* reader, CNetCommandBase* cmd);

void __fastcall CNetCommandReplayViewSpeedDecrement_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandReplayViewSpeedDecrement cmd(ctx, sender);
    CNetCommandReplayViewSpeedDecrement_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetData_FireCommand(player, &cmd, commandName, replayFlag, turn);
}
