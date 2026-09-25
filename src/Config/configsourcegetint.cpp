//============================================================================
//  0x6F6F3490 - CConfigSource::GetInt.
//
//  CConfigSource::GetFloatRaw's int sibling, reached by CMiscCustom::GetInt
//  (CMiscCustom.cpp).  A thunk until now, on the grounds that the repo had
//  no dump for it; asm/sub_6F29E930_... has carried one all along.
//
//  Zero a local, hand its address to CConfigFile::GetIntValue, return what
//  is in it - the same three-line shape GetFloat and GetFloatRaw already
//  have in configsource.cpp, and the reason the discarded `int` return of
//  GetIntValue never appears: the value comes back through the out
//  parameter, not through eax.
//
//  **`key` first, `section` second**, unlike GetFloat next door.  That is
//  not a naming choice here: the shipped body puts its own first stack
//  argument in *edx* and pushes its second, and config.h's own note on
//  0x6F4D1F80/0x6F4D2000 records that those two members of CConfigFile are
//  the ones that take the key ahead of the section while every other member
//  of the class takes the section first.  GetFloatRaw (0x6F6F34C0) forwards
//  the same pair the same way and is spelled the same.  CMiscCustom::GetInt
//  passes its own two through positionally, so its parameter names carry
//  the same inversion; that is the shipped code's, not this file's.
//
//  Own translation unit, kept from when this was a naked body: the caller
//  must reach it with a real `call`, so it cannot be inlined into
//  configsource.cpp beside its float siblings.
//============================================================================
#include "config.h"

int CConfigSource::GetInt(const char* key, const char* section, int index) const
{
    int value = 0;
    m_pFile->GetIntValue(key, section, &value, index);
    return value;
}
