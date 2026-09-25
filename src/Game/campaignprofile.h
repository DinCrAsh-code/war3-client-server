//============================================================================
//  CampaignProfile - one entry of the campaign list the game-cache store
//  keeps.  The name is an RTTI descriptor: every free of the array that
//  holds them passes ".PAVCampaignProfile@@" (0x6FA7A380), and the leading
//  `PAV` is MSVC's "pointer to class" marker, not part of the name.
//
//  Only the two members this tree reads are named: the profile's own name
//  is its first field (its accessor is a bare `mov eax, ecx`) and the
//  0x104-byte directory the caches under it live in is at +0x168.
//============================================================================
#ifndef CAMPAIGNPROFILE_H
#define CAMPAIGNPROFILE_H

struct CampaignProfile
{
    //  0x6F3D9FC0 - `mov eax, ecx`: the name is at offset zero.
    const char* Name();
    //  0x6F3D9FD0 - `lea eax, [ecx+168h]`.
    char* PathBuffer();

    char m_name[0x168];         // +0x000
    char m_path[0x104];         // +0x168
};

//----------------------------------------------------------------------------
//  The array of them, with its own cursor - see objectdatacursor.cpp for
//  0x6F3DF380, which reads the element the cursor is on.
//----------------------------------------------------------------------------
struct SCursoredArray
{
    //  0x6F3DA2A0-family lookups: both are members (`retn 4` against a
    //  receiver in ecx and one push), not free `__fastcall`s.
    CampaignProfile* FindByName(const char* name);      // 0x6F3DF2A0
    int              IndexOfName(const char* name);     // 0x6F3DF300

    unsigned int      m_alloc;      // +0x00
    unsigned int      m_count;      // +0x04
    CampaignProfile** m_data;       // +0x08
    char              m_reservedC[0x04];
    unsigned int      m_cursor;     // +0x10
};

#endif
