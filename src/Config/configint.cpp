//============================================================================
//  0x6F009F90 - a named configuration value as a plain int.  The int sibling
//  of GetConfigFloat (0x6F009EE0, configfloat.cpp), with the same three
//  sources consulted in the same order and the same inline two-pointer test
//  in front of CMiscCustom::HasValue.
//
//  Unlike GetConfigFloat, this one has *two* epilogues: the shipped code
//  returns straight out of the override branch instead of falling through to
//  a shared exit, so the override arm is written as an early return here.
//============================================================================
#include "CMiscCustom.h"
#include "gamecontext.h"

int __fastcall GetConfigInt(const char* section, const char* key, int index)
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
