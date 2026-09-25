//============================================================================
//  0x6F62D3D0 - `jmp Event::CloseForward`. See event.h for why this is a
//  free function rather than a second Event member.
//============================================================================
#include "event.h"

void __fastcall Event_CloseForward2(Event* self)
{
    self->CloseForward();
}
