//============================================================================
//  0x6F009EE0 - a named configuration value as a deterministic float, with the
//  map's overrides taking precedence over the game's own data files.
//
//  Three places can answer: the map's CMiscCustom (either of its two sources),
//  the game context's override file, and the game context's base file.  The
//  first two are only consulted if they actually carry the key, so a missing
//  key falls all the way through to the base file, and a key that is in no
//  file at all comes back as zero.
//============================================================================
#include "CMiscCustom.h"
#include "gamecontext.h"

CFloat __fastcall GetConfigFloat(const char* section, const char* key,
                                 int index)
{
    CGameDataContext* context =
        (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    CMiscCustom* misc = GetMiscCustom();

    //  The `||` is not redundant with HasValue(): HasValue() is a call, and the
    //  shipped code tests both source pointers inline first so that the common
    //  case - a map with no overrides at all - costs two loads and no call.
    if ((misc->m_pPrimary != 0 || misc->m_pSecondary != 0) &&
        misc->HasValue(section, key))
        return misc->GetFloat(section, key, index);

    CFloat value = g_CFloatZero;
    CConfigFile* file =
        (context->m_pConfigOverride != 0 &&
         context->m_pConfigOverride->HasKey(section, key))
            ? context->m_pConfigOverride
            : context->m_pConfig;
    file->GetFloat(section, key, &value, index);
    return value;
}
