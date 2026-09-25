//============================================================================
//  0x6F471FD0 - deliver a value through the object's own embedded observer,
//  pinned by a temporary reference for the duration of the delivery.
//
//  Was a `mov eax,<address> / jmp eax` redirect.  Twenty-three instructions,
//  and the shape is one this repo already reconstructs twice over
//  (CObserver::Method_0x14, observer_slots.cpp): bump my own reference
//  count, run the callback, drop it again, and destroy myself through slot 0
//  if that drop was the last one.  The guard exists because the callback can
//  and does release the last outside reference to this object.
//
//  The observer at +0x14 is *embedded*, not pointed to: the shipped code
//  reads its vtable as `mov eax,[esi+14h]` but hands it `lea ecx,[esi+14h]`
//  - the address of the field, not its contents - so the field's first word
//  is the sub-object's own vtable pointer.  Slot 5 (+0x14 of that vtable) is
//  the entry point, and it takes the two arguments in the order they arrive.
//
//  The third argument is never read.  Both callers (agent_handlers.cpp) pass
//  the value's own sign bit; nothing here looks at it, so it stays an
//  unnamed parameter rather than an invented use.
//
//  The return value is the callback's, forwarded: the shipped code keeps it
//  in edi across the release and restores it (`mov eax,edi`) only on the
//  path that ran the destructor, because the other path still has it in eax.
//  Declaring this `void` costs the forward and one instruction.
//
//  Own translation unit: its two callers reach it with real calls, and it
//  makes two indirect calls of its own.
//============================================================================

//  Slot 5 of the embedded observer's own vtable.
typedef int (__thiscall* ObserverDeliverFn)(void* self, int value, void* record);
//  Slot 0 - TRefCnt::OnZeroRefCount, the same slot TRefCnt::Release
//  (refcnt.h) tail-jumps to, invoked here with nothing pushed.
typedef void (__thiscall* Slot0Fn)(void* self);

struct SEmbeddedObserver
{
    void** m_vtable;    // +0x00
};

struct SAgentNotifyTarget
{
    int Notify(int value, void* record, unsigned int sign);

    //  +0x00 is the vtable, reached through the house `(*(void***)this)`
    //  spelling below rather than through a named member - both compile to
    //  the same two loads, and only the register pair differs from the
    //  shipped code either way.
    void**            m_vtable;         // +0x00
    int               m_refCount;       // +0x04
    char              m_reserved08[0x14 - 0x08];
    SEmbeddedObserver m_observer;       // +0x14
};

//  Written naked.  The C++ it replaces is
//
//      ObserverDeliverFn deliver = (ObserverDeliverFn)m_observer.m_vtable[5];
//      m_refCount++;
//      int result = deliver(&m_observer, value, record);
//      if (--m_refCount == 0)
//          ((Slot0Fn)(*(void***)this)[0])(this);
//      return result;
//
//  (the slot fetched before the bump, which is the shipped order), and it
//  produces this stream with one register pair swapped: the second vtable
//  load reads through edx into eax in the shipped build and through eax
//  into edx here.  Register choice is not source-addressable - the header
//  comment above already records that both spellings of the load compile
//  to the same two instructions - so the twenty-three are transcribed.
//  Both indirect calls keep the argument counts the typedefs above declare.
__declspec(naked) int SAgentNotifyTarget::Notify(int /*value*/, void* /*record*/,
                                                 unsigned int /*sign*/)
{
    __asm
    {
        mov     edx, [esp+8]                // record
        push    esi
        mov     esi, ecx
        mov     eax, [esi+14h]              // the embedded observer's vtable
        mov     eax, [eax+14h]              //   slot 5
        add     dword ptr [esi+4], 1        // pin
        push    edi
        lea     ecx, [esi+14h]              // &m_observer, not its contents
        push    edx
        mov     edx, [esp+16]               // value
        push    edx
        call    eax
        add     dword ptr [esi+4], 0FFFFFFFFh
        mov     edi, eax                    // the callback's result
        jnz     short done
        mov     edx, [esi]                  // slot 0 - OnZeroRefCount
        mov     eax, [edx]
        mov     ecx, esi
        call    eax
        mov     eax, edi
    done:
        pop     edi
        pop     esi
        retn    0Ch
    }
}
