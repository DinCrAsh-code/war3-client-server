//============================================================================
//  0x6F4D3200 / 0x6F4D3340 / 0x6F4D3190 / 0x6F4D3220 / 0x6F4D3270 - the
//  small CSprite setters, all of them reached from
//  CWar3Image::CreateSpriteFromDescriptor (0x6F26BCD0, widgetsprite.cpp) and
//  from CWidget::InitSprite itself.
//
//  They share one translation unit because none of them calls another - the
//  shipped code has three separate "set some bits in +0x28" entry points
//  that do not forward to each other, which is why SetSpritePausedBit spells
//  its single 0x40000 bit out rather than calling SetSpriteStateBits with
//  it.
//============================================================================
#include "sprite.h"

//  The float goes by *value*: the shipped call site reserves a stack
//  slot with `push ecx` and writes the float straight into it with
//  `fstp [esp]`.  Passing its address instead emits a `lea`/`push`
//  pair and loses the `fld`.
typedef void (__thiscall *SetFloatFn)(void*, float);
typedef void (__thiscall *SetIntFn)(void*, const int*);
typedef void (__thiscall *RefreshFn)(void*);
typedef int  (__thiscall *QueryFn)(void*);
typedef int* (__thiscall *QueryRecordFn)(void*, int* scratch);
typedef void (__thiscall *SetValueFn)(void*, int);

//----------------------------------------------------------------------------
//  0x6F4D3200.  `bits` is only ever 16 bits wide: the shipped code reads it
//  out of edx with `movzx eax, dx` on the set path and `movzx edx, dx` on
//  the clear path, so it is an unsigned short parameter, not an int masked
//  down.
//----------------------------------------------------------------------------
void __fastcall SetSpriteStateBits(CSprite* self, unsigned short bits, int set)
{
    if (set)
        self->m_stateBits |= bits;
    else
        self->m_stateBits &= ~(unsigned int)bits;
}

//----------------------------------------------------------------------------
//  0x6F4D3340.
//----------------------------------------------------------------------------
void __fastcall SetSpritePausedBit(CSprite* self, int paused)
{
    if (paused)
        self->m_stateBits |= 0x40000;
    else
        self->m_stateBits &= ~0x40000;
}

//----------------------------------------------------------------------------
//  0x6F4D3190 - vtable slot 0x24, the float forwarded by address.
//----------------------------------------------------------------------------
void __fastcall SetSpriteAnimationScale(CSprite* self, float scale)
{
    ((SetFloatFn)(*(void***)self)[0x24 / 4])(self, scale);
}

//----------------------------------------------------------------------------
//  0x6F4D3220 - vtable slot 0x30 with the colour's address, then slot 0x14
//  with nothing, only when `refresh` is set.  `refresh` arrives in edx and
//  the colour on the stack, which is why the parameters are in this order.
//----------------------------------------------------------------------------
void __fastcall SetSpriteTeamColor(CSprite* self, int refresh,
                                   CTeamColorSlot color)
{
    ((SetIntFn)(*(void***)self)[0x30 / 4])(self, &color.m_value);
    if (refresh)
        ((RefreshFn)(*(void***)self)[0x14 / 4])(self);
}

//----------------------------------------------------------------------------
//  0x6F4D3270 - `self` is moved into eax first and the model instance read
//  into ecx, so the release call takes the *model* as its `this` and the
//  sprite as its one stack argument, not the other way round.
//----------------------------------------------------------------------------
void __fastcall ReleaseSpriteModel(CSprite* self, void* notify)
{
    if (self->m_pModel != 0)
        ModelInstanceReleaseForSprite(self->m_pModel, notify, self);
}

//----------------------------------------------------------------------------
//  0x6F4D3160 - vtable slot 0x4C, forwarded with nothing added.  A bare
//  `mov eax,[ecx]` / `mov edx,[eax+4Ch]` / `jmp edx`, which is what MSVC
//  emits for a virtual call in return position - the same non-virtual-
//  interface shape SetSpriteAnimationScale above documents.
//----------------------------------------------------------------------------
int __fastcall QuerySpriteSlot4C(CSprite* self)
{
    return ((QueryFn)(*(void***)self)[0x4C / 4])(self);
}

//----------------------------------------------------------------------------
//  0x6F4D3140 - vtable slot 0x48, one slot before QuerySpriteSlot4C and the
//  identical bare tail-jump shape.  Its callers dereference the answer once
//  more (`mov eax,[eax]`), so it hands back a pointer rather than a plain
//  int/bool.
//----------------------------------------------------------------------------
typedef void* (__thiscall *QueryPtrFn)(void*);

void* __fastcall QuerySpriteSlot48(CSprite* self)
{
    return ((QueryPtrFn)(*(void***)self)[0x48 / 4])(self);
}

//----------------------------------------------------------------------------
//  0x6F4D3250 - slot 0x34 with the value in edx, then slot 0x14 when the
//  caller asked for a refresh.  `value` before `refresh` for the same
//  reason SetSpriteTeamColor's parameters are in their order: the value
//  arrives in edx and the flag on the stack.
//----------------------------------------------------------------------------
void __fastcall SetSpriteSlot34(CSprite* self, int value, int refresh)
{
    ((SetValueFn)(*(void***)self)[0x34 / 4])(self, value);
    if (refresh)
        ((RefreshFn)(*(void***)self)[0x14 / 4])(self);
}

//----------------------------------------------------------------------------
//  0x6F4D4BB0 - slot 0x50 hands back a pointer to a record, given a scratch
//  slot of its own to work in; only that record's first dword is wanted.
//  The scratch slot really is the shipped code's (`push ecx` to reserve it,
//  `lea edx,[esp+4]` to pass it) and its contents are never read here.
//----------------------------------------------------------------------------
void __fastcall QuerySpriteSlot50Word(CSprite* self, int* out)
{
    int scratch;
    *out = *((QueryRecordFn)(*(void***)self)[0x50 / 4])(self, &scratch);
}
