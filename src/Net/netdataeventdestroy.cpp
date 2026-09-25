//============================================================================
//  0x6F548DF0 - the real, unhooked ~CNetData::CNetEvent().  See
//  netdataeventqueue.h for the file-split rationale.
//
//  Dispatches one of two already-real "release a payload buffer"
//  notifications depending on the node's own type byte at +0x14 (both
//  loads happen unconditionally before the branch, matching the shipped
//  code hoisting them above the `cmp`): under 0x40 goes to
//  NotifyTurnPayload_6F650DB0(buffer, size); 0x40..0xFE goes to
//  NotifyTurnPayloadAlt_6F686280(buffer); 0xFF calls neither. Then unlinks
//  itself from whatever list it is still on (the same TSLink<T>::Unlink()
//  shape as everywhere else in this cluster - CNetEvent's own TSLink sits
//  at its own offset 0, i.e. this object's own `this`).
//
//  SEH-framed (__CxxFrameHandler3 / g_securityCookie) the same way
//  SNetSessionInfo's own ctor/dtor are (netdatactor.cpp) - docs/msvc-vc8-
//  idioms.md's "An `__except_handler4`-shaped frame this toolchain cannot
//  reproduce" caps the achievable score regardless of how faithful the rest
//  is; the buffer/size fields at +0x08/+0x10 and the type byte at +0x14 are
//  as far into CNetEvent's own ~40-subclass layout as this closure needs to
//  go.
//============================================================================
#include "netdataeventqueue.h"

extern void __fastcall NotifyTurnPayload_6F650DB0(void* self, void* size);
extern void __fastcall NotifyTurnPayloadAlt_6F686280(void* self, void* unused);

void __fastcall DestroyNetEvent(void* self)
{
    char* p = (char*)self;

    unsigned char type   = *(unsigned char*)(p + 0x14);
    void*         buffer = *(void**)(p + 8);
    void*         size   = *(void**)(p + 0x10);

    if (type < 0x40)
        NotifyTurnPayload_6F650DB0(buffer, size);
    else if (type < 0xFF)
        NotifyTurnPayloadAlt_6F686280(buffer, 0);

    ((TSLink<CNetEventNode>*)self)->Unlink();
}
