//============================================================================
//  CSprite and the model it drives, as far as `JASS_PauseGame`'s closure
//  reaches them.
//
//  CSprite and CModelInstance are Widget/sprite.h's, which this file
//  includes through material.h: 0x6F4D32E0 reads exactly the +0x20 that
//  header already calls m_pModel.  The two attachment-slot layouts below are
//  local to their own translation units because nothing else reaches them.
//============================================================================
#ifndef SPRITEATTACH_H
#define SPRITEATTACH_H

#include "material.h"

//  Three floats, moved as three dwords: every copy of one in this module is
//  an integer move pair, which is a struct assignment and not three `float`
//  assignments.
struct C3VECTOR
{
    float x;
    float y;
    float z;
};

//  0x6F4D32E0 - bind a texture into one of the sprite's model's slots.
void __fastcall SpriteSetTexture(CSprite* sprite, CTexture* texture, int id);

//  0x6F4D3100 - the agent's world position, through its own vtable slot
//  0x3C, which returns the vector by value into a buffer the caller gives
//  it and answers that buffer in eax.
void __fastcall GetAgentWorldPosition(void* agent, float* out);

//  0x6F4E80D0 and 0x6F4E8260 are GameUI/controllerslot.h's
//  ReadControllerSlotValue and QuerySlotVec3.  One shipped address per C++
//  name: the slot reader is reached from the camera controller and from the
//  sprite alike, because it reads slot N's three floats out of whatever
//  object it is handed.  ReadControllerSlotValue is in tools/unhookable.py -
//  the shipped body takes its host in eax and its destination in edi and
//  cleans nothing, which no MSVC spelling reaches.
#include "controllerslot.h"

//  0x6F4E82B0 - clear one attachment's colour track and hand it a new one.
void __fastcall SpriteSetChannelColor(void* object, int index,
                                      const unsigned int* color);

#endif
