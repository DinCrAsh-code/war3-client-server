//============================================================================
//  Convenience wrappers over a single global CConfigFile
//  (dword_6FADB0A4, "g_pDefaultConfig"), reached in this call tree only
//  through Storm's own singleton bootstrap (ConstructStormSingletonA reading
//  a game-config value while it builds itself) - not through any
//  widget/pathing code.  See docs/targets/CWidget__MoveForward.md session 6.
//============================================================================
#ifndef GAMEDEFAULTCONFIG_H
#define GAMEDEFAULTCONFIG_H

//  0x6F71A9B0
int __fastcall GetDefaultConfigInt(const char* key, const char* section,
                                   unsigned int index);
//  0x6F71A950 - returns a plain hardware `float`, not a `CFloat`: the target
//  hands the result back through the FPU (`fld`/`retn 4`, no hidden return
//  pointer), unlike CFloat::operator+/- which do use the hidden-pointer
//  convention elsewhere in this codebase.
float __fastcall GetDefaultConfigFloat(const char* key, const char* section,
                                       unsigned int index);

//----------------------------------------------------------------------------
//  A plain hardware-float 3-vector, not a CWorldVec3 (which holds
//  deterministic CFloat members): every store the caller of
//  GetDefaultConfigVec3 makes is a raw `fstp dword ptr`, straight off
//  GetDefaultConfigFloat's own plain-float return, never through CFloat's
//  encode/decode.
//----------------------------------------------------------------------------
struct SRawVec3
{
    float m_x, m_y, m_z;
};

//  0x6F71A9E0
void __fastcall GetDefaultConfigVec3(SRawVec3* out, const char* key,
                                     const char* section, unsigned int index);

//  0x6F72B0B0 - see gamedefaultconfigcolor.cpp.
int* __fastcall GetDefaultConfigColor(int* out, const char* key,
                                      const char* section,
                                      unsigned int baseIndex);

#endif
