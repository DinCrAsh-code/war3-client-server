//============================================================================
//  0x6F4F1F90 - CRibbonEmitter::SetTexture: drop the old reference, take one
//  on the new texture, and cache the texture's own +0x0C beside it.
//
//  0x6F4F2140 - CRibbonEmitter::GetTextureId, the two-instruction reader
//  0x6F502950 compares against.  IDA gave it the name
//  `?GetNumberOfBoundContexts@SchedulerBase@details@Concurrency@@IBEKXZ_5`,
//  which is identical-code-folding onto a ConcRT method, not what it is: the
//  body is `mov eax,[ecx+190h]` / `retn` and its one caller in this closure
//  compares the answer with a texture id.
//============================================================================
#include "material.h"
#include "refcnt.h"

void CRibbonEmitter::SetTexture(CTexture* texture)
{
    if (m_texture != 0)
        ((TRefCnt*)m_texture)->Release();

    m_texture = (CTexture*)((TRefCnt*)texture)->AddRefOrNull();
    m_alphaMode = GetField0xC_6F4DE1E0(texture);
}

unsigned int CRibbonEmitter::GetTextureId() const
{
    return m_textureId;
}
