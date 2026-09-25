//============================================================================
//  Net::CGameDescStore and the game description it decodes.
//
//  The class name is stamped: 0x6F654510 writes `??_7CGameDescStore@Net@@6B@`
//  (0x6F970E80) over the CDataStoreScratch base it has just built, which is
//  also what says the two are related by inheritance rather than by luck.
//  The object is a scratch buffer with a 0x68-byte inline store: the game
//  description arrives obfuscated in 0x80 bytes of the save-game header
//  block, is unscrambled *into* the store, and is then read back out of it
//  field by field.
//============================================================================
#ifndef CGAMEDESCSTORE_H
#define CGAMEDESCSTORE_H

#include "cdatastorescratch.h"

//----------------------------------------------------------------------------
//  The block one saved-game header read (0x6F6521B0,
//  netmsgio_652_readheader.cpp) puts on the stack: three NUL-terminated
//  strings then two dwords.  Declared here rather than in that file so the
//  decoder below and the reader agree on one type.
//----------------------------------------------------------------------------
struct GAMEDESCNAME { char m_text[0x20]; };
struct GAMEDESCPASS { char m_text[0x10]; };

struct SSaveGameHeaderBlock
{
    GAMEDESCNAME m_name;            // +0x00
    GAMEDESCPASS m_short;           // +0x20
    char         m_long[0x80];      // +0x30
    unsigned int m_wordB0;          // +0xB0
    unsigned int m_wordB4;          // +0xB4
};

namespace Net
{

//  GAMEDESCNAME / GAMEDESCPASS are two named sub-objects rather than two
//  bare arrays: 0x6F654510 copies each of them whole out of the header
//  block, which is a struct assignment (`rep movsd` at 32 bytes, four inline
//  dword moves at 16) and not a field by field copy.  They are declared
//  above, at global scope, because the header block shares them.

//----------------------------------------------------------------------------
//  0xA4 bytes, and exactly what CSaveGame keeps at +0x348.
//----------------------------------------------------------------------------
struct GAMEDESC
{
    ::GAMEDESCNAME m_name;          // +0x00
    ::GAMEDESCPASS m_password;      // +0x20
    unsigned char  m_byte30;        // +0x30
    unsigned char  m_playerCount;   // +0x31 - refused above 2 by the header
                                    //         validator, so a slot index
    unsigned char  m_reserved32[2]; // +0x32
    unsigned int   m_word34;        // +0x34
    unsigned short m_word38;        // +0x38
    unsigned short m_word3A;        // +0x3A
    unsigned int   m_word3C;        // +0x3C
    unsigned int   m_block40[5];    // +0x40 - the 20-byte block, defaulted
                                    //         from dword_6F970E38 when the
                                    //         stream carries none
    char           m_mapPath[0x36]; // +0x54
    char           m_creator[0x10]; // +0x8A
    unsigned char  m_reserved9A[2]; // +0x9A
    unsigned int   m_word9C;        // +0x9C
    unsigned int   m_wordA0;        // +0xA0
};

//----------------------------------------------------------------------------
//  The scratch store the obfuscated description is recovered into.  It adds
//  no state of its own - the buffer is a caller-supplied local - only the
//  vtable stamp and the unscrambler.
//----------------------------------------------------------------------------
class CGameDescStore : public CDataStoreScratch
{
public:
    CGameDescStore(void* buffer, void* size) : CDataStoreScratch(buffer, size) {}

    //  0x6F650820
    int Unscramble(const char* src);
};

}  // namespace Net

//  0x6F654510 - Net/cgamedescstore.cpp.  Decode one save game's header
//  block into a GAMEDESC; 0 if what came out is not a description.
int __fastcall GameDescDecode(Net::GAMEDESC* desc, const SSaveGameHeaderBlock* block);

#endif
