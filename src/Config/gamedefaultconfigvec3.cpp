//============================================================================
//  0x6F71A9E0 - GetDefaultConfigVec3: three consecutive indexed values
//  (index, index+1, index+2) under the same key/section, read out as x/y/z.
//  See src/gamedefaultconfig.h for GetDefaultConfigFloat, and why this is a
//  separate translation unit from it.
//============================================================================
#include "gamedefaultconfig.h"

//  All three calls are made before any of the three stores, matching the
//  shipped code's own call/call/call-then-store/store/store order - not
//  call/store three times.
void __fastcall GetDefaultConfigVec3(SRawVec3* out, const char* key,
                                     const char* section, unsigned int index)
{
    float x = GetDefaultConfigFloat(key, section, index);
    float y = GetDefaultConfigFloat(key, section, index + 1);
    float z = GetDefaultConfigFloat(key, section, index + 2);

    out->m_x = x;
    out->m_y = y;
    out->m_z = z;
}
