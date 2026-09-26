// Engine services with no knowledge of game objects: RNG, handle table,
// bit streams / checksums, compression, timers and the event bus.

#ifndef MR_ENGINE_H
#define MR_ENGINE_H

#include "Base.h"

class CUnit;

// ---------------------------------------------------------------------------
// Random.cpp

class CRandom
{
public:
    void  Seed(u32 seed);
    u32   Next();
    int   Range(int lo, int hi);
    float RangeF(float lo, float hi);
    bool  Chance(float probability);
    u32   State() const { return m_a ^ m_b; }

private:
    u32 m_a;
    u32 m_b;
};

// ---------------------------------------------------------------------------
// Memory.cpp

class CHandleTable
{
public:
    void     Init(int capacity);
    void     Shutdown();
    Handle   Register(CObject* obj);
    void     Unregister(Handle h);
    CObject* Lookup(Handle h) const;
    int      LiveCount() const { return m_liveCount; }

private:
    struct Slot
    {
        CObject* object;
        u16      generation;
        u16      nextFree;
    };

    Slot* m_slots;
    int   m_capacity;
    int   m_freeHead;
    int   m_liveCount;
};

// ---------------------------------------------------------------------------
// Bits.cpp

void InitCrcTable();
u32  Crc32(const void* data, u32 length, u32 crc);
u32  HashBytes(const void* data, u32 length, u32 seed);
u32  HashCombine(u32 hash, u32 value);

class CBitWriter
{
public:
    void Init(u8* buffer, u32 capacity);
    void WriteBits(u32 value, int count);
    void WriteVar(u32 value);
    void WriteSigned(s32 value);
    void WriteQuant(float value, float lo, float hi, int bits);
    u32  Finish();
    bool Overflowed() const { return m_overflow; }
    u32  BitPosition() const { return m_bitPos; }

private:
    u8*  m_buffer;
    u32  m_capacity;
    u32  m_bitPos;
    bool m_overflow;
};

class CBitReader
{
public:
    void  Init(const u8* data, u32 length);
    u32   ReadBits(int count);
    u32   ReadVar();
    s32   ReadSigned();
    float ReadQuant(float lo, float hi, int bits);
    bool  AtEnd() const { return m_bitPos >= m_length * 8; }
    bool  Failed() const { return m_failed; }

private:
    const u8* m_data;
    u32       m_length;
    u32       m_bitPos;
    bool      m_failed;
};

// ---------------------------------------------------------------------------
// Compress.cpp

u32 LzCompress(const u8* src, u32 srcLength, u8* dst, u32 dstCapacity);
u32 LzDecompress(const u8* src, u32 srcLength, u8* dst, u32 dstCapacity);

// ---------------------------------------------------------------------------
// Timer.cpp

typedef void (*TimerFn)(void* context, u32 param);

class CTimerQueue
{
public:
    CTimerQueue() : m_nextId(1), m_now(0.0f) {}
    void  Init();
    u32   Schedule(float delay, float period, TimerFn fn, void* context, u32 param);
    void  Advance(float now);
    int   Count() const { return m_heap.Count(); }
    float Now() const { return m_now; }

private:
    struct Timer
    {
        float   due;
        float   period;
        TimerFn fn;
        void*   context;
        u32     param;
        u32     id;
    };

    void  Push(const Timer& t);
    Timer Pop();
    void  SiftUp(int index);
    void  SiftDown(int index);

    TArray<Timer> m_heap;
    u32           m_nextId;
    float         m_now;
};

// ---------------------------------------------------------------------------
// Events.cpp

enum EventType
{
    EVT_UNIT_DEATH = 0,
    EVT_UNIT_DAMAGED,
    EVT_UNIT_TRAINED,
    EVT_BUILD_FINISHED,
    EVT_HERO_LEVEL,
    EVT_ITEM_PICKUP,
    EVT_RESEARCH_DONE,
    EVT_COUNT
};

struct EventData
{
    int    type;
    CUnit* unit;
    CUnit* other;
    float  value;
    int    param;
};

typedef void (*EventFn)(const EventData& e, void* context);

class CEventBus
{
public:
    void Register(int type, EventFn fn, void* context);
    void Fire(int type, CUnit* unit, CUnit* other, float value, int param);

private:
    struct Listener
    {
        EventFn fn;
        void*   context;
        bool operator==(const Listener& o) const { return fn == o.fn && context == o.context; }
    };

    TArray<Listener> m_listeners[EVT_COUNT];
};

#endif
