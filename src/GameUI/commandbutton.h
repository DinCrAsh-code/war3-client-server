//============================================================================
//  CCommandButton - one cell of the command card.
//
//  The class name is the shipped one (??_7CCommandButton@@6B@, stamped at
//  0x6F35E557) and so is its module: its own allocation is tagged
//  "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI/CCommandButton.cpp".  A
//  CSimpleButton is 0x168 bytes and this class's own fields start at exactly
//  +0x168, which is what settles the base.
//============================================================================
#ifndef COMMANDBUTTON_H
#define COMMANDBUTTON_H

#include "frame.h"

//----------------------------------------------------------------------------
//  The 0x6D8-byte record a command button optionally owns, built at
//  0x6F34F850 - which stamps ??_7CCommandButtonData@@6B@ and is what names
//  it.  Only its construction is reached here, so it is spelled as a
//  `Construct` member on its own storage rather than as a C++ constructor:
//  the body is still a redirect, MSVC will not take __declspec(naked) on a
//  constructor, and nothing in this tree establishes a single one of its
//  fields.  It hands `this` back, which is what the shipped `mov ecx, eax` /
//  `call` / `mov [esi+190h], eax` sequence uses.
//----------------------------------------------------------------------------
struct CCommandButtonData
{
    //  0x6F34F850 - `retn 0`.  Still a redirect.
    CCommandButtonData* Construct();
};

class CCommandButton : public CSimpleButton
{
public:
    //  0x6F3687C0 - GameUI/commandbuttondtor.cpp.  `retn 0`.
    ~CCommandButton();
    //  0x6F35E4E0 - GameUI/commandbuttonctor.cpp.  `retn 8`.
    CCommandButton(void* parent, int withData);

    //  0x6F337380 - store `alpha` at +0x1A4 and, when the button has a
    //  sprite slot at +0x19C with a live head, push it at that too.
    //  `retn 4`; still a redirect.
    void SetIconAlpha(float alpha);

    //  +0x168 .. +0x184 - eight floats the constructor zeroes as one `fldz`
    //  run.  Nothing in this tree reads any of them back.
    float m_span[8];                // +0x168
    int   m_field188;               // +0x188
    //  +0x18C - 1 exactly when the button owns the record below.
    int   m_hasData;                // +0x18C
    CCommandButtonData* m_pData;    // +0x190
    int   m_field194;               // +0x194
    int   m_field198;               // +0x198
    int   m_field19C;               // +0x19C
    char  m_reserved1A0[0x04];      // +0x1A0
    //  +0x1A4 - the only field that does not start at zero: `fld1`.  The
    //  inventory bar drops it to 0.82 through SetIconAlpha below, and that
    //  setter hands it on to the button's own sprite slot when it has one,
    //  which is the whole of the evidence for the name: nothing in this call
    //  tree reads it back.
    float m_iconAlpha;              // +0x1A4
    int   m_field1A8;               // +0x1A8
    char  m_reserved1AC[0x04];      // +0x1AC
    int   m_field1B0;               // +0x1B0
    int   m_field1B4;               // +0x1B4
};

#endif
