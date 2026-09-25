//============================================================================
//  Shared "return two floats / one int by value" wrapper types for the
//  CFootprintTypeData pair properties (footprintshadowoffset.cpp).  Same
//  hidden-return-pointer convention Agent/relationconstant.h's
//  SRelationColorConstant already established for a single int: a
//  user-declared copy constructor disqualifies the class from being a
//  trivial POD, which is what forces MSVC to pass the return buffer as a
//  hidden `ecx` argument instead of returning in eax/edx.
//============================================================================
#ifndef FOOTPRINTPAIR_H
#define FOOTPRINTPAIR_H

class SFootprintFloatPair
{
public:
    SFootprintFloatPair(float ix, float iy) : x(ix), y(iy) {}
    SFootprintFloatPair(const SFootprintFloatPair& other)
        : x(other.x), y(other.y) {}

    float x;
    float y;
};

class SFootprintModelColor
{
public:
    explicit SFootprintModelColor(int value) : m_value(value) {}
    SFootprintModelColor(const SFootprintModelColor& other)
        : m_value(other.m_value) {}

    int m_value;
};

#endif
