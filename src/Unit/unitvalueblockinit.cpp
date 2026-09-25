//============================================================================
//  0x6F077E80 - SUnitValueBlock::Init.
//
//  Forty-two stores and nothing else.  The order is the shipped one and it
//  is not the declaration order: the three mode words first, then the nine
//  CFloats at +0x2C..+0x4C, then the three int totals, then the remaining
//  fields of the first three groups interleaved - which is MSVC scheduling
//  independent stores through one base register, not a source pattern.
//  Reproduced in that order so the diff lines up.
//
//  Own translation unit: 0x6F077FA0 calls it twice per iteration for real.
//============================================================================
#include "unitvalueblock.h"

SUnitValueBlock* SUnitValueBlock::Init()
{
    m_mode00 = 2;
    m_mode0C = 2;
    m_mode18 = 2;

    m_f24 = g_CFloatZero;
    m_f28 = g_CFloatZero;
    m_f2C = g_CFloatZero;
    m_f30 = g_CFloatZero;
    m_f34 = g_CFloatZero;
    m_f38 = g_CFloatZero;
    m_f3C = g_CFloatZero;
    m_f40 = g_CFloatZero;
    m_f44 = g_CFloatZero;
    m_f48 = g_CFloatZero;
    m_f4C = g_CFloatZero;

    m_total50 = 0;
    m_total54 = 0;
    m_total58 = 0;

    m_count04 = 0;
    m_f10 = g_CFloatZero;
    m_f1C = g_CFloatZero;

    m_count08 = 0;
    m_f14 = g_CFloatZero;
    m_f20 = g_CFloatZero;

    return this;
}
