// Core types, math primitives, allocator interface and a growable array
// shared by every module of the mini RTS.
//
// Everything here must compile with VC80 (C++03): no <stdint.h>, no C++11.

#ifndef MR_BASE_H
#define MR_BASE_H

#include <stddef.h>
#include <string.h>
#include <math.h>

typedef unsigned char  u8;
typedef signed char    s8;
typedef unsigned short u16;
typedef signed short   s16;
typedef unsigned int   u32;
typedef signed int     s32;

typedef u32 Handle;

#define COUNTOF(a) (sizeof(a) / sizeof((a)[0]))

const float kPi        = 3.14159265f;
const float kTwoPi     = 6.28318531f;
const float kHalfPi    = 1.57079633f;
const float kDegToRad  = 0.01745329f;

// Simulation runs at a fixed step.
const float kTickSeconds    = 0.05f;
const int   kTicksPerSecond = 20;

// 4 playable slots + neutral hostile (creeps) + neutral passive (mines).
const int kMaxPlayers           = 6;
const int kMaxRealPlayers       = 4;
const int kPlayerNeutralHostile = 4;
const int kPlayerNeutralPassive = 5;

const int kMaxAbilitySlots = 5;
const int kInventorySlots  = 6;
const int kOrderQueueSize  = 8;
const int kTrainQueueSize  = 5;
const int kMaxHeroLevel    = 10;

inline float MinF(float a, float b) { return a < b ? a : b; }
inline float MaxF(float a, float b) { return a > b ? a : b; }
inline float AbsF(float a) { return a < 0.0f ? -a : a; }
inline float ClampF(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
inline int   MinI(int a, int b) { return a < b ? a : b; }
inline int   MaxI(int a, int b) { return a > b ? a : b; }
inline int   ClampI(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }
inline int   AbsI(int a) { return a < 0 ? -a : a; }

struct Vec2
{
    float x;
    float y;

    Vec2() {}
    Vec2(float ax, float ay) : x(ax), y(ay) {}

    Vec2 operator+(const Vec2& o) const { return Vec2(x + o.x, y + o.y); }
    Vec2 operator-(const Vec2& o) const { return Vec2(x - o.x, y - o.y); }
    Vec2 operator*(float s) const { return Vec2(x * s, y * s); }
    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
    float Dot(const Vec2& o) const { return x * o.x + y * o.y; }
    float LengthSq() const { return x * x + y * y; }
};

inline float DistSq(const Vec2& a, const Vec2& b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

// MathUtil.cpp
void  InitMathTables();
float FastSin(float angle);
float FastCos(float angle);
float Length(const Vec2& v);
float Distance(const Vec2& a, const Vec2& b);
Vec2  Normalize(const Vec2& v);
Vec2  PolarOffset(const Vec2& p, float dist, float angle);
float AngleTo(const Vec2& from, const Vec2& to);
float NormalizeAngle(float a);
float AngleDiff(float from, float to);
float TurnTowards(float current, float target, float maxStep);
float Atan2Approx(float y, float x);
Vec2  ClosestPointOnSegment(const Vec2& a, const Vec2& b, const Vec2& p);
bool  SegmentHitsCircle(const Vec2& a, const Vec2& b, const Vec2& c, float r);
int   FloorToInt(float v);
float SmoothStep(float t);
float Lerp(float a, float b, float t);

// Memory.cpp - size-class pool allocator used by all game objects.
void  MemInit();
void  MemShutdown();
void* MemAlloc(u32 size);
void  MemFree(void* p);
void* MemRealloc(void* p, u32 size);

// Growable array for POD-like element types (grows with MemRealloc, so
// elements must be trivially copyable).
template <class T>
class TArray
{
public:
    TArray() : m_data(0), m_count(0), m_capacity(0) {}
    ~TArray() { if (m_data) MemFree(m_data); }

    int Count() const { return m_count; }
    T& operator[](int i) { return m_data[i]; }
    const T& operator[](int i) const { return m_data[i]; }
    T* Data() { return m_data; }
    const T* Data() const { return m_data; }

    void Add(const T& value)
    {
        if (m_count == m_capacity)
            Grow(m_count + 1);
        m_data[m_count++] = value;
    }

    void RemoveSwap(int index)
    {
        --m_count;
        if (index != m_count)
            m_data[index] = m_data[m_count];
    }

    void RemoveOrdered(int index);
    int  Find(const T& value) const;
    void Reserve(int capacity);

    bool RemoveValue(const T& value)
    {
        int i = Find(value);
        if (i < 0)
            return false;
        RemoveSwap(i);
        return true;
    }

    void Clear() { m_count = 0; }
    void SetCount(int count) { if (count > m_capacity) Grow(count); m_count = count; }

private:
    void Grow(int needed);

    TArray(const TArray&);
    TArray& operator=(const TArray&);

    T*  m_data;
    int m_count;
    int m_capacity;
};

template <class T>
void TArray<T>::Grow(int needed)
{
    int capacity = m_capacity ? m_capacity * 2 : 16;
    while (capacity < needed)
        capacity *= 2;
    m_data = static_cast<T*>(MemRealloc(m_data, (u32)(capacity * sizeof(T))));
    m_capacity = capacity;
}

template <class T>
void TArray<T>::Reserve(int capacity)
{
    if (capacity > m_capacity)
        Grow(capacity);
}

template <class T>
void TArray<T>::RemoveOrdered(int index)
{
    for (int i = index + 1; i < m_count; ++i)
        m_data[i - 1] = m_data[i];
    --m_count;
}

template <class T>
int TArray<T>::Find(const T& value) const
{
    for (int i = 0; i < m_count; ++i)
    {
        if (m_data[i] == value)
            return i;
    }
    return -1;
}

// Every heap-allocated game object comes from the pool allocator.
class CObject
{
public:
    CObject() : m_handle(0) {}
    virtual ~CObject() {}

    static void* operator new(size_t size) { return MemAlloc((u32)size); }
    static void operator delete(void* p) { MemFree(p); }

    Handle m_handle;
};

#endif
