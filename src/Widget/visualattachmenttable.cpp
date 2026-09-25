//============================================================================
//  0x6F741BE0 - one bit out of a visual-attachment slot's flags word.
//
//  The table is an array of 0xA0-byte records hanging off +0x2CC of whatever
//  owns it (`lea eax, [eax+eax*4]` / `shl eax, 5` is index * 160), and the
//  flags live in the record's second dword.  Bit 8 is the one this reads;
//  nothing here says what it means, so the accessor is named for the bit and
//  not for a guess at its meaning.
//
//  Reached from AttachmentSetScale (0x6F00DB60, still a redirect thunk in
//  configgates.cpp), which is the visual-attachment gate CSelectable's slots
//  13/105/106 poke.
//============================================================================

struct SVisualAttachmentRecord
{
    char         m_reserved00[4];
    unsigned int m_flags;              // +0x04
    char         m_reserved08[0xA0 - 8];
};

class CVisualAttachmentTable
{
public:
    //  0x6F741BE0
    int IsSlotFlagBit8Set(int slot);

    char                      m_reserved0000[0x2CC];
    SVisualAttachmentRecord*  m_records;   // +0x2CC
};

int CVisualAttachmentTable::IsSlotFlagBit8Set(int slot)
{
    return (m_records[slot].m_flags >> 8) & 1;
}
