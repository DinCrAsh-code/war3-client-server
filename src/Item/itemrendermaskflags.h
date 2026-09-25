//============================================================================
//  SItemRenderMaskFields - see itemrendermaskflags.cpp for the four leaf
//  accessors reconstructed against it.  Split into its own header once a
//  second TU (Widget/widget_ghostimagemaskclear.cpp) needed the same
//  class rather than a second, incompatible redeclaration.
//============================================================================
#ifndef ITEMRENDERMASKFLAGS_H
#define ITEMRENDERMASKFLAGS_H

struct SItemRenderMaskFields
{
    char            m_reserved00[0x4A];
    unsigned short  m_maskLow;     // +0x4A
    unsigned short  m_maskHigh;    // +0x4C

    unsigned short GetMaskLow() const;
    void SetMaskLow(unsigned short value);
    unsigned short GetMaskHigh() const;
    void OrMaskHigh(unsigned short bits);
};

#endif
