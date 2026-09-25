//============================================================================
//  0x6F71A950/0x6F71A9B0 - see src/gamedefaultconfig.h.
//
//  Both are thin: default the return value, call the matching CConfigFile
//  member on the one global file, and hand back the value alone - the
//  found/not-found flag GetIntValue/GetFloatValue return is discarded here.
//  `key` arrives in the wrapper's own `ecx` and `section` in `edx` (the
//  opposite of GetConfigFloat's `(section, key)` order in
//  src/configfloat.cpp) because both forward straight into
//  GetIntValue/GetFloatValue's own (key, section) order without renaming
//  either register.
//
//  Kept in their own translation unit, apart from GetDefaultConfigVec3 (see
//  src/gamedefaultconfigvec3.cpp), because that caller's own build inlines
//  both flat otherwise - the shipped code calls them, so it has to be a call
//  here too (CLAUDE.md: a TU boundary, not `__declspec(noinline)`).
//============================================================================
#include "config.h"
#include "gamedefaultconfig.h"

extern CConfigFile* g_pDefaultConfig;  // dword_6FADB0A4

//  0x6F71A9B0 - `value` is zeroed here *and* inside GetIntValue itself; the
//  shipped code pays that redundant second store too (`mov [esp+...], 0`
//  right before the call, on top of GetIntValue's own), so it is kept
//  rather than relied on as dead.
int __fastcall GetDefaultConfigInt(const char* key, const char* section,
                                   unsigned int index)
{
    int value = 0;
    g_pDefaultConfig->GetIntValue(key, section, &value, index);
    return value;
}

//  0x6F71A950 - `CFloat` is still the type GetFloatValue's out-parameter
//  needs (the encoded bit pattern lives in `.m_bits`); the raw bits are
//  reinterpreted as `float` only at the very end, to pick the plain-float
//  return path GetDefaultConfigFloat's own target disassembly shows.
float __fastcall GetDefaultConfigFloat(const char* key, const char* section,
                                       unsigned int index)
{
    CFloat value;
    g_pDefaultConfig->GetFloatValue(key, section, &value, index);
    return *reinterpret_cast<float*>(&value.m_bits);
}
