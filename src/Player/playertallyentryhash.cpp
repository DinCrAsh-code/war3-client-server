//============================================================================
//  0x6F3DC3E0 - a leaf checksum fold over one tally-list entry's own
//  offsets +0x18/+0x1C/+0x20/+0x24/+0x28/+0x2C/+0x30 - the run of fields
//  playerscore.h's own SPlayerTally leaves reserved because no
//  score-screen walk in that module ever reads them.  Only
//  CPlayerTallyList::ComputeChecksum (playerroster.cpp, 0x6F3F6760) calls
//  this, so the fields it touches are genuinely checksum-only.
//
//  Written against a raw `void*` rather than SPlayerTally* on purpose:
//  SPlayerTally's own header only names the four fields its already-scored
//  siblings (TotalTalliedCost and friends) read, and reusing that struct's
//  named accessors here would either force this file to add four
//  synonyms for fields another module already owns, or read them as the
//  wrong type (this leaf reads +0x30 as a full dword; playerscore.h's own
//  m_flags30 is a byte, because every other reader of that offset masks it
//  with 8).  A raw offset cast keeps this file from having any opinion
//  about what the rest of SPlayerTally is - the same reasoning
//  Sync/ctimesynchashinto.cpp's own note gives for not pulling in game.h's
//  full CTimeSync definition.
//
//  Own translation unit: a different address neighbourhood
//  (0x6F3DCxxx) from its one caller (0x6F3F67xx, playerroster.cpp) and
//  from CGameWar3's own module (0x6F3Axxx/0x6F40Fxxx) - CLAUDE.md's
//  one-TU-per-original-module rule.
//============================================================================

//  a = rol((entry+0x20) ^ (entry+0x1C), 3)
//  b = rol((entry+0x28) ^ (entry+0x18), 3)
//  a = rol(a ^ b, 3)
//  c = rol((entry+0x2C) ^ (entry+0x24), 3)
//  a = rol(a ^ c, 3)
//  a = rol(a ^ (entry+0x30), 3)
unsigned int __fastcall PlayerTallyEntryChecksumFields(void* entryPtr)
{
    const char* entry = (const char*)entryPtr;

    unsigned int a = *(const unsigned int*)(entry + 0x20) ^ *(const unsigned int*)(entry + 0x1C);
    a = (a << 3) | (a >> 29);

    unsigned int b = *(const unsigned int*)(entry + 0x28) ^ *(const unsigned int*)(entry + 0x18);
    b = (b << 3) | (b >> 29);

    a ^= b;
    a = (a << 3) | (a >> 29);

    unsigned int c = *(const unsigned int*)(entry + 0x2C) ^ *(const unsigned int*)(entry + 0x24);
    c = (c << 3) | (c >> 29);

    a ^= c;
    a = (a << 3) | (a >> 29);

    a ^= *(const unsigned int*)(entry + 0x30);
    a = (a << 3) | (a >> 29);

    return a;
}
