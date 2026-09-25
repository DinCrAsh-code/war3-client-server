//============================================================================
//  0x6F741920 / 0x6F741970 - the two visual-attachment writers on the large
//  Storm singleton (stormsingletona.h), reached from CSelectable's own
//  vtable slot 104 through the two one-line wrappers in
//  stormsingleton_attach.cpp.
//
//  Both index the same 0xA0-byte record array at +0x2CC, both set the
//  record's "dirty" bit (8) on the way out, and both return the address of
//  the flags word they just touched - which neither caller in this family
//  reads.
//
//  Own translation unit: their callers reach them with real calls, and
//  neither of these two calls the other.
//============================================================================
#include "stormsingletona.h"

//----------------------------------------------------------------------------
//  0x6F741920 - the Z offset, plus the "on walkable ground" bit.
//----------------------------------------------------------------------------
unsigned int* SStormSingletonA::SetAttachmentWalkableZ(int index, int onWalkable,
                                                       float z)
{
    //  Written through m_attachments each time rather than through one
    //  cached `SAttachmentRecord* rec` local: the shipped code reloads
    //  +0x2CC before each of the three accesses, which is what repeating
    //  the member access gives and a local does not.
    if (onWalkable)
        m_attachments[index].m_flags |= 0x10;
    else
        m_attachments[index].m_flags &= ~0x10u;

    m_attachments[index].m_z = z;

    //  The last two accesses *do* share one load, and the shipped code
    //  says so: `mov ecx,[ecx+2CCh]` once, then `or [ecx+eax+4],8` and
    //  `lea eax,[ecx+eax+4]` off the same base.  Repeating the member
    //  access here instead adds a third reload and a second `lea`.
    SAttachmentRecord* records = m_attachments;
    records[index].m_flags |= 8;
    return &records[index].m_flags;
}

//----------------------------------------------------------------------------
//  0x6F741970 - the world position, copied component by component.
//----------------------------------------------------------------------------
unsigned int* SStormSingletonA::SetAttachmentPosition(int index, const FVec3* pos)
{
    //  A whole-struct assignment, not three float stores: the shipped code
    //  moves the three components through an integer register
    //  (`mov edi,[esi]` / `mov [edx+eax+0Ch],edi`), which is what a POD
    //  copy compiles to and what three `fld`/`fstp` pairs do not.
    m_attachments[index].m_pos = *pos;

    //  One shared load for the last two accesses - see the sibling above.
    SAttachmentRecord* records = m_attachments;
    records[index].m_flags |= 8;
    return &records[index].m_flags;
}

//----------------------------------------------------------------------------
//  0x6F741A50 - the colour.  Reconstructed: the dump that carries its body
//  (asm/sub_6F285770_...) arrived after the comment that used to sit here
//  said none did.
//
//  The odd one out of the three: it writes the record and stops.  Neither
//  the dirty bit its two siblings set nor their `return &m_flags` is in the
//  shipped body - it is a plain `retn 8` with nothing meaningful in eax.
//
//  The colour is read through the argument pointer as one dword
//  (`mov edx,[edx]` / `mov [ecx+eax+48h],edx`), not as four bytes, so the
//  parameter is the packed word's address and the store is a whole-word
//  copy.
//----------------------------------------------------------------------------
void SStormSingletonA::SetAttachmentColor(int index, const unsigned int* color)
{
    m_attachments[index].m_color = *color;
}

//----------------------------------------------------------------------------
//  0x6F7419B0 - bit 0x200.  Same shape as SetAttachmentWalkableZ: the
//  set/clear reads +0x2CC once, the dirty-bit-and-return tail reloads it a
//  second time, matching the shipped code's own repeated `mov ecx,[ecx+2CCh]`.
//----------------------------------------------------------------------------
unsigned int* SStormSingletonA::SetAttachmentFlagBit9(int index, int on)
{
    if (on)
        m_attachments[index].m_flags |= 0x200;
    else
        m_attachments[index].m_flags &= ~0x200u;

    SAttachmentRecord* records = m_attachments;
    records[index].m_flags |= 8;
    return &records[index].m_flags;
}

//----------------------------------------------------------------------------
//  0x6F741A00 - bit 0x400.  Identical shape to SetAttachmentFlagBit9, one
//  bit over.
//----------------------------------------------------------------------------
unsigned int* SStormSingletonA::SetAttachmentFlagBit10(int index, int on)
{
    if (on)
        m_attachments[index].m_flags |= 0x400;
    else
        m_attachments[index].m_flags &= ~0x400u;

    SAttachmentRecord* records = m_attachments;
    records[index].m_flags |= 8;
    return &records[index].m_flags;
}
