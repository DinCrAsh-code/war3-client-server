//============================================================================
//  0x6F4DF470 - CBlpTexture::Create: the same shape as the `.tga` one with
//  three things extra.  The dimensions are shrunk to what the device can
//  take before the decode is submitted, the picture type picks the texture
//  format out of a four-arm switch, and the format is 1 rather than 0.
//
//  The switch's default arm is the constructor's own default picture type
//  (2), which is why a file that never got read still asks for format 0.
//============================================================================
#include "texture.h"
#include "imagefile.h"
#include "renderdevice.h"
#include "renderthunks.h"
#include "refcnt.h"

CBlpTexture* __fastcall CBlpTexture::Create(const char* path, CStatus* status,
                                            TEXTUREOPTIONS options, int arg8)
{
    CBlpFile file;
    CBlpTexture* result = 0;

    if (file.Load(path, arg8) != 0)
    {
        CBlpTexture* tex =
            (CBlpTexture*)SMemAlloc(Storm_506(path) + 0x25, kHTexture, -2, 0);
        if (tex != 0)
        {
            tex->m_refcount    = 0;
            tex->m_image       = 0;
            tex->m_flags       = 0;
            tex->m_vtable      = (void*)&g_vftCBlpTexture;
            tex->m_decoder     = 0;
            tex->m_reserved18  = 0;
            tex->m_reserved1C  = 0;
            tex->m_format      = 1;
            tex->m_path        = tex->m_pathText;
            tex->m_path[0]     = 0;

            if (file.m_header.m_alphaBits == 0)
                tex->m_flags |= 1;

            tex->m_decoder = &file;

            unsigned int width  = file.m_header.m_width;
            unsigned int height = file.m_header.m_height;
            int mipSkip = 0;
            TEXTUREDIMREF widthRef;
            widthRef.m_value = &width;
            TEXTUREMIPREF mipRef;
            mipRef.m_value = &mipSkip;
            ShrinkTextureToDeviceLimit(&height, widthRef, mipRef);

            Storm_501(tex->m_path, path, 0x104);

            int format;
            switch (file.m_header.m_pictureType)
            {
            case 3:  format = 2; break;
            case 4:  format = 1; break;
            case 5:  format = 3; break;
            default: format = 0; break;
            }

            if (SubmitTextureCreate(width, height, format, options, tex,
                                    &BlpTextureDecodeStep, &tex->m_image) != 0)
            {
                tex->m_decoder = 0;
                file.Release();
                result = (CBlpTexture*)((TRefCnt*)tex)->AddRef(kHTexture);
            }
            else
            {
                typedef void (__thiscall *DestroyFn)(CTexture*, int);
                ((DestroyFn)(*(void***)tex)[0])(tex, 1);
            }
        }
    }
    else
    {
        ReportTextureLoadError(status, path, "texture");
    }

    return result;
}
