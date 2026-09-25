//============================================================================
//  0x6F4269B0 - DispatchEventToObserver: hand one event record to one
//  observer through its own vtable, and say the walk should carry on.
//
//  Six instructions, all of them read off the shipped bytes
//  (`ida_query bytes 0x6F4269B0 16`):
//
//      8B 01           mov eax,[ecx]
//      52              push edx
//      8B 50 0C        mov edx,[eax+0Ch]
//      FF D2           call edx
//      B8 01 00 00 00  mov eax,1
//      C3              retn
//
//  `retn 0` with both arguments in registers is __fastcall, and the
//  pushed edx is the slot's own single stack argument - so the slot is
//  `retn 4` and the visitor cleans nothing.
//
//  The constant 1 is the "keep going" answer every visitor of this shape
//  in the binary returns; nothing in this call tree shows a visitor that
//  returns 0, so no name is invented for the other value.
//
//  Its address is taken - CAgentRegistrySingleton::RebuildDayNightListeners
//  (agentdaynight.cpp) passes it to BuildAndRegisterEventBinding - so it
//  cannot be `static`.
//============================================================================

//  Slot 3 (+0x0C) on whatever the walk hands over.  A hand-written
//  dispatch because nothing here identifies the class; the argument count
//  is the one the shipped `push edx` fixes.
typedef void (__thiscall *ObserverEventSlotFn)(void*, void*);

int __fastcall DispatchEventToObserver(void* observer, void* event)
{
    ((ObserverEventSlotFn)(*(void***)observer)[0x0C / 4])(observer, event);
    return 1;
}
