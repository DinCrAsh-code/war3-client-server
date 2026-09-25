//============================================================================
//  0x6F009D80 - a second, separately-compiled body with the identical
//  logic to GetConfigInt (0x6F009F90, configint.cpp): same
//  GetThreadLocalSlot/GetMiscCustom prologue, same CMiscCustom::HasValue
//  gate, same CConfigFile::HasKey/GetIntValue two-source fallback, same
//  `retn 4`.  Two genuinely different addresses in the shipped binary
//  (0x210 bytes apart, not an alias) - reached as a depth-1 callee off
//  slot 16/slot 107 in CUnit's own vtable closure
//  (cunit_vtable_closure_worklist.json, agent-cunit-batch-3).  Given no
//  other name for what distinguishes this call site's use from
//  GetConfigInt's own, this repo names it for what it is rather than
//  guessing at a caller-specific reason.
//============================================================================
#include "CMiscCustom.h"
#include "gamecontext.h"

int __fastcall GetConfigIntDuplicate_6F009D80(const char* section,
                                              const char* key, int index)
{
    CGameDataContext* context =
        (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    CMiscCustom* misc = GetMiscCustom();

    if ((misc->m_pPrimary != 0 || misc->m_pSecondary != 0) &&
        misc->HasValue(section, key))
        return misc->GetInt(section, key, index);

    int value = 0;
    if (context->m_pConfigOverride != 0 &&
        context->m_pConfigOverride->HasKey(section, key))
    {
        context->m_pConfigOverride->GetIntValue(section, key, &value, index);
        return value;
    }
    context->m_pConfig->GetIntValue(section, key, &value, index);
    return value;
}
