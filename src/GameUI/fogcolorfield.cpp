//============================================================================
//  0x6F333B60 - `retn 0`, thiscall.  `this` is itself the colour output
//  buffer (Agent/relationconstant.h's own SRelationColorConstant /
//  Config/configcolor.h's own GetConfigColor `out` convention: a hidden
//  return pointer and a packed colour respectively, both spelled as
//  writing through `this`).
//
//  playerIndex < 12 (a real player slot): fill `this` with
//  GetPlayerRelationConstant(localSlot, playerIndex) - localSlot read
//  directly out of the world object at +0x28
//  (dword_6FAB65F4/g_unk6FAB65F4) rather than through
//  IndexedArrayHolder_6F3A1650::At() the way Agent/relationconstant.cpp's
//  own two lookups are, because this call site already has the value at a
//  fixed field rather than a slot to look up.
//
//  playerIndex >= 12 (not a real player slot): fill `this` with the
//  config-file default instead (GetConfigColor, "FogOfWar" /
//  "FogColorResource").
//
//  Both arms return `this`, the fluent chain both GetConfigColor's own
//  call sites and this file's own caller (SSelectionVisualDetail::
//  ReleasePreviousTarget, GameUI/selectionvisualdetail.cpp - not
//  reconstructed) both read.
//============================================================================
#include "relationconstant.h"

extern void* g_unk6FAB65F4;

//  Config/configcolor.cpp - no header of its own; declared here to match
//  its mangled signature exactly.
unsigned int* __fastcall GetConfigColor(unsigned int* out, const char* section,
                                        const char* key, int index);

SRelationColorConstant* __fastcall SetFogColorForPlayerOrDefault(
    SRelationColorConstant* self, int playerIndex)
{
    if (playerIndex < 0x0C)
    {
        unsigned short localSlot = *(unsigned short*)((char*)g_unk6FAB65F4 + 0x28);
        *self = GetPlayerRelationConstant(localSlot, playerIndex);
        return self;
    }

    GetConfigColor((unsigned int*)self, "FogOfWar", "FogColorResource", 0);
    return self;
}
