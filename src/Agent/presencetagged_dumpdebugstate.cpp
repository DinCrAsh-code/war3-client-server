//============================================================================
//  0x6F479C30 - NTempest::CPresenceTagged::DumpDebugState, vtable slot 16.
//  Two calls into the already-reconstructed variadic sink
//  (Storm/textsink.h's `LogFormatted`, 0x6F4B73F0):
//
//    - LogFormatted(sink, "") - the fmt argument is `push offset ValueName`
//      (0x6F87529C), the shared all-zero-bytes global GameUI/
//      gameuiplayername.cpp's own header comment already reads out of the
//      binary and spells as a plain `""` literal rather than a DATA name,
//      for the same "a shared constant several modules pass" reason;
//    - LogFormatted(sink, "[PrTg] t[%u] b[%u]", this->m_tagT, this->m_tagB) -
//      this object's own tag pair at +0x14/+0x18, read directly off `this`
//      (unlike the similarly-shaped calls elsewhere in this codebase that
//      read a birth-tag/presence-tag pair off g_unk6FAB778C instead).
//============================================================================
#include "presence.h"
#include "textsink.h"

namespace NTempest {

void CPresenceTagged::DumpDebugState(void* sink)
{
    LogFormatted(sink, "");
    LogFormatted(sink, "[PrTg] t[%u] b[%u]", m_tagT, m_tagB);
}

}  // namespace NTempest
