//============================================================================
//  0x6F514840 - does this object's +0x44 sub-object select between more than
//  one variant?
//
//  Two flag bits ask the same question of the same count, and the shipped
//  code asks it twice rather than folding them into one mask - which is what
//  says the source has two separate tests and not `(flags & 0x0A)`.
//============================================================================
#include "modelpick.h"

struct PICKVARIANTOWNER
{
    char          m_reserved00[0x1C];   // +0x00
    unsigned int  m_count;              // +0x1C
    char          m_reserved20[0x98];   // +0x20
    unsigned char m_flags;              // +0xB8
};

struct PICKVARIANTHOLDER
{
    char               m_reserved00[0x44];  // +0x00
    PICKVARIANTOWNER*  m_owner;             // +0x44
};

int __fastcall HasVariantSelection_6F514840(const void* self)
{
    const PICKVARIANTOWNER* owner = ((const PICKVARIANTHOLDER*)self)->m_owner;
    unsigned char flags = owner->m_flags;

    if ((flags & 8) != 0 && owner->m_count > 1)
        return 1;
    if ((flags & 2) != 0 && owner->m_count > 1)
        return 1;
    return 0;
}
