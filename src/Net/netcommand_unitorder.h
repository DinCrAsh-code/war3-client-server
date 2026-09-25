//============================================================================
//  The unit-order CNetCommandXxx family - a real inheritance chain, not six
//  independent classes.  Each Attach hook (netcommand_unitorder_attach_*.cpp)
//  calls its immediate base's own Attach hook first, then reads its own
//  extra payload fields - matching the chained `call` sequence the dump
//  shows (TargetPoint_Attach calls UnitOrderBasic_Attach, TargetImage_Attach
//  calls TargetPoint_Attach, and so on).  See netcommand.h for the shared
//  base-header rationale and netcommand_pause.cpp for the exemplar this
//  module's own shape is taken from.
//
//  Every extra payload field the constructor gives a literal default is
//  read back by the class's own Attach hook before the object is fired -
//  the same "zero/sentinel now, real value from the wire hook" split
//  netcommand_classes_1field.h documents, just repeated per inheritance
//  level here.  CFloat fields default to `g_CFloatZero`'s encoded bits
//  (dword_6FAAE470 in the dump) rather than a `0` immediate - see
//  msvc-vc8-idioms.md's own note on why a defining declaration is wrong for
//  it and CFloat.h's own comment on the global.
//============================================================================
#ifndef NETCOMMAND_UNITORDER_H
#define NETCOMMAND_UNITORDER_H

#include "netcommand.h"
#include "CFloat.h"

//  0x6F540790 - dispatcher case for wire type 0xA0010 (sub-index 0x10).
class CNetCommandUnitOrderBasic : public CNetCommandBase
{
public:
    CNetCommandUnitOrderBasic(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0010, 0x10)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_field18 = 0;
        m_field1C = -1;
        m_field20 = -1;
        m_field24 = -1;
    }

    unsigned short m_field18;   // +0x18
    int m_field1C;  // +0x1C
    int m_field20;  // +0x20
    int m_field24;  // +0x24
};

//  0x6F540870 - wire type 0xA0011 (sub-index 0x11).  Adds a target point.
class CNetCommandUnitOrderTargetPoint : public CNetCommandUnitOrderBasic
{
public:
    CNetCommandUnitOrderTargetPoint(int ctx, unsigned char sender)
        : CNetCommandUnitOrderBasic(ctx, sender)
    {
        m_id = 0xA0011;
        m_subIndex = 0x11;
        m_x = g_CFloatZero;
        m_y = g_CFloatZero;
    }

    CFloat m_x;  // +0x28
    CFloat m_y;  // +0x2C
};

//  0x6F540960 - wire type 0xA0012 (sub-index 0x12).  Adds one unit handle.
class CNetCommandUnitOrderTargetImage : public CNetCommandUnitOrderTargetPoint
{
public:
    CNetCommandUnitOrderTargetImage(int ctx, unsigned char sender)
        : CNetCommandUnitOrderTargetPoint(ctx, sender)
    {
        m_id = 0xA0012;
        m_subIndex = 0x12;
        m_handle0 = 0;
        m_handle1 = 0;
    }

    unsigned int m_handle0;  // +0x30
    unsigned int m_handle1;  // +0x34
};

//  0x6F540A50 - wire type 0xA0013 (sub-index 0x13).  Adds a second handle pair.
class CNetCommandUnitOrderTargetImage2 : public CNetCommandUnitOrderTargetImage
{
public:
    CNetCommandUnitOrderTargetImage2(int ctx, unsigned char sender)
        : CNetCommandUnitOrderTargetImage(ctx, sender)
    {
        m_id = 0xA0013;
        m_subIndex = 0x13;
        m_handle2 = 0;
        m_handle3 = 0;
    }

    unsigned int m_handle2;  // +0x38
    unsigned int m_handle3;  // +0x3C
};

//  0x6F540B50 - wire type 0xA0014 (sub-index 0x14).  Derives from
//  TargetPoint directly (not TargetImage) - the fogged variant carries its
//  own three dwords/one byte/two floats instead of the two handles.
class CNetCommandUnitOrderTargetImageFogged : public CNetCommandUnitOrderTargetPoint
{
public:
    CNetCommandUnitOrderTargetImageFogged(int ctx, unsigned char sender)
        : CNetCommandUnitOrderTargetPoint(ctx, sender)
    {
        m_id = 0xA0014;
        m_subIndex = 0x14;
        m_field30 = 0;
        m_field34 = 0;
        m_field38 = 0;
        m_field3C = 0;
        m_field40 = g_CFloatZero;
        m_field44 = g_CFloatZero;
    }

    unsigned int m_field30;              // +0x30
    unsigned int m_field34;              // +0x34
    unsigned int m_field38;              // +0x38
    unsigned char m_field3C;              // +0x3C
    CFloat m_field40;   // +0x40
    CFloat m_field44;   // +0x44
};

//  0x6F540C50 - wire type 0xA0015 (sub-index 0x15).  Adds two more ints
//  beyond TargetImageFogged.
class CNetCommandUnitOrderTargetImageFogged2 : public CNetCommandUnitOrderTargetImageFogged
{
public:
    CNetCommandUnitOrderTargetImageFogged2(int ctx, unsigned char sender)
        : CNetCommandUnitOrderTargetImageFogged(ctx, sender)
    {
        m_id = 0xA0015;
        m_subIndex = 0x15;
        m_field48 = -1;
        m_field4C = -1;
    }

    int m_field48;  // +0x48
    int m_field4C;  // +0x4C
};

#endif
