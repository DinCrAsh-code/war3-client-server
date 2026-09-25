//============================================================================
//  CModelFrame and CBackdropFrame - two links of the CFrame chain the chat
//  bar's own base sits on.
//
//  Both names are the shipped ones, off the vtable pair each constructor
//  stamps at +0x00 and +0x0B4 (the CLayer / CLayoutFrame pair that says a
//  CFrame is underneath).  The chain terminates cleanly: CBackdropFrame's
//  constructor calls CModelFrame's, and CModelFrame's calls CFrame's
//  (0x6F608200), which this repo already reconstructs.
//
//  Neither is reached from this target for anything but construction, so
//  only the fields the two constructors write are named.
//============================================================================
#ifndef MODELFRAME_H
#define MODELFRAME_H

#include "frame.h"

class CModelFrame : public CFrame
{
public:
    //  0x6F621B70 - Frame/modelframector.cpp.  `retn 0Ch`.
    CModelFrame(int a, int b, int c);

    int m_field168;     // +0x168
    int m_field16C;     // +0x16C
    int m_field170;     // +0x170
};

class CBackdropFrame : public CModelFrame
{
public:
    //  0x6F620ED0 - Frame/backdropframector.cpp.  `retn 0Ch`.
    CBackdropFrame(int a, int b, int c);

    char  m_reserved174[0x04];  // +0x174
    //  +0x178 .. +0x190 - seven words the constructor clears last, after the
    //  flag merges.
    int   m_field178;           // +0x178
    int   m_field17C;           // +0x17C
    int   m_field180;           // +0x180
    int   m_field184;           // +0x184
    int   m_field188;           // +0x188
    int   m_field18C;           // +0x18C
    int   m_field190;           // +0x190
    char  m_reserved194[0x24];  // +0x194
    //  +0x1B8 - 0.025, the only field that does not start at zero.  The chat
    //  bar overwrites it with 0.01.
    float m_float1B8;           // +0x1B8
    float m_float1BC;           // +0x1BC
    int   m_field1C0;           // +0x1C0
    int   m_field1C4;           // +0x1C4
    //  +0x1C8 .. +0x1D4 - the backdrop's four insets, all zero here and all
    //  four rewritten by the chat bar.
    float m_inset[4];           // +0x1C8 .. +0x1D8
    int   m_field1D8;           // +0x1D8
};

#endif
