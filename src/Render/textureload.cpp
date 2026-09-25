//============================================================================
//  0x6F4E3BE0 - the texture module's public entry.
//
//  With `reload` clear this is a cache lookup first and a decode only on a
//  miss, and the decode's result goes back into the cache; with it set the
//  name is marked *uncachable* in the file cache instead and the result is
//  never cached.  The extension classifier picks the decoder, and a name
//  whose extension is neither `.tga` nor `.blp` is reported through the
//  CStatus - through its vtable slot 3 with the shipped typo, "texure",
//  intact.
//============================================================================
#include "texture.h"
#include "cstatus.h"
#include "filecache.h"

CTexture* __fastcall LoadTextureFile(const char* path, CStatus* status,
                                     TEXTUREOPTIONS options, int reload,
                                     int arg8)
{
    if (reload == 0)
    {
        CTexture* cached = TextureCacheFind(path, options);
        if (cached != 0)
            return cached;
    }
    else
    {
        MarkFileUncachable(path);
    }

    CTexture* texture;
    switch (ClassifyTextureExtension(path))
    {
    case 0:
        texture = CTgaTexture::Create(path, status, options);
        if (texture == 0)
            return 0;
        break;

    case 1:
        texture = CBlpTexture::Create(path, status, options, arg8);
        if (texture == 0)
            return 0;
        break;

    default:
        if (status != 0)
        {
            typedef void (__cdecl *StatusReportFn)(CStatus*, int,
                                                   const char*, ...);
            ((StatusReportFn)(*(void***)status)[0x0C / 4])(
                status, 3,
                "Error loading texure file \"%s\": unsupported file type\n",
                path);
        }
        return 0;
    }

    if (reload == 0)
        TextureCacheAdd(path, options, texture);

    return texture;
}
