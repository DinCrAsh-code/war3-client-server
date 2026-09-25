//============================================================================
//  0x6F653B40 - a standard byte-at-a-time CRC32 (the classic reflected
//  polynomial table form: `crc = (crc >> 8) ^ table[(crc ^ *data) & 0xFF]`,
//  table pre-inverted so the loop starts from an all-ones `eax` and the
//  final result is `~eax`). Reached from GameAction_0x26_PacketHandler
//  (gameaction0x26_handler.cpp) to validate an incoming action block's
//  declared checksum against its own payload.
//
//  `dword_6F970A30`'s 256 real entries are not recoverable from a static
//  disassembly dump (data, not code) - declared, not defined. Nothing about
//  the instruction stream this produces depends on its contents.
//============================================================================

void __stdcall SErrSetLastError(unsigned int code);
extern const unsigned int g_gameActionCrc32Table[256];

unsigned int __fastcall Crc32(const void* data, unsigned int length)
{
    if (!data && length != 0)
    {
        SErrSetLastError(0x57);
        return 0;
    }

    unsigned int crc = 0xFFFFFFFF;
    const unsigned char* p = (const unsigned char*)data;
    while (length--)
    {
        crc = g_gameActionCrc32Table[(crc ^ *p) & 0xFF] ^ (crc >> 8);
        ++p;
    }
    return ~crc;
}
