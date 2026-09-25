//============================================================================
//  0x6F60D630 - CSimpleFontString::Rebuild: lay the current text out into a
//  new text block, optionally tinting it afterwards.
//
//  Three things here are the shipped code's shape and not a tidier
//  spelling:
//
//  * with no rect of its own to work from the function falls back on the
//    layout frame's own +0x44 rect, and refuses outright when +0x54 is
//    clear;
//  * the "does this string already carry a colour code" scan sets the
//    0x400 flag only when it finds *none*, and leaves via the same forward
//    branch when it does - which is the `goto` below;
//  * the tint's alpha is the text colour's own top byte scaled by the
//    parent frame's, divided by 255 with the signed magic multiply.
//============================================================================
#include "frame.h"
#include "framethunks.h"
#include "refcnt.h"

void CSimpleFontString::Rebuild(const float* rect)
{
    const float* box = rect;

    if (box == 0)
    {
        if (m_54 == 0)
            return;

        box = m_rect;
    }

    if (m_pRendered != 0)
    {
        ((TRefCnt*)m_pRendered)->Release();
        m_pRendered = 0;
    }

    const char* text = m_pText;

    if (text != 0 && *text != 0)
    {
        float y = m_B8 * m_scale + box[1];
        float x = m_BC * m_scale + box[0];

        unsigned int flags = m_style;

        float pos[3];
        float w;
        float h;

        pos[0] = y;
        pos[1] = x;
        pos[2] = 0.0f;

        const char* scan = (*text != 0) ? text : 0;

        if ((flags & 0x400) == 0)
        {
            char ch = *scan;
            int i = 0;

            if (ch != 0)
            {
                for (;;)
                {
                    if (ch == '|')
                    {
                        char next = scan[i + 1];
                        if (next == 'C')
                            goto coloured;
                        if (next == 'c')
                            goto coloured;
                    }

                    ch = scan[i + 1];
                    i += 1;
                    if (ch == 0)
                        break;
                }
            }

            flags |= 0x400;
        }

    coloured:
        w = box[2] - box[0];
        h = box[3] - box[1];

        //  The text goes in edx, which is why this is a free __fastcall and
        //  not a member of the font - see Render/textblockbuild.cpp.
        m_pRendered = BuildTextBlock(
            m_pFont, scan, &m_color[1], pos, m_fontHeight * m_scale, h, w,
            (int)flags, 0.0f);

        if ((m_style & 0x100) != 0)
        {
            unsigned char parentAlpha = m_pParent->m_88;

            float offsets[2];
            unsigned int color = m_shadowColor;
            int alpha = ((const unsigned char*)&m_shadowColor)[3];

            offsets[0] = m_shadowX * m_scale;
            offsets[1] = m_shadowY * m_scale;

            int scaled = alpha * (int)parentAlpha;
            ((unsigned char*)&color)[3] = (unsigned char)(scaled / 255);

            SetTextBlockTint(m_pRendered, offsets, CFrameColor(color));
        }
    }

    MarkLayerDirty();
}
