//============================================================================
//  0x6F502950 - and over the ribbon emitters, which are pointers rather than
//  an inline array and which do carry an id of their own.
//
//  The shipped build takes the model in ebx, as its particle twin does.
//============================================================================
#include "material.h"

void CModelInstance::ReplaceRibbonTexture(int id, CTexture* texture)
{
    for (unsigned int i = 0; i < m_ribbonCount; i++)
    {
        CRibbonEmitter* ribbon = (CRibbonEmitter*)m_ribbons[i];
        if ((int)ribbon->GetTextureId() == id)
            ((CRibbonEmitter*)m_ribbons[i])->SetTexture(texture);
    }
}
