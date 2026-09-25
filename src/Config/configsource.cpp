//============================================================================
//  0x6F6F3410, 0x6F6F34F0 - CConfigSource, one file's worth of configuration
//  seen through the interface CMiscCustom uses: "have you got this key" and
//  "give me its value".
//============================================================================
#include "config.h"

//----------------------------------------------------------------------------
//  0x6F6F3410 - presence is "the first value exists": index 0 of the key.
//----------------------------------------------------------------------------
int CConfigSource::HasValue(const char* section, const char* key) const
{
    return m_pFile->GetValue(section, key, 0) != 0;
}

//----------------------------------------------------------------------------
//  0x6F6F34F0 - the value as a float, zero when the key is missing.
//
//  `value` is the return value, so the named-return optimisation makes the
//  address handed to CConfigFile::GetFloat the caller's own buffer - which is
//  why the shipped code stores the zero into the caller's buffer before the
//  call and never copies anything afterwards.  CConfigFile::GetFloat clears it
//  again; both stores are in the shipped code.
//----------------------------------------------------------------------------
CFloat CConfigSource::GetFloat(const char* section, const char* key,
                               int index) const
{
    CFloat value = g_CFloatZero;
    m_pFile->GetFloat(section, key, &value, index);
    return value;
}

//----------------------------------------------------------------------------
//  0x6F6F34C0 - see config.h.  The local is a plain `float`, not a CFloat:
//  the shipped code zeroes it with `fldz`/`fstp` rather than the `mov ...,0`
//  a CFloat's own default constructor emits, and reads it back with `fld`
//  straight into the return.  GetFloatValue writes a hardware float there,
//  so the cast is only telling it which out-parameter type it wants.
//----------------------------------------------------------------------------
float CConfigSource::GetFloatRaw(const char* key, const char* section,
                                 unsigned int index) const
{
    float value = 0.0f;
    m_pFile->GetFloatValue(key, section, reinterpret_cast<CFloat*>(&value), index);
    return value;
}

//----------------------------------------------------------------------------
//  0x6F6F3570 - the string sibling: straight into CConfigFile::GetValue with
//  the arguments unrenamed.  Its own body rather than a redirect now; batch
//  G of the JASS_PauseGame closure is what reaches it.
//----------------------------------------------------------------------------
const char* CConfigSource::GetString(const char* section, const char* key,
                                     int index) const
{
    return m_pFile->GetValue(section, key, index);
}
