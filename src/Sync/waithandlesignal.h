//============================================================================
//  See waithandlesignal.cpp for the class-level notes.
//============================================================================
#ifndef WAITHANDLESIGNAL_H
#define WAITHANDLESIGNAL_H

//----------------------------------------------------------------------------
//  A generic interface reached through exactly one of its slots
//  (vtable+0x24, index 9) - the same "reserved slot" technique
//  vslot4thunk.h already uses for an unidentified interface this call tree
//  only ever calls through one entry of. Nothing in NetProviderLTCP__Method_0xC's
//  own call tree gives any of the other nine slots a job, or names the real
//  interface.
//----------------------------------------------------------------------------
class IWaitHandleSignal
{
public:
    virtual void m_reservedSlot0() {}
    virtual void m_reservedSlot1() {}
    virtual void m_reservedSlot2() {}
    virtual void m_reservedSlot3() {}
    virtual void m_reservedSlot4() {}
    virtual void m_reservedSlot5() {}
    virtual void m_reservedSlot6() {}
    virtual void m_reservedSlot7() {}
    virtual void m_reservedSlot8() {}
    virtual void Signal() = 0;   // vtable+0x24 (slot 9)
};

//  0x6F6DAD30 - null-checked dispatch through vtable+0x24 (a diagnostic
//  wrapper of the same "AddRef/Release-style" shape NETCONN's own
//  AddRef/Release use, but for a call that never returns to its caller on
//  the non-null path - a genuine `jmp`, not a `call`).
void __fastcall SignalWaitHandle(IWaitHandleSignal* self);

#endif
