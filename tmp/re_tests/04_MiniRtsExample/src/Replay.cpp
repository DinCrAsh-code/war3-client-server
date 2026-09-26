#include "Game.h"

// Replay stream: a sequence of records (turn packets and periodic sync
// checksums) buffered into 8K blocks. Each block is LZ-compressed (or
// stored if that does not help) behind a small header:
//   u16 rawLength, u16 storedLength, u8 method, u8 pad, u32 crc32(raw)

enum ReplayRecord
{
    RR_TURN = 1,
    RR_SYNC = 2
};

enum BlockMethod
{
    BM_STORED = 0,
    BM_LZ     = 1
};

static const u32 kBlockHeaderSize = 10;

static u8 s_blockScratch[8192 + 8192 / 8 + 64];

static void PutU16(u8* p, u32 v)
{
    p[0] = (u8)v;
    p[1] = (u8)(v >> 8);
}

static void PutU32(u8* p, u32 v)
{
    PutU16(p, v & 0xFFFF);
    PutU16(p + 2, v >> 16);
}

static u32 GetU16(const u8* p)
{
    return (u32)p[0] | ((u32)p[1] << 8);
}

static u32 GetU32(const u8* p)
{
    return GetU16(p) | (GetU16(p + 2) << 16);
}

static void AppendBytes(TArray<u8>& out, const u8* data, u32 length)
{
    int base = out.Count();
    out.SetCount(base + (int)length);
    memcpy(out.Data() + base, data, length);
}

void CReplay::Init()
{
    m_rawLength = 0;
    m_blocks.Clear();
    m_shadow.Clear();
    m_crc = 0;
    m_numBlocks = 0;
    m_numTurns = 0;
}

void CReplay::Append(const void* data, u32 length)
{
    if (m_rawLength + length > sizeof(m_raw))
        FlushBlock();
    if (length > sizeof(m_raw))
        return;
    memcpy(m_raw + m_rawLength, data, length);
    m_rawLength += length;
    AppendBytes(m_shadow, static_cast<const u8*>(data), length);
}

void CReplay::RecordTurn(int turn, const u8* data, u32 length)
{
    u8 header[5];
    header[0] = RR_TURN;
    PutU16(header + 1, (u32)turn);
    PutU16(header + 3, length);
    Append(header, sizeof(header));
    Append(data, length);
    ++m_numTurns;
}

void CReplay::RecordSync(int turn, u32 checksum)
{
    u8 record[7];
    record[0] = RR_SYNC;
    PutU16(record + 1, (u32)turn);
    PutU32(record + 3, checksum);
    Append(record, sizeof(record));
}

void CReplay::FlushBlock()
{
    if (m_rawLength == 0)
        return;

    u32 packed = LzCompress(m_raw, m_rawLength, s_blockScratch, sizeof(s_blockScratch));
    u8 method = BM_LZ;
    const u8* payload = s_blockScratch;
    if (packed == 0 || packed >= m_rawLength)
    {
        method = BM_STORED;
        payload = m_raw;
        packed = m_rawLength;
    }

    u32 crc = Crc32(m_raw, m_rawLength, 0);
    u8 header[kBlockHeaderSize];
    PutU16(header, m_rawLength);
    PutU16(header + 2, packed);
    header[4] = method;
    header[5] = 0;
    PutU32(header + 6, crc);

    AppendBytes(m_blocks, header, kBlockHeaderSize);
    AppendBytes(m_blocks, payload, packed);
    m_crc = Crc32(m_raw, m_rawLength, m_crc);
    ++m_numBlocks;
    m_rawLength = 0;
}

u32 CReplay::Finalize()
{
    FlushBlock();
    return Crc32(m_blocks.Data(), (u32)m_blocks.Count(), m_crc ^ (u32)m_numTurns);
}

bool CReplay::Verify() const
{
    static u8 unpacked[8192];
    const u8* p = m_blocks.Data();
    u32 total = (u32)m_blocks.Count();
    u32 pos = 0;
    u32 shadowPos = 0;
    const u8* shadow = m_shadow.Data();

    while (pos + kBlockHeaderSize <= total)
    {
        u32 rawLength = GetU16(p + pos);
        u32 stored = GetU16(p + pos + 2);
        u8 method = p[pos + 4];
        u32 crc = GetU32(p + pos + 6);
        pos += kBlockHeaderSize;
        if (pos + stored > total || rawLength > sizeof(unpacked))
            return false;

        u32 got;
        if (method == BM_LZ)
            got = LzDecompress(p + pos, stored, unpacked, sizeof(unpacked));
        else
        {
            memcpy(unpacked, p + pos, stored);
            got = stored;
        }
        if (got != rawLength || Crc32(unpacked, got, 0) != crc)
            return false;
        if (shadowPos + got > (u32)m_shadow.Count() || memcmp(unpacked, shadow + shadowPos, got) != 0)
            return false;

        shadowPos += got;
        pos += stored;
    }
    return pos == total && shadowPos == (u32)m_shadow.Count();
}
