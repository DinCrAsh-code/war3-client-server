//============================================================================
//  0x6F474980 - PositionTrack::StoreTopByteBits.
//
//  Resolve the {handle, typeTag} pair PositionTrack keeps at its own
//  +0x08/+0x0C (inherited from CPresenceRef, Agent/presenceref.h) through
//  the game-wide handle table, walk to the record that object parks at
//  +0x94, and rewrite the *top byte* of the dword at +0x3C of it - clearing
//  whatever `mask` selects and OR-ing `value` in, leaving the low 24 bits
//  of the same dword alone.
//
//  The low 24 bits are load-bearing elsewhere: CItem::RefreshPlacementRadius
//  (Item/item_radius.cpp) writes them through SHandleWithType::SetLow24Bits
//  on a record reached the same way, which is why this one is careful to
//  preserve them rather than storing a whole dword.
//
//  Moved here from Widget/position_flagbyte.cpp once Widget/position.h's
//  own `Position` was confirmed to be this exact class, not a separate one
//  - see positiontrack.h's own header comment. Same address, same body,
//  same verdict.
//
//  Own translation unit: 0x6F4749E0 (positiontrackflagbyte2.cpp) is its one
//  caller in this tree and reaches it with a real `call`, so the two
//  cannot share a file or MSVC folds the callee in and the call vanishes.
//============================================================================
#include "positiontrack.h"
#include "game.h"

//----------------------------------------------------------------------------
//  What the resolved handle object parks at +0x94: a record whose +0x3C
//  dword is split into a 24-bit value and a byte of flags above it.  Only
//  the two halves this function touches are named.
//----------------------------------------------------------------------------
struct SPathFlagRecord
{
    char          m_reserved00[0x3C];
    unsigned int  m_packed;         // +0x3C - low 24 bits value, +0x3F flags

    //  +0x3F reached as a byte rather than as `m_packed >> 24`: the shipped
    //  code loads it with `movzx ecx, byte ptr [eax+3Fh]`, which is what a
    //  byte lvalue at that address compiles to and what a shift of the
    //  whole dword does not.
    unsigned char* TopByte() { return (unsigned char*)&m_packed + 3; }
};

struct SPathFlagOwner
{
    char              m_reserved00[0x94];
    SPathFlagRecord*  m_pFlags;     // +0x94
};

void PositionTrack::StoreTopByteBits(unsigned char value, unsigned char mask)
{
    SPathFlagRecord* rec =
        ((SPathFlagOwner*)LookupHandle(m_handle, m_typeTag))->m_pFlags;

    unsigned int bits = (*rec->TopByte() & (unsigned char)~mask) | value;
    rec->m_packed = (bits << 24) | (rec->m_packed & 0xFFFFFF);
}
