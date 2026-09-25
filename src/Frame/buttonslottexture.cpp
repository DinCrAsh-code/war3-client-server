//============================================================================
//  0x6F603220 - CSimpleButton::SetSlotTexture: give one of the button's three
//  texture slots a new art path, making the CSimpleTexture the first time.
//
//  The allocation is ".\\CSimpleButton.cpp" line 0xE8, 0x80 bytes - which is
//  not sizeof(CSimpleTexture) (0xE8) but the pool block the shipped code asks
//  for; the constructor is run into it regardless, exactly as the shipped
//  stream does.
//
//  When the path fails to load on a *newly made* texture the texture is
//  thrown away again through its own slot 0 and the answer is 0; when it
//  fails on one that already existed the shipped code still answers 1, which
//  is the early-out at the top.
//
//  Score capped by the unified EH frame; see framecore.cpp.
//============================================================================
#include "frame.h"
#include "framethunks.h"


int CSimpleButton::SetSlotTexture(int slot, const char* path)
{
    CSimpleTexture* texture = m_slots[slot];
    if (texture)
    {
        texture->SetTexturePath(path, 0);
        return 1;
    }

    CSimpleTexture* made = (CSimpleTexture*)SMemAlloc(
        0x80, ".\\CSimpleButton.cpp", 0xE8, 0);
    if (made)
        made = new (made) CSimpleTexture(0, 2, 1);

    if (made->SetTexturePath(path, 0))
    {
        made->SetAllPoints(this, 1);
        AttachSlotTexture(slot, made);
        return 1;
    }

    if (made)
    {
        typedef void (__thiscall *DestroyFn)(void*, int);
        ((DestroyFn)(*(void***)made)[8 / 4])(made, 1);
    }
    return 0;
}
