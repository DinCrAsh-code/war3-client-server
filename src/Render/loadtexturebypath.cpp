//============================================================================
//  0x6F732360 - load a texture by path, trying each extension in turn.
//
//  The name is copied, its own extension stripped (the scan walks back from
//  the end and stops at the first `.` or `\`), and then `.tga` and `.blp`
//  are appended in turn until one loads.  The two extensions are one packed
//  block of five-byte entries in the image, which is what the `add edi, 5`
//  and the compare against its end are.
//
//  The default load options are the tick count with three bits cleared and
//  three set - the shipped code's own `(ticks & ~0x31) | 0x0E` - which is
//  what makes the caller's optional override a pointer and not a value.  The
//  microsecond counter is only credited on the load that succeeded.
//============================================================================
#include "texture.h"
#include "cstatus.h"

//  0x6F71D070 - Storm/stormtickcount.cpp, spelled as that file defines it.
int GetStormTickCount();

static const char s_extensions[] = ".tga\0.blp";

CTexture* __fastcall LoadTextureByPath(const char* path,
                                       const TEXTUREOPTIONS* override,
                                       int reload, int arg8)
{
    unsigned int start = (unsigned int)GetStormTickCount();

    CStatus status;
    char name[0x104];

    Storm_501(name, path, 0x7FFFFFFF);

    int at = (int)Storm_506(name) - 1;
    while (at >= 0)
    {
        char c = name[at];
        if (c == '.')
        {
            name[at] = 0;
            break;
        }
        if (c == '\\')
            break;
        at--;
    }

    at = (int)Storm_506(name);

    TEXTUREOPTIONS options;
    options.m_word = (start & 0xFFFFFFCE) | 0x0E;
    if (override != 0)
        options = *override;

    char* tail = name + at;

    CTexture* texture = 0;
    for (const char* ext = s_extensions;
         ext < s_extensions + sizeof(s_extensions) - 1; ext += 5)
    {
        Storm_501(tail, ext, 0x7FFFFFFF);

        texture = LoadTextureFile(name, &status, options, reload, arg8);
        if (texture != 0)
        {
            g_textureLoadMicros += (unsigned int)GetStormTickCount() - start;
            break;
        }
    }

    return texture;
}
