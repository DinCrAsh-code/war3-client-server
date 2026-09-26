#include "Engine.h"

// LZSS-style compressor: 4K window, 3..18 byte matches, one flag byte per
// eight items (bit set = literal). A match is stored as 12-bit distance and
// 4-bit length in two bytes.

namespace
{
    const u32 kWindowSize = 4096;
    const u32 kMinMatch   = 3;
    const u32 kMaxMatch   = 18;
    const u32 kHashSize   = 4096;
    const u32 kMaxChain   = 32;

    struct MatchFinder
    {
        s32 head[kHashSize];
        s32 prev[kWindowSize];
    };

    MatchFinder s_finder;

    u32 HashTriplet(const u8* p)
    {
        u32 v = ((u32)p[0] << 16) | ((u32)p[1] << 8) | p[2];
        return ((v * 2654435761u) >> 20) & (kHashSize - 1);
    }

    void InsertPosition(const u8* src, u32 pos, u32 srcLength)
    {
        if (pos + kMinMatch > srcLength)
            return;
        u32 h = HashTriplet(src + pos);
        s_finder.prev[pos & (kWindowSize - 1)] = s_finder.head[h];
        s_finder.head[h] = (s32)pos;
    }

    u32 FindLongestMatch(const u8* src, u32 pos, u32 srcLength, u32* outDistance)
    {
        if (pos + kMinMatch > srcLength)
            return 0;

        u32 best = 0;
        u32 maxLen = srcLength - pos;
        if (maxLen > kMaxMatch)
            maxLen = kMaxMatch;

        s32 candidate = s_finder.head[HashTriplet(src + pos)];
        u32 chain = 0;
        while (candidate >= 0 && chain < kMaxChain)
        {
            u32 distance = pos - (u32)candidate;
            if (distance == 0 || distance >= kWindowSize)
                break;

            u32 len = 0;
            while (len < maxLen && src[candidate + len] == src[pos + len])
                ++len;
            if (len > best)
            {
                best = len;
                *outDistance = distance;
                if (len == maxLen)
                    break;
            }
            candidate = s_finder.prev[candidate & (kWindowSize - 1)];
            ++chain;
        }
        return best >= kMinMatch ? best : 0;
    }
}

u32 LzCompress(const u8* src, u32 srcLength, u8* dst, u32 dstCapacity)
{
    for (u32 i = 0; i < kHashSize; ++i)
        s_finder.head[i] = -1;
    for (u32 i = 0; i < kWindowSize; ++i)
        s_finder.prev[i] = -1;

    u32 out = 0;
    u32 pos = 0;
    while (pos < srcLength)
    {
        if (out + 1 + 8 * 2 > dstCapacity)
            return 0;

        u32 flagPos = out++;
        u8 flags = 0;
        for (int item = 0; item < 8 && pos < srcLength; ++item)
        {
            u32 distance = 0;
            u32 len = FindLongestMatch(src, pos, srcLength, &distance);
            if (len)
            {
                u32 token = ((distance & 0xFFF) << 4) | (len - kMinMatch);
                dst[out++] = (u8)(token >> 8);
                dst[out++] = (u8)token;
                for (u32 k = 0; k < len; ++k)
                    InsertPosition(src, pos + k, srcLength);
                pos += len;
            }
            else
            {
                flags |= (u8)(1 << item);
                dst[out++] = src[pos];
                InsertPosition(src, pos, srcLength);
                ++pos;
            }
        }
        dst[flagPos] = flags;
    }
    return out;
}

u32 LzDecompress(const u8* src, u32 srcLength, u8* dst, u32 dstCapacity)
{
    u32 in = 0;
    u32 out = 0;
    while (in < srcLength)
    {
        u8 flags = src[in++];
        for (int item = 0; item < 8 && in < srcLength; ++item)
        {
            if (flags & (1 << item))
            {
                if (out >= dstCapacity)
                    return 0;
                dst[out++] = src[in++];
            }
            else
            {
                if (in + 2 > srcLength)
                    return 0;
                u32 token = ((u32)src[in] << 8) | src[in + 1];
                in += 2;
                u32 distance = token >> 4;
                u32 len = (token & 0xF) + kMinMatch;
                if (distance == 0 || distance > out || out + len > dstCapacity)
                    return 0;
                for (u32 k = 0; k < len; ++k, ++out)
                    dst[out] = dst[out - distance];
            }
        }
    }
    return out;
}

