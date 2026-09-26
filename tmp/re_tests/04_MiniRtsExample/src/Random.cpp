#include "Engine.h"

// Combined generator: a 32-bit LCG stepped alongside a xorshift register.
// Both halves are part of the synchronized game state.

void CRandom::Seed(u32 seed)
{
    m_a = seed ^ 0x5DEECE66u;
    m_b = (seed << 13) ^ (seed >> 7) ^ 0x2545F491u;
    if (m_b == 0)
        m_b = 0x9E3779B9u;
    for (int i = 0; i < 8; ++i)
        Next();
}

u32 CRandom::Next()
{
    m_a = m_a * 1664525u + 1013904223u;
    m_b ^= m_b << 13;
    m_b ^= m_b >> 17;
    m_b ^= m_b << 5;
    return (m_a >> 16) ^ m_b;
}

// Inclusive range.
int CRandom::Range(int lo, int hi)
{
    if (hi <= lo)
        return lo;
    u32 span = (u32)(hi - lo) + 1;
    return lo + (int)(Next() % span);
}

float CRandom::RangeF(float lo, float hi)
{
    float t = (float)(Next() & 0xFFFFFF) / 16777215.0f;
    return lo + (hi - lo) * t;
}

bool CRandom::Chance(float probability)
{
    if (probability <= 0.0f)
        return false;
    if (probability >= 1.0f)
        return true;
    return RangeF(0.0f, 1.0f) < probability;
}

