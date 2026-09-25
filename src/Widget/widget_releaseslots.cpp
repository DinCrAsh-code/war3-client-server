//============================================================================
//  0x6F2AB8C0 and 0x6F26B7D0 - the two teardown halves CWidget::Deactivate
//  (widget_deactivate.cpp) calls out of line.  Two separate addresses in the
//  shipped code, in two different modules, but neither calls the other, so
//  one translation unit reproduces both without eating a call.
//============================================================================
#include "widget.h"
#include "refcnt.h"

//  0x6F00F790 - hand a handle back to whatever pool issued it.
//  Reconstructed in stormattachmentgates.cpp; declared here so this file
//  reaches it with a real call.
void __fastcall ReleaseIssuedHandle(unsigned int handle);

//----------------------------------------------------------------------------
//  0x6F2AB8C0.  The test is signed (`jl`), not "!= -1": any negative value
//  counts as "nothing to release", which is consistent with m_reserved38
//  being an index-or-(-1) rather than an opaque word.
//----------------------------------------------------------------------------
void CWidget::ReleaseHandle38()
{
    if ((int)m_reserved38 >= 0)
    {
        ReleaseIssuedHandle(m_reserved38);
        m_reserved38 = (unsigned int)-1;
    }
}

//----------------------------------------------------------------------------
//  0x6F26B7D0.  Drop one reference and clear the field unconditionally -
//  the store happens even when the pointer was already null, which is why
//  it sits outside the `if` here too.
//----------------------------------------------------------------------------
void CWar3Image::Deactivate()
{
    if (m_pSprite != 0)
        ((TRefCnt*)m_pSprite)->Release();
    m_pSprite = 0;
}
