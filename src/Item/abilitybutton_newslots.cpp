//============================================================================
//  Twelve of CAbilityButton's sixteen brand-new vtable slots (194-205, 208)
//  - plain constant answers or one-line tail-forwards to another slot in
//  this same group, `this` only (`retn 0`/`retn 4`).  See abilitybutton.h
//  for each slot's own index/offset/address.  Slots 206/207/209 are
//  elsewhere: 206/207 (abilitybutton_bigslots.cpp, real bodies left as
//  thunks this session) and 209 (a real empty body, inline in the header).
//============================================================================
#include "abilitybutton.h"

//  194 / +0x308 (0x6F068830).
unsigned int CAbilityButton::Method_0x308()
{
    return g_defaultAbilityIconId;
}

//  195 / +0x30C (0x6F068840).
int CAbilityButton::Method_0x30C()
{
    return 1;
}

//  196 / +0x310 (0x6F068850) - tail-forwards to Method_0x308 through this
//  instance's own vtable (`mov eax,[ecx]; mov edx,[eax+308h]; jmp edx`),
//  the same "call through the vtable rather than by name" reasoning
//  attachmentcounters.cpp's own slot-0xCC dispatch already documents.
typedef unsigned int (__thiscall *VtableSlot308Fn)(void*);

unsigned int CAbilityButton::Method_0x310()
{
    return ((VtableSlot308Fn)(*(void***)this)[0x308 / 4])(this);
}

//  197 / +0x314 (0x6F068860).
int CAbilityButton::Method_0x314()
{
    return 0;
}

//  198 / +0x318 (0x6F068870).
unsigned int CAbilityButton::Method_0x318()
{
    return g_defaultAbilityIconId;
}

//  199 / +0x31C (0x6F068880).
int CAbilityButton::Method_0x31C()
{
    return 1;
}

//  200 / +0x320 (0x6F068890).
unsigned int CAbilityButton::Method_0x320()
{
    return g_defaultAbilityIconId;
}

//  201 / +0x324 (0x6F0688A0).
int CAbilityButton::Method_0x324()
{
    return 0;
}

//  202 / +0x328 (0x6F0688B0).
unsigned int CAbilityButton::Method_0x328()
{
    return g_defaultAbilityIconId;
}

//  203 / +0x32C (0x6F0688C0).
int CAbilityButton::Method_0x32C()
{
    return 0;
}

//  204 / +0x330 (0x6F022980) - one stack argument, never forwarded (no
//  push before the shipped code's own `call edx`): only cleaned off the
//  stack (`retn 4`).
void CAbilityButton::Method_0x330(void* /*rec*/)
{
    Method_0x338();
}

//  205 / +0x334 (0x6F0688D0) - same shape as 204 above.
void CAbilityButton::Method_0x334(void* /*rec*/)
{
    Method_0x33C();
}

//  208 / +0x340 (0x6F0688E0) - tail-forwards to Method_0x318, same idiom
//  as Method_0x310 above.
typedef unsigned int (__thiscall *VtableSlot318Fn)(void*);

unsigned int CAbilityButton::Method_0x340()
{
    return ((VtableSlot318Fn)(*(void***)this)[0x318 / 4])(this);
}
