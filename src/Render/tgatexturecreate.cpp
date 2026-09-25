//============================================================================
//  0x6F4DF230 - CTgaTexture::Create: open the file, allocate the object with
//  its own path text behind it, and hand the decode to the render device.
//
//  The allocation is `SStrLen(path) + 0x25`: 0x24 of members plus the name
//  and its NUL, with m_path pointing at the tail.  The alpha test is TGA's
//  own - bits 0-3 of the image descriptor are the alpha bit count, and zero
//  of them sets the texture's flag bit 0.
//
//  Both failure exits fall into the same block, and only the *open* failure
//  reports: a failed allocation is silent in the shipped code.
//============================================================================
#include "texture.h"
#include "imagefile.h"
#include "renderdevice.h"
#include "renderthunks.h"
#include "refcnt.h"

CTgaTexture* __fastcall CTgaTexture::Create(const char* path, CStatus* status,
                                            TEXTUREOPTIONS options)
{
    CTgaFile file;
    CTgaTexture* result = 0;

    if (file.Open(path) != 0)
    {
        CTgaTexture* tex =
            (CTgaTexture*)SMemAlloc(Storm_506(path) + 0x25, kHTexture, -2, 0);
        if (tex != 0)
        {
            tex->m_refcount    = 0;
            tex->m_image       = 0;
            tex->m_flags       = 0;
            tex->m_vtable      = (void*)&g_vftCTgaTexture;
            tex->m_decoder     = 0;
            tex->m_reserved18  = 0;
            tex->m_reserved1C  = 0;
            tex->m_format      = 0;
            tex->m_path        = tex->m_pathText;
            tex->m_path[0]     = 0;

            if ((file.m_header.m_descriptor & 0x0F) == 0)
                tex->m_flags |= 1;

            Storm_501(tex->m_path, path, 0x104);

            tex->m_decoder = &file;

            if (SubmitTextureCreate(file.m_header.m_width,
                                    file.m_header.m_height, 0, options, tex,
                                    &TgaTextureDecodeStep, &tex->m_image) != 0)
            {
                tex->m_decoder = 0;
                file.Close();
                result = (CTgaTexture*)((TRefCnt*)tex)->AddRef(kHTexture);
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
