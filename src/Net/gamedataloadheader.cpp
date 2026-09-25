//============================================================================
//  0x6F534750 - read and check one saved game's header.
//
//  Eight refusals, each with its own code, and the codes are what the caller
//  turns into a boolean:
//
//      8   the source would not open
//      9   the file is shorter than the header it claims
//      0Ah a read failed
//      0Bh m_headerSize is not the size this version's header is
//      0Ch the magic is not "Warcraft III recorded game\x1A"
//      0Dh the header's own CRC does not check
//      0Eh m_dataSize runs past the end of the file - the *only* one that
//          does not stop the header being filled in
//      0Fh m_version is neither 0 nor 1
//
//  Version 0's header is 0x40 bytes and has no +0x30, so it is read into a
//  local of its own shape and scattered afterwards with a zero in the hole;
//  version 1's 0x1C-byte tail is read straight into the caller's header.
//  Both CRCs are taken over the whole header with the CRC field zeroed,
//  which is why the field is saved and written back at the bottom.
//
//  m_headerSize and m_version are saved and written back on the same tail.
//  Neither read can have changed them - version 0 reads into a local and
//  version 1's tail starts at +0x28 - but the shipped code stores them
//  anyway, on the one path that reaches both.
//============================================================================
#include "netgameload.h"

int __stdcall Storm_508(const char* a, const char* b, unsigned int max);
unsigned int __fastcall GameDataCrc32(const void* data, unsigned int length);

//  NetGameLoadSource's three pure virtuals, reached as hand-written slot
//  dispatches - see netgameload.h for why there is no declaration to call.
typedef int              (__thiscall* SourceOpenFn)(void* self);
typedef int              (__thiscall* SourceReadFn)(void* self, void* dest, unsigned int size);
typedef unsigned __int64 (__thiscall* SourceSizeFn)(void* self);

int __fastcall GameDataReadHeader(NetGameLoadSource* source, SGameLoadHeader* header)
{
    //  One `result` and a `goto`, not eight `return`s: the shipped code
    //  writes the code into a frame slot and reads it back out at every one
    //  of its exits, which is what a single-exit local looks like once MSVC
    //  has duplicated the tail behind each jump.
    int result = 0;

    if (!((SourceOpenFn)(*(void***)source)[0x00 / 4])(source))
    {
        result = 8;
        goto done;
    }

    {
    unsigned __int64 fileSize = ((SourceSizeFn)(*(void***)source)[0x08 / 4])(source);
    if (fileSize < 0x28)
    {
        result = 9;
        goto done;
    }

    if (!((SourceReadFn)(*(void***)source)[0x04 / 4])(source, header, 0x28))
    {
        result = 0xA;
        goto done;
    }

    if (Storm_508(header->m_prefix.m_magic, "Warcraft III recorded game\x1A", 0x7FFFFFFF) != 0)
    {
        result = 0xC;
        goto done;
    }

    {
    unsigned int headerSize = header->m_prefix.m_headerSize;
    unsigned int version = header->m_prefix.m_version;
    unsigned int crc;

    if (version == 0)
    {
        if (fileSize < 0x40)
        {
            result = 9;
            goto done;
        }

        if (headerSize != 0x40)
        {
            result = 0xB;
            goto done;
        }

        {
        SGameLoadHeaderV0 old;
        old.m_prefix = header->m_prefix;

        if (!((SourceReadFn)(*(void***)source)[0x04 / 4])(source, &old.m_word28, 0x18))
        {
            result = 0xA;
            goto done;
        }

        crc = old.m_crc;
        old.m_crc = 0;

        if (crc != GameDataCrc32(&old, 0x40))
        {
            result = 0xD;
            goto done;
        }

        header->m_word28 = old.m_word28;
        header->m_word2C = old.m_word2C;
        header->m_word30 = 0;
        header->m_word34 = old.m_word34;
        header->m_word38 = old.m_word38;
        header->m_word3C = old.m_word3C;
        }
    }
    else
    {
        if (version != 1)
        {
            result = 0xF;
            goto done;
        }

        if (fileSize < 0x44)
        {
            result = 9;
            goto done;
        }

        if (headerSize != 0x44)
        {
            result = 0xB;
            goto done;
        }

        if (!((SourceReadFn)(*(void***)source)[0x04 / 4])(source, &header->m_word28, 0x1C))
        {
            result = 0xA;
            goto done;
        }

        crc = header->m_crc;
        header->m_crc = 0;

        if (crc != GameDataCrc32(header, 0x44))
        {
            result = 0xD;
            goto done;
        }
    }

    header->m_crc = crc;
    header->m_prefix.m_headerSize = headerSize;
    header->m_prefix.m_version = version;

    if (header->m_prefix.m_dataSize > (unsigned int)fileSize)
        result = 0xE;
    }
    }

done:
    return result;
}
