//============================================================================
//  0x6F314BB0 - see relationconstant.cpp.
//============================================================================
#ifndef RELATIONCONSTANT_H
#define RELATIONCONSTANT_H

//  A free function's hidden-pointer return needs a class that is not
//  trivially copyable - same reason CFloat.h's own copy constructor exists
//  - even though this one is a single `int`.
class SRelationColorConstant
{
public:
    explicit SRelationColorConstant(int value) : m_value(value) {}
    SRelationColorConstant(const SRelationColorConstant& other)
        : m_value(other.m_value) {}

    int m_value;
};

SRelationColorConstant __fastcall GetPlayerRelationConstant(int slot1, int slot2);

#endif
