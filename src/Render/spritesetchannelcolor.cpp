//============================================================================
//  0x6F4E82B0 - clear one attachment's colour track and hand it a new
//  colour through vtable slot 3.
//
//  The shipped body is a two-instruction head into a block IDA lists as a
//  separate chunk at 0x6F4E8180, and the tail is a `jmp` into the slot -
//  which is what makes the whole thing `retn 4`, cleaned by the slot itself.
//============================================================================
#include "spriteattach.h"

struct SPRITECOLORSLOT
{
    void*        m_vtable;          // +0x00
    char         m_reserved04[0x0C];// +0x04
    unsigned int m_trackA;          // +0x10
    unsigned int m_trackB;          // +0x14
    float        m_blend;           // +0x18
};

struct SPRITECOLOROWNER
{
    char               m_reserved00[0x10]; // +0x00
    SPRITECOLORSLOT**  m_slots;            // +0x10
};

void __fastcall SpriteSetChannelColor(void* object, int index,
                                      const unsigned int* color)
{
    SPRITECOLORSLOT* slot = ((SPRITECOLOROWNER*)object)->m_slots[index];

    slot->m_blend  = 0.0f;
    slot->m_trackA = 0;
    slot->m_trackB = 0;

    typedef void (__thiscall *SetColorFn)(SPRITECOLORSLOT*,
                                          const unsigned int*);
    ((SetColorFn)(*(void***)slot)[3])(slot, color);
}
