//============================================================================
//  The four CNetCommandXxx classes whose one (or, for TrainCancel, two)
//  extra payload field(s) is/are populated by their own Attach hook
//  (netcommand_attach_all.cpp) reading directly into `this + offset`,
//  rather than the caller pre-computing a field pointer to hand in - see
//  netcommand_action_builders_1field.cpp for the BuildAndFire side.
//============================================================================
#ifndef NETCOMMAND_CLASSES_1FIELD_H
#define NETCOMMAND_CLASSES_1FIELD_H

#include "netcommand.h"

class CNetCommandGameSpeed : public CNetCommandBase
{
public:
    CNetCommandGameSpeed(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0003, 3)
    {
        m_ctx = ctx;
        m_sender = sender;
    }

    unsigned char m_speedLevel;  // +0x18
};

class CNetCommandSaveDone : public CNetCommandBase
{
public:
    CNetCommandSaveDone(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0007, 7)
    {
        m_ctx = ctx;
        m_sender = sender;
    }

    unsigned int m_saveSlot;  // +0x18
};

class CNetCommandTrainCancel : public CNetCommandBase
{
public:
    CNetCommandTrainCancel(int ctx, unsigned char sender)
        : CNetCommandBase(0xA001E, 0x1E)
    {
        m_ctx = ctx;
        m_sender = sender;
    }

    unsigned char m_field18;  // +0x18
    unsigned int  m_field1C;  // +0x1C
};

class CNetCommandArrowKey : public CNetCommandBase
{
public:
    CNetCommandArrowKey(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0075, 0x75)
    {
        m_ctx = ctx;
        m_sender = sender;
    }

    unsigned char m_key;  // +0x18
};

#endif
