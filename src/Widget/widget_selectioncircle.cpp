//============================================================================
//  0x6F2AC920 - CWidget's vtable slot 66 (+0x108) and 0x6F26BB70, the
//  colour-slot refresh its slot 16 (+0x40) calls.  Two unrelated leaves
//  that only share this file because neither calls the other and both make
//  real out-of-line calls that need to survive.
//============================================================================
#include "widget.h"
#include "footprinttype.h"   // GetFootprintScaleFactor (0x6F32D070)

//  0x6F009E30 - see configgates.cpp.  A second, hardware-float copy of
//  GetConfigFloat's body (configfloat.cpp) at its own address.
float __fastcall GetConfigFloatHW(const char* section, const char* key,
                                  int index);

//----------------------------------------------------------------------------
//  0x6F2AC920 - slot 66.  The doubling is spelled `* 2.0f` inside the same
//  expression, which MSVC folds to `fadd st, st` with the product still on
//  the FPU stack.  Written as a named local plus `r + r` instead, the local
//  gets spilled and reloaded *before* the add and two instructions appear
//  that the shipped code does not have.
//----------------------------------------------------------------------------
float CWidget::Method_0x108()
{
    float scale = GetFootprintScaleFactor(m_footprintType);
    return (GetConfigFloatHW("SelectionCircle", "ScaleFactor", 0) * scale) * 2.0f;
}

//----------------------------------------------------------------------------
//  0x6F26BB70 - not a vtable slot of its own.  The owning player comes from
//  slot 25 (+0x64, CAgentWar3::Method_0x64 - always -1 unless a derived
//  class overrides it), and the colour slot is m_reserved24's own low byte
//  read straight back out and written straight back in, which is what makes
//  this a *refresh* rather than a setter: only the sprite side of
//  StoreOwningPlayer actually changes anything.
//----------------------------------------------------------------------------
typedef unsigned int (__thiscall *Slot0x64Fn)(void* self);

void CWar3Image::RefreshOwningPlayerColor()
{
    StoreOwningPlayer(((Slot0x64Fn)(*(void***)this)[0x64 / 4])(this),
                      (unsigned char)m_reserved24);
}
