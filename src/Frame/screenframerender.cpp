//============================================================================
//  0x6F60A6E0 - CScreenFrame::EnsureRenderObject: hand back the screen-space
//  sprite frame this frame draws its cursor through, creating it the first
//  time `create` says so.
//
//  ".\\CScreenFrame.cpp" is the allocation tag and is what names this class;
//  the 0x1B0 bytes it asks Storm for are exactly sizeof(CSpriteFrame).  Both
//  refcount steps are the shipped code's own inlined AddRef and
//  release-if-last, and both +0xB4 upcasts are written as raw casts because
//  a `static_cast` off a possibly-null pointer would emit a null test the
//  shipped code does not have.
//============================================================================
#include "frame.h"
#include "framethunks.h"
#include "cursorframe.h"

static const char kScreenFrameFile[] = ".\\CScreenFrame.cpp";

void* CScreenFrame::EnsureRenderObject(int create)
{
    if (m_pRenderObject == 0 && create != 0)
    {
        void* block = SMemAlloc(0x1B0, kScreenFrameFile, 0x8B, 0);

        void* sprite;
        if (block != 0)
            sprite = new (block) CCursorFrame(this, 0, 0);
        else
            sprite = 0;

        if (sprite != 0)
            *(int*)((char*)sprite + 4) += 1;

        void* old = m_pRenderObject;
        if (old != 0)
        {
            *(int*)((char*)old + 4) += -1;
            if (*(int*)((char*)old + 4) == 0)
                ((LayerReleaseFn)(*(void***)old)[0])(old);
        }

        m_pRenderObject = sprite;

        ((CLayoutFrame*)((char*)sprite + 0xB4))->SetAllPoints(
            (CLayoutFrame*)((char*)this + 0xB4), 1);
    }

    return m_pRenderObject;
}
