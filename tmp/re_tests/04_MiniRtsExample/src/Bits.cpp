#include "Engine.h"

static u32  s_crcTable[256];
static bool s_crcReady;

void InitCrcTable()
{
    for (u32 i = 0; i < 256; ++i)
    {
        u32 c = i;
        for (int k = 0; k < 8; ++k)
            c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        s_crcTable[i] = c;
    }
    s_crcReady = true;
}

u32 Crc32(const void* data, u32 length, u32 crc)
{
    if (!s_crcReady)
        InitCrcTable();
    const u8* p = static_cast<const u8*>(data);
    crc = ~crc;
    for (u32 i = 0; i < length; ++i)
        crc = s_crcTable[(crc ^ p[i]) & 0xFF] ^ (crc >> 8);
    return ~crc;
}

u32 HashBytes(const void* data, u32 length, u32 seed)
{
    const u8* p = static_cast<const u8*>(data);
    u32 h = 0x811C9DC5u ^ seed;
    for (u32 i = 0; i < length; ++i)
    {
        h ^= p[i];
        h *= 0x01000193u;
    }
    return h;
}

u32 HashCombine(u32 hash, u32 value)
{
    value *= 0xCC9E2D51u;
    value = (value << 15) | (value >> 17);
    value *= 0x1B873593u;
    hash ^= value;
    hash = (hash << 13) | (hash >> 19);
    return hash * 5 + 0xE6546B64u;
}

// ---------------------------------------------------------------------------

void CBitWriter::Init(u8* buffer, u32 capacity)
{
    m_buffer = buffer;
    m_capacity = capacity;
    m_bitPos = 0;
    m_overflow = false;
    memset(buffer, 0, capacity);
}

// LSB-first bit packing.
void CBitWriter::WriteBits(u32 value, int count)
{
    if (m_overflow)
        return;
    if (m_bitPos + (u32)count > m_capacity * 8)
    {
        m_overflow = true;
        return;
    }

    for (int i = 0; i < count; ++i)
    {
        if (value & (1u << i))
            m_buffer[m_bitPos >> 3] |= (u8)(1u << (m_bitPos & 7));
        ++m_bitPos;
    }
}

// 7 bits per group, high bit = continuation.
void CBitWriter::WriteVar(u32 value)
{
    do
    {
        u32 group = value & 0x7F;
        value >>= 7;
        WriteBits(group | (value ? 0x80u : 0u), 8);
    }
    while (value && !m_overflow);
}

void CBitWriter::WriteSigned(s32 value)
{
    u32 zigzag = ((u32)value << 1) ^ (u32)(value >> 31);
    WriteVar(zigzag);
}

void CBitWriter::WriteQuant(float value, float lo, float hi, int bits)
{
    u32 maxValue = (1u << bits) - 1;
    float t = ClampF((value - lo) / (hi - lo), 0.0f, 1.0f);
    WriteBits((u32)(t * (float)maxValue + 0.5f), bits);
}

u32 CBitWriter::Finish()
{
    return (m_bitPos + 7) >> 3;
}

// ---------------------------------------------------------------------------

void CBitReader::Init(const u8* data, u32 length)
{
    m_data = data;
    m_length = length;
    m_bitPos = 0;
    m_failed = false;
}

u32 CBitReader::ReadBits(int count)
{
    if (m_bitPos + (u32)count > m_length * 8)
    {
        m_failed = true;
        m_bitPos = m_length * 8;
        return 0;
    }

    u32 value = 0;
    for (int i = 0; i < count; ++i)
    {
        if (m_data[m_bitPos >> 3] & (1u << (m_bitPos & 7)))
            value |= 1u << i;
        ++m_bitPos;
    }
    return value;
}

u32 CBitReader::ReadVar()
{
    u32 value = 0;
    int shift = 0;
    for (;;)
    {
        u32 group = ReadBits(8);
        value |= (group & 0x7F) << shift;
        if (!(group & 0x80) || m_failed)
            break;
        shift += 7;
        if (shift > 28)
        {
            m_failed = true;
            break;
        }
    }
    return value;
}

s32 CBitReader::ReadSigned()
{
    u32 zigzag = ReadVar();
    return (s32)(zigzag >> 1) ^ -(s32)(zigzag & 1);
}

float CBitReader::ReadQuant(float lo, float hi, int bits)
{
    u32 maxValue = (1u << bits) - 1;
    u32 raw = ReadBits(bits);
    return lo + (hi - lo) * ((float)raw / (float)maxValue);
}
