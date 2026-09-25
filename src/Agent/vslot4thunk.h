//============================================================================
//  See vslot4thunk.cpp for the class-level notes.
//============================================================================
#ifndef VSLOT4THUNK_H
#define VSLOT4THUNK_H

class IVSlot4Target
{
public:
    virtual void m_reservedSlot0() = 0;
    virtual void m_reservedSlot1() = 0;
    virtual void m_reservedSlot2() = 0;
    virtual void m_reservedSlot3() = 0;
    virtual int  Forward() = 0;   // vtable+0x10
};

int __fastcall CallVSlot4(IVSlot4Target* self);

#endif
