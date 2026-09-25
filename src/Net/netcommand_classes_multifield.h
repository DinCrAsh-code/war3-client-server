//============================================================================
//  Independent (non-chained) CNetCommandXxx classes carrying 1-4 extra
//  payload fields, each populated by its own Attach hook
//  (netcommand_attach_multifield.cpp) reading directly into `this + offset`
//  - the same split netcommand_classes_1field.h documents for the single-
//  field builders.  None of these derive from one another, unlike the
//  unit-order family (netcommand_unitorder.h), so they can all share one
//  header and one Attach TU without any inlining risk between them.
//============================================================================
#ifndef NETCOMMAND_CLASSES_MULTIFIELD_H
#define NETCOMMAND_CLASSES_MULTIFIELD_H

#include "netcommand.h"
#include "CFloat.h"

//  0x6FAAE4F4 - defined in src/Pathfinding/pathmove.h; re-declared here
//  rather than pulling in the whole pathfinding header for one constant.
extern const CFloat g_slopeThreshold1;

//  0x6F541170 - dispatcher case for wire type 0xA001D.
class CNetCommandReviveCancel : public CNetCommandBase
{
public:
    CNetCommandReviveCancel(int ctx, unsigned char sender)
        : CNetCommandBase(0xA001D, 0x1D)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_field18 = -1;
        m_field1C = -1;
    }

    int m_field18;  // +0x18
    int m_field1C;  // +0x1C
};

//  0x6F540FD0 - wire type 0xA001B.
class CNetCommandUnitSelectionEvent : public CNetCommandBase
{
public:
    CNetCommandUnitSelectionEvent(int ctx, unsigned char sender)
        : CNetCommandBase(0xA001B, 0x1B)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_field18 = 0;
        m_field1C = -1;
        m_field20 = -1;
    }

    unsigned char m_field18;   // +0x18
    int m_field1C;  // +0x1C
    int m_field20;  // +0x20
};

//  0x6F5410A0 - wire type 0xA001C.  Same field shape as UnitSelectionEvent
//  above (confirmed by both their Attach hooks reading in the same order).
class CNetCommandSelectableSelectionModify : public CNetCommandBase
{
public:
    CNetCommandSelectableSelectionModify(int ctx, unsigned char sender)
        : CNetCommandBase(0xA001C, 0x1C)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_field18 = 0;
        m_field1C = -1;
        m_field20 = -1;
    }

    unsigned char m_field18;   // +0x18
    int m_field1C;  // +0x1C
    int m_field20;  // +0x20
};

//  0x6F540D60 - wire type 0xA0018.  Two adjacent bytes, read in reverse
//  (m_field19 before m_field18) - the dump's own order, kept as-is.
class CNetCommandUnitSelectControlGroup : public CNetCommandBase
{
public:
    CNetCommandUnitSelectControlGroup(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0018, 0x18)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_field18 = 0;
        m_field19 = 0;
    }

    unsigned char m_field18;  // +0x18
    unsigned char m_field19;  // +0x19
};

//  0x6F540E30 - wire type 0xA0019.
class CNetCommandUnitSelectSubGroup : public CNetCommandBase
{
public:
    CNetCommandUnitSelectSubGroup(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0019, 0x19)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_field18 = 0;
        m_field1C = -1;
        m_field20 = -1;
    }

    int m_field18;   // +0x18
    int m_field1C;  // +0x1C
    int m_field20;  // +0x20
};

//  0x6F5414C0 - wire type 0xA0021 (one of the cheat commands carrying a
//  resource/critter/TOD payload the CLAUDE.md worklist named).
class CNetCommandCheatCritter : public CNetCommandBase
{
public:
    CNetCommandCheatCritter(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0021, 0x21)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_field18 = -1;
        m_field1C = -1;
    }

    int m_field18;  // +0x18
    int m_field1C;  // +0x1C
};

//  0x6F5418E0 - wire type 0xA0027.  0xFF is the "no target player" sentinel
//  the base header's own m_sender uses too.
class CNetCommandCheatGold : public CNetCommandBase
{
public:
    CNetCommandCheatGold(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0027, 0x27)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_targetPlayer = 0xFF;
        m_amount = 0;
    }

    unsigned char m_targetPlayer;  // +0x18
    int m_amount;           // +0x1C
};

//  0x6F5419B0 - wire type 0xA0028.  Same shape as CheatGold.
class CNetCommandCheatLumber : public CNetCommandBase
{
public:
    CNetCommandCheatLumber(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0028, 0x28)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_targetPlayer = 0xFF;
        m_amount = 0;
    }

    unsigned char m_targetPlayer;  // +0x18
    int m_amount;           // +0x1C
};

//  0x6F541DC0 - wire type 0xA002D.  Same shape as CheatGold/CheatLumber.
class CNetCommandCheatResources : public CNetCommandBase
{
public:
    CNetCommandCheatResources(int ctx, unsigned char sender)
        : CNetCommandBase(0xA002D, 0x2D)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_targetPlayer = 0xFF;
        m_amount = 0;
    }

    unsigned char m_targetPlayer;  // +0x18
    int m_amount;           // +0x1C
};

//  0x6F541E90 - wire type 0xA002E ("set time of day").  Its one payload
//  field defaults to g_CFloatZero's encoded bits, like every other CFloat
//  member in this module - see CFloat.h and msvc-vc8-idioms.md.
class CNetCommandCheatSetTOD : public CNetCommandBase
{
public:
    CNetCommandCheatSetTOD(int ctx, unsigned char sender)
        : CNetCommandBase(0xA002E, 0x2E)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_timeOfDay = g_CFloatZero;
    }

    CFloat m_timeOfDay;  // +0x18
};

//  0x6F5422B0 - wire type 0xA0050.
class CNetCommandTeamChangeAlliance : public CNetCommandBase
{
public:
    CNetCommandTeamChangeAlliance(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0050, 0x50)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_targetPlayer = 0xFF;
        m_field1C = 0;
    }

    unsigned char m_targetPlayer;  // +0x18
    int m_field1C;          // +0x1C
};

//  0x6F542380 - wire type 0xA0051.
class CNetCommandTeamTradeResources : public CNetCommandBase
{
public:
    CNetCommandTeamTradeResources(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0051, 0x51)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_targetPlayer = 0xFF;
        m_field1C = 0;
        m_field20 = 0;
    }

    unsigned char m_targetPlayer;  // +0x18
    int m_field1C;          // +0x1C
    int m_field20;          // +0x20
};

//  0x6F542450 - wire type 0xA0060 ("player chat match" - a keyword/phrase
//  string the trigger/synchronizer side matches chat text against).  The
//  0x100-byte buffer at +0x18 is CDataStore::ReadString's destination
//  (netcommand_attach_multifield.cpp's own CNetCommandPlayerChatMatch_
//  Attach, 0x6F555010); its first byte is pre-zeroed the same way every
//  ReadString-backed buffer in this codebase is, and gets forced back to 0
//  by the Attach hook itself if the reader ran past its declared length
//  mid-string.  The two trailing dwords default to -1 like every other
//  unread-sentinel field in this module.
class CNetCommandPlayerChatMatch : public CNetCommandBase
{
public:
    CNetCommandPlayerChatMatch(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0060, 0x60)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_matchText[0] = 0;
        m_field118 = -1;
        m_field11C = -1;
    }

    char m_matchText[0x100];  // +0x18
    int  m_field118;          // +0x118
    int  m_field11C;          // +0x11C
};

//  0x6F542620 - wire type 0xA0062.  m_field20 carries no default member
//  initialiser - the dump's own constructor never stores anything there,
//  only the Attach hook (netcommand_attach_multifield.cpp) writes it.
class CNetCommandResumeTriggerExec : public CNetCommandBase
{
public:
    CNetCommandResumeTriggerExec(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0062, 0x62)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_field18 = -1;
        m_field1C = -1;
    }

    int m_field18;  // +0x18
    int m_field1C;  // +0x1C
    int m_field20;       // +0x20 - deliberately uninitialised, see above
};

//  0x6F542700 - wire type 0xA0063.
class CNetCommandTriggerSyncReady : public CNetCommandBase
{
public:
    CNetCommandTriggerSyncReady(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0063, 0x63)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_field18 = -1;
        m_field1C = -1;
    }

    int m_field18;  // +0x18
    int m_field1C;  // +0x1C
};

//  0x6F5427E0 - wire type 0xA0064.  Same shape as TriggerSyncReady.
class CNetCommandTrackableHit : public CNetCommandBase
{
public:
    CNetCommandTrackableHit(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0064, 0x64)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_field18 = -1;
        m_field1C = -1;
    }

    int m_field18;  // +0x18
    int m_field1C;  // +0x1C
};

//  0x6F5428C0 - wire type 0xA0065.  Same shape as TrackableHit.
class CNetCommandTrackableTrack : public CNetCommandBase
{
public:
    CNetCommandTrackableTrack(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0065, 0x65)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_field18 = -1;
        m_field1C = -1;
    }

    int m_field18;  // +0x18
    int m_field1C;  // +0x1C
};

//  0x6F5429A0 - wire type 0xA0069.
class CNetCommandDialogButtonClick : public CNetCommandBase
{
public:
    CNetCommandDialogButtonClick(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0069, 0x69)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_field18 = -1;
        m_field1C = -1;
        m_field20 = -1;
        m_field24 = -1;
    }

    int m_field18;  // +0x18
    int m_field1C;  // +0x1C
    int m_field20;  // +0x20
    int m_field24;  // +0x24
};

//  0x6F542A80 - wire type 0xA006A.  Same shape as DialogButtonClick.
class CNetCommandDialogClick : public CNetCommandBase
{
public:
    CNetCommandDialogClick(int ctx, unsigned char sender)
        : CNetCommandBase(0xA006A, 0x6A)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_field18 = -1;
        m_field1C = -1;
        m_field20 = -1;
        m_field24 = -1;
    }

    int m_field18;  // +0x18
    int m_field1C;  // +0x1C
    int m_field20;  // +0x20
    int m_field24;  // +0x24
};

//  0x6F543100 - wire type 0xA0068.  m_field20 reuses g_slopeThreshold1's
//  encoded 1.0 bit pattern (dword_6FAAE4F4 in the dump) rather than
//  g_CFloatZero - a cross-module constant the shipped code happened to
//  already have lying around with the value this field wants, exactly the
//  kind of reuse msvc-vc8-idioms.md documents for dword_X globals like it.
class CNetCommandAllyPing : public CNetCommandBase
{
public:
    CNetCommandAllyPing(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0068, 0x68)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_x = g_CFloatZero;
        m_y = g_CFloatZero;
        m_field20 = g_slopeThreshold1;
    }

    CFloat m_x;         // +0x18
    CFloat m_y;         // +0x1C
    CFloat m_field20;  // +0x20
};

#endif
