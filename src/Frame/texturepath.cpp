//============================================================================
//  0x6F60E090 - CSimpleTexture::SetTexturePath.
//
//  Three attempts, in the shipped order: the name as given (with ".blp"
//  appended when it has no extension of its own), then the *other* of the
//  ".tga"/".blp" pair, then a one-pixel 0xFF00FF00 texture so that something
//  is always bound.  The answer is 1 unless every attempt failed.
//
//  +0x90 ends up 2 when the texture that loaded has no alpha channel and 0
//  when it has one - that is the `neg`/`sbb`/`and -2`/`add 2` chain reading
//  bit 0 of the texture's +0x10 (Misc/trivialpredicates.cpp's
//  GetFlag0x10Bit0_6F4DE1F0), not a two-valued flag.
//
//  Score capped by the unified EH frame *and* by the /GS stack cookie the
//  shipped function carries around its two 0x104-byte buffers; this build's
//  fixed `/GS- /EHs-c-` emits neither.  See framecore.cpp.
//============================================================================
#include "frame.h"
#include "framethunks.h"

#include "refcnt.h"

int CSimpleTexture::SetTexturePath(const char* path, int flags)
{
    int result = 1;

    if (m_pTexture)
    {
        ((TRefCnt*)m_pTexture)->Release();
        m_pTexture = 0;
    }

    if (path)
    {
        char name[0x104];
        char alternate[0x104];

        int length = (int)Storm_506(path);
        if (path[length - 4] == '.')
            Storm_501(name, path, 0x104);
        else
            Storm_578(name, 0x104, "%s.blp", path);

        CStatus status;

        //  The flag word the texture loader wants: bit 0 of `flags` scaled by
        //  three and shifted into bits 2-3, the result word masked and 0x11
        //  raised.  Written the way the shipped stream forms it - `(a<<1)|a`
        //  and two doublings, not a multiply.
        int one = flags & 1;
        TEXTUREOPTIONS loadFlags;
        loadFlags.m_word =
            (((one << 1) | one) << 2) | (result & ~0x2E) | 0x11;

        if (name[0])
            m_pTexture = LoadTextureFile(name, &status, loadFlags, 0, 1);

        if (!m_pTexture)
        {
            int wasTga = (SStrnICmp(&name[length - 4], ".tga",
                                    0x7FFFFFFF) == 0);

            Storm_501(alternate, name, 0x7FFFFFFF);

            if (length < 5 || name[length - 4] != '.')
                length += 4;

            Storm_501(&alternate[length - 4], wasTga ? ".blp" : ".tga",
                      0x7FFFFFFF);

            m_pTexture = LoadTextureFile(alternate, &status, loadFlags, 0, 1);
            if (!m_pTexture)
            {
                unsigned int magenta = 0xFF00FF00;
                m_pTexture = MakeSolidTexture(magenta);
            }
        }

        if (m_pTexture)
            m_textureMode = GetFlag0x10Bit0_6F4DE1F0(m_pTexture) ? 0 : 2;
        else
            result = 0;
    }

    MarkLayerDirty();
    return result;
}
