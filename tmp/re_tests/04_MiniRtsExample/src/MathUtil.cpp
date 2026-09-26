#include "Base.h"

static const int kSinTableSize = 1024;
static float s_sinTable[kSinTableSize + 1];

void InitMathTables()
{
    for (int i = 0; i <= kSinTableSize; ++i)
        s_sinTable[i] = sinf((float)i * kTwoPi / (float)kSinTableSize);
}

// Table lookup with linear interpolation between entries.
float FastSin(float angle)
{
    float t = angle * ((float)kSinTableSize / kTwoPi);
    int i = FloorToInt(t);
    float frac = t - (float)i;
    i &= kSinTableSize - 1;
    return s_sinTable[i] + (s_sinTable[i + 1] - s_sinTable[i]) * frac;
}

float FastCos(float angle)
{
    return FastSin(angle + kHalfPi);
}

float Length(const Vec2& v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

float Distance(const Vec2& a, const Vec2& b)
{
    return sqrtf(DistSq(a, b));
}

Vec2 Normalize(const Vec2& v)
{
    float len = Length(v);
    if (len < 0.0001f)
        return Vec2(1.0f, 0.0f);
    float inv = 1.0f / len;
    return Vec2(v.x * inv, v.y * inv);
}

Vec2 PolarOffset(const Vec2& p, float dist, float angle)
{
    return Vec2(p.x + FastCos(angle) * dist, p.y + FastSin(angle) * dist);
}

// Polynomial atan2 approximation (max error ~0.005 rad).
float Atan2Approx(float y, float x)
{
    float ax = AbsF(x);
    float ay = AbsF(y);
    if (ax < 1e-6f && ay < 1e-6f)
        return 0.0f;

    float a = (ax > ay) ? ay / ax : ax / ay;
    float s = a * a;
    float r = ((-0.0464964749f * s + 0.15931422f) * s - 0.327622764f) * s * a + a;

    if (ay > ax)
        r = kHalfPi - r;
    if (x < 0.0f)
        r = kPi - r;
    if (y < 0.0f)
        r = -r;
    return r;
}

float NormalizeAngle(float a)
{
    while (a < 0.0f)
        a += kTwoPi;
    while (a >= kTwoPi)
        a -= kTwoPi;
    return a;
}

float AngleTo(const Vec2& from, const Vec2& to)
{
    return NormalizeAngle(Atan2Approx(to.y - from.y, to.x - from.x));
}

// Signed shortest rotation from 'from' to 'to', in (-pi, pi].
float AngleDiff(float from, float to)
{
    float d = NormalizeAngle(to) - NormalizeAngle(from);
    if (d > kPi)
        d -= kTwoPi;
    else if (d <= -kPi)
        d += kTwoPi;
    return d;
}

float TurnTowards(float current, float target, float maxStep)
{
    float d = AngleDiff(current, target);
    if (AbsF(d) <= maxStep)
        return NormalizeAngle(target);
    return NormalizeAngle(current + (d > 0.0f ? maxStep : -maxStep));
}

Vec2 ClosestPointOnSegment(const Vec2& a, const Vec2& b, const Vec2& p)
{
    Vec2 ab = b - a;
    float lenSq = ab.LengthSq();
    if (lenSq < 0.0001f)
        return a;
    float t = ClampF((p - a).Dot(ab) / lenSq, 0.0f, 1.0f);
    return a + ab * t;
}

bool SegmentHitsCircle(const Vec2& a, const Vec2& b, const Vec2& c, float r)
{
    Vec2 closest = ClosestPointOnSegment(a, b, c);
    return DistSq(closest, c) <= r * r;
}

int FloorToInt(float v)
{
    int i = (int)v;
    if ((float)i > v)
        --i;
    return i;
}

float SmoothStep(float t)
{
    return t * t * (3.0f - 2.0f * t);
}

float Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}
