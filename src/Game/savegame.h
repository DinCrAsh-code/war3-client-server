//============================================================================
//  CSaveGame - the reader for a Warcraft III saved game (`.w3z`).  The name
//  is stamped: 0x6F35D4C0 writes `??_7CSaveGame@@6B@` into the object it is
//  building.
//
//  Only the fields RestartGame's own path actually reaches are named.  The
//  three counted strings at +0x40C, +0x418 and +0x424 and the slot array at
//  +0x3EC are pinned by the constructor and destructor between them; the
//  version gates (0x1770, 0x178B, 0x17AA, 0x17AB, 0x11DC) are the shipped
//  ones and are what decides which of the three GAMESETUP layouts the header
//  carries.
//
//  Those three layouts are named by the allocation tags the slot array's own
//  resize passes Storm: ".?AUSLOT@GAMESETUP_WAR3_0001@Net@@" at seven bytes
//  an element, "..._0002" at eight and "..._0003" at nine.  The array the
//  save game keeps is always the nine-byte one; the older two are read into
//  a temporary and widened.
//============================================================================
#ifndef SAVEGAME_H
#define SAVEGAME_H

#include "storm.h"
#include "cdatastore.h"
#include "jassnatives.h"        // STStringField
#include "cgamedescstore.h"     // Net::GAMEDESC

namespace Net
{

//  Nine bytes, not twelve: the copy loop moves a dword, a dword and a byte,
//  and the array strides by nine.
#pragma pack(push, 1)
struct GAMESETUP_SLOT
{
    unsigned char m_playerId;       // +0x00
    unsigned char m_byte1;          // +0x01
    unsigned char m_state;          // +0x02 - 2 is "this slot is a player"
    unsigned char m_byte3;          // +0x03
    unsigned char m_reserved4[4];   // +0x04
    unsigned char m_byte8;          // +0x08
};

//  Seven bytes - `TSFixedArray<...>::SetAlloc`'s own move loop for this
//  instantiation (0x6F654040) copies a dword, a word and a byte, and the
//  array strides by seven (`lea edx,[eax*8]` / `sub edx,eax`).
struct GAMESETUP_SLOT_0001
{
    int   m_a;      // +0x00
    short m_b;      // +0x04
    char  m_c;      // +0x06
};
#pragma pack(pop)

//  Eight bytes - 0x6F6540D0's move loop copies two dwords and the array
//  strides by eight.  No packing: two dwords are already eight bytes.
struct GAMESETUP_SLOT_0002
{
    int m_a;        // +0x00
    int m_b;        // +0x04
};

//----------------------------------------------------------------------------
//  What the three readers below actually fill: the element array plus three
//  trailing fields the same serialiser writes after the elements.  0x12
//  bytes, and it is what CSaveGame keeps at +0x3EC - the constructor only
//  zeroes the array's own three words, and the three trailing fields are
//  written by the read.
//
//  One struct per element type rather than a template, because each is a
//  separate shipped instantiation with its own SetAlloc/SetCount pair and
//  its own Storm allocation tag.
//----------------------------------------------------------------------------
struct GAMESETUP_SLOTS
{
    TSFixedArray<GAMESETUP_SLOT> m_array;   // +0x00
    unsigned int  m_field0C;                // +0x0C
    unsigned char m_field10;                // +0x10
    unsigned char m_field11;                // +0x11
};

struct GAMESETUP_SLOTS_0001
{
    TSFixedArray<GAMESETUP_SLOT_0001> m_array;
    unsigned int  m_field0C;
    unsigned char m_field10;
    unsigned char m_field11;
};

struct GAMESETUP_SLOTS_0002
{
    TSFixedArray<GAMESETUP_SLOT_0002> m_array;
    unsigned int  m_field0C;
    unsigned char m_field10;
    unsigned char m_field11;
};

//----------------------------------------------------------------------------
//  The wire form of the table above, and what the world's session keeps at
//  +0x33C: a two-byte length followed by that many bytes of the same 653
//  message the array reader parses.  0x80 is the bound both sides test - it
//  is the CDataStoreCache<128> the encoder builds it in.
//----------------------------------------------------------------------------
struct GAMESETUP_SLOTRECORD
{
    unsigned short m_length;        // +0x00
    char           m_data[0x80];    // +0x02
};

}  // namespace Net

class CSaveGame
{
public:
    //  0x6F443860 - retn 4.  Read `path`'s header into it; 0 if it would
    //  not.  A **member**: the shipped body takes `this` in ecx and the
    //  path on the stack and pops it, so a free `__fastcall` taking both in
    //  registers pushes nothing where the callee cleans four bytes - which
    //  is what this was declared as while it was a redirect, and is a live
    //  stack shift in every caller (CLAUDE.md's own thunk_abi_audit table).
    int ReadHeader(const char* path);

    void*   m_vtable;                       // +0x000
    char    m_reserved004[0x344];           // +0x004
    Net::GAMEDESC m_header;                 // +0x348 - the decoded header
    //  0x14 bytes with its tail padding, which is what puts m_valid at
    //  +0x400 - the two bytes past +0x3FD are the struct's own.
    Net::GAMESETUP_SLOTS m_slots;           // +0x3EC .. +0x3FF
    int     m_valid;                        // +0x400
    int     m_word404;                      // +0x404
    int     m_word408;                      // +0x408
    STStringField m_mapPath;                // +0x40C
    STStringField m_string418;              // +0x418
    STStringField m_string424;              // +0x424
    int     m_word430;                      // +0x430
    int     m_word434;                      // +0x434
    int     m_block438[5];                  // +0x438 .. +0x44B
};

//: ??_7CSaveGame@@6B@ - stamped, never dispatched.  Local, for the reason
//: src/Misc/cstringrep.h gives.
extern void* g_vftableCSaveGame[1];

//: 0x6F35D4C0 - retn 0.  Build one.
CSaveGame* __fastcall SaveGameConstruct(CSaveGame* self);

//: 0x6F34D9A0 - retn 0.  Tear one down.
void __fastcall SaveGameDestruct(CSaveGame* self);



#endif
