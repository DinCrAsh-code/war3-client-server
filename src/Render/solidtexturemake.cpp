//============================================================================
//  0x6F4E3C80 - a one-pixel texture of one colour, cached on the colour.
//
//  `__stdcall` with one stack argument: the shipped call site pushes the
//  dword and sets no register, and this body takes its address straight away
//  (`lea eax, [esp+20h+arg_0]`) to hand to the cache, which is what makes it
//  a stack parameter rather than a `__fastcall` one.
//
//  CTexTexture is 0x14 bytes - CTexture and nothing more - which is what
//  says where the base class ends.
//============================================================================
#include "texture.h"
#include "renderdevice.h"
#include "renderthunks.h"
#include "refcnt.h"

CTexture* __stdcall MakeSolidTexture(unsigned int color)
{
    CTexture* cached = SolidTextureCacheFind(&color);
    if (cached != 0)
        return cached;

    CTexTexture* tex = (CTexTexture*)SMemAlloc(0x14, kHTexture, -2, 0);
    if (tex == 0)
        return 0;

    TEXTUREIMAGE image;

    tex->m_refcount = 0;
    tex->m_image    = 0;
    tex->m_flags    = 0;
    tex->m_vtable   = (void*)&g_vftCTexTexture;
    tex->m_format   = 2;

    TEXTURECOLOR argb;
    argb.m_argb = color;

    if (CreateSolidTextureImage(&image, argb) == 0)
    {
        typedef void (__thiscall *DestroyFn)(CTexture*, int);
        ((DestroyFn)(*(void***)tex)[0])(tex, 1);
        return 0;
    }

    tex->m_image = (void*)image.m_result[0];

    CTexture* result = (CTexture*)((TRefCnt*)tex)->AddRef(kHTexture);
    CTEXTUREREF ref;
    ref.m_texture = result;
    SolidTextureCacheAdd(&color, ref);
    return result;
}
