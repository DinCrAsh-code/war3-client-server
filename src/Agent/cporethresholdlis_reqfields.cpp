//============================================================================
//  0x6F4A9170 / 0x6F4A9190 - free `__fastcall` helpers reached from
//  0x6F4A9D80 (CPoReThresholdLis::NotifyEndpointAIfTagged,
//  cporethresholdlis_notify.cpp): build a local CFloat by copying one dword
//  straight out of the incoming SHandleRegistrationRequest* (its own
//  +0x10 / +0x14) before handing the address to CPathTrace::RampValueAt.
//
//  Receiver (the CFloat being constructed) in ecx, source request in edx,
//  `retn` with no operand - the free-`__fastcall`-with-receiver-in-ecx
//  shape docs/msvc-vc8-idioms.md's own "A receiver in ecx with a second
//  argument in edx is a free __fastcall" note documents (this compiler
//  rejects `__thiscall` on a free function, and a real thiscall member
//  would take the source on the stack, not in edx - the ABI itself says
//  these are not members). Reads the request's own field as a raw offset
//  against the type's existing opaque forward declaration, same reasoning
//  Agent/cpofglis_slot3.cpp's own header already gives for the same
//  handletable.h/Game/game.h SHandleTable collision.
//============================================================================
#include "cporethresholdlis.h"

CFloat* __fastcall CopyRequestFieldToCFloat0x10(CFloat* dest, const void* request)
{
    dest->m_bits = *(const int*)((const char*)request + 0x10);
    return dest;
}

CFloat* __fastcall CopyRequestFieldToCFloat0x14(CFloat* dest, const void* request)
{
    dest->m_bits = *(const int*)((const char*)request + 0x14);
    return dest;
}
