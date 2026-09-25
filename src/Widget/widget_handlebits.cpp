//============================================================================
//  0x6F4762B0 - SHandleWithType::SetLow24Bits, the leaf CWidget's vtable
//  slot 87 (widget_vtable_leaves.cpp) hands its argument to.
//
//  Own translation unit: slot 87 reaches it with a real `call` from a
//  different module (0x6F266xxx against this function's 0x6F476xxx), and
//  it in turn makes a real call to LookupHandle (handle.cpp).
//
//  A member of SHandleWithType (timesyncbounds.h) and not a free function:
//  `this` arrives in ecx, the one argument on the stack, and it cleans that
//  argument itself (`retn 4`).  The two fields it reads off `this` are
//  m_handle (+0x08) and m_typeTag (+0x0C) - exactly the pair
//  SHandleWithType already declares, and exactly the pair CAgent's own
//  vtable slot 6 (0x6F472AE0) resolves the same way one field further in.
//============================================================================
#include "timesyncbounds.h"
#include "game.h"       // CHandleObject, LookupHandle

//----------------------------------------------------------------------------
//  What the resolved handle object parks at +0x98: an object whose +0x34
//  dword is split 24/8.  The shipped store is the read-xor-mask-xor
//  sequence MSVC emits for assigning to a bitfield at the bottom of a dword
//  and for nothing else (widget.h makes the same argument for
//  SWidgetColorBits); a hand-written
//  `x = (x & ~0xFFFFFF) | (v & 0xFFFFFF)` narrows to an `and` with the
//  complement instead.
//----------------------------------------------------------------------------
struct SHandleColorBits
{
    unsigned int m_low24  : 24;   // +0x34, bits 0-23
    unsigned int m_high8  : 8;    // +0x34, bits 24-31 - left alone
};

struct SHandleTarget
{
    char              m_reserved00[0x34];
    SHandleColorBits  m_bits;     // +0x34
};

//  What the resolved handle object parks at +0x98.  Reached by offset off
//  the CHandleObject* LookupHandle returns rather than by extending
//  CHandleObject itself: game.h's own note explains why growing that base
//  would silently move CPathTrace's fields, and this call tree does not
//  establish which concrete type actually carries +0x98.
struct SHandleOwnerRecord
{
    char            m_reserved00[0x98];
    SHandleTarget*  m_target;     // +0x98
};

//----------------------------------------------------------------------------
//  0x6F4762B0.  No null check on the lookup: the shipped code loads +0x98
//  off whatever comes back, so a handle that no longer resolves faults
//  here.  Reproduced as written rather than "fixed".
//----------------------------------------------------------------------------
void SHandleWithType::SetLow24Bits(unsigned int bits)
{
    SHandleOwnerRecord* owner =
        (SHandleOwnerRecord*)LookupHandle(m_handle, m_typeTag);

    //  Through a pointer to the bitfield word, not `...->m_bits.m_low24 =`
    //  directly: the shipped code forms the address once (`add eax, 34h`)
    //  and stores through it, where the direct member spelling folds the
    //  displacement into both the load and the store and loses that
    //  instruction.
    SHandleColorBits* word = &owner->m_target->m_bits;
    word->m_low24 = bits;
}

//----------------------------------------------------------------------------
//  0x6F4762E0 - reached off CUnit's vtable slot 91 (RefreshPlacementRadius,
//  0x6F28D4F0's own 13-function closure).
//
//  What's parked at +0xA8 - a distinct offset from SetLow24Bits' own +0x98,
//  so a different field on the same (still-unestablished) concrete owner
//  type - is a CPathClient* (pathmove.h): the callee at sub_6F49AD10 turns
//  out to already be one of CPathClient's own address neighbours
//  (0x6F49ACA0/0x6F49AD30/0x6F49AD40, pathclientstate.cpp), and its own
//  body (`this->m_typeMask = *ptr`) matches
//  CPathSearch::TestFootprint's identical "mask arrives as a pointer, one
//  dereference, straight into +0x9C/+0xA4" shape (pathfootprintsize.cpp)
//  exactly - CPathClient::SetTypeMask.  The packed byte is therefore a
//  collision-type mask with the caller's own low byte smeared into its top
//  byte, the same top-byte-plus-low-24-bits shape CPathClient::m_typeMask's
//  own header comment already documents being staged onto CPathSearch's
//  twin.
//----------------------------------------------------------------------------
#include "pathmove.h"   // CPathClient::SetTypeMask, 0x6F49AD10

void SHandleWithType::PropagatePlacementIndex(unsigned int arg0)
{
    CPathClient* client =
        *(CPathClient**)((char*)LookupHandle(m_handle, m_typeTag) + 0xA8);

    if (client != 0)
    {
        //  (arg0 << 24) | (arg0 & 0xFFFFFF) - the top byte of arg0
        //  smeared into the top byte of the packed word, the bottom
        //  three bytes left as arg0's own.  Reassigned into arg0's own
        //  stack slot (not a fresh local) so that taking its address for
        //  SetTypeMask's pointer argument reads back the packed value
        //  rather than the original one, matching the shipped
        //  `lea eax, [esp+arg_0]` / `push eax` /
        //  `mov [esp+4+arg_0], edx` in-place overwrite.
        arg0 = (arg0 << 24) | (arg0 & 0xFFFFFFu);
        client->SetTypeMask((const int*)&arg0);
    }
}
