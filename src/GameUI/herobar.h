//============================================================================
//  The hero bar and its seven buttons.
//
//  Both class names are the shipped ones - ??_7CHeroBar@@6B@ / its `_0` twin
//  at +0x15C, and ??_7CHeroBarButton@@6B@ / its twin at +0x1B8 - and both
//  constructors tag their allocations
//  "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI/CHeroBar.cpp".
//
//  The two `_0` offsets fix both bases exactly: CSimpleGrid is 0x15C bytes
//  and CCommandButton is 0x1B8, so each class is that plus a CObserver, and
//  each one's own fields start twelve bytes after the twin.  The button's
//  size is confirmed from the other side as well - the bar allocates 0x204
//  bytes for one.
//============================================================================
#ifndef HEROBAR_H
#define HEROBAR_H

#include "commandbutton.h"
#include "agenttypedslots.h"
#include "frame.h"
#include "observer.h"
#include "storm.h"

//----------------------------------------------------------------------------
//  CStatBar - the 0x158-byte bar a hero button carries two of (health and
//  mana, by the order they are built).  The name is the shipped one, off the
//  ??_7CStatBar@@6B@ its own constructor stamps.  Only that constructor is
//  reached here and it is still a redirect, so it is spelled as a `Construct`
//  member on the storage: MSVC will not take __declspec(naked) on a
//  constructor.  It hands `this` back.
//----------------------------------------------------------------------------
struct CStatBar
{
    //  0x6F359CC0 - `retn 0Ch`.  Still a redirect.
    CStatBar* Construct(void* parent, int a, int kind);
};

class CHeroBarButton : public CCommandButton, public CObserver
{
public:
    //  0x6F368F90 - GameUI/herobarbutton.cpp.  `retn 4`.
    CHeroBarButton(void* parent);

    //  +0x1C4 - a '+w3u' checked slot, built from null.
    SCheckedUnitSlot m_unit;        // +0x1C4
    int       m_field1C8;           // +0x1C8
    CStatBar* m_pStatBarA;          // +0x1CC
    CStatBar* m_pStatBarB;          // +0x1D0
    int       m_field1D4;           // +0x1D4
    //  +0x1D8 - the "HeroBarPointModel" sprite frame, parented on the game
    //  UI's own sprite host rather than on the button.
    class CSpriteFrame* m_pPointSprite; // +0x1D8
    //  +0x1DC - zeroed with the rest and then left at 0xFFFF4444, the last
    //  thing the constructor writes.
    int       m_field1DC;           // +0x1DC
    float     m_float1E0;           // +0x1E0
    char      m_reserved1E4[0x04];  // +0x1E4
    //  +0x1E8 - a CEventTimer (Frame/frame.h), 0x1C bytes, which is exactly
    //  what is left of the 0x204 the hero bar allocates for one of these.
    CEventTimer m_timer;            // +0x1E8 .. +0x204
};

class CHeroBar : public CSimpleGrid, public CObserver
{
public:
    //  0x6F3692D0 - GameUI/herobar.cpp.  `retn 4`.
    CHeroBar(void* parent);

    //  +0x168 .. +0x174 - a TSExplicitList with link offset 0: the
    //  constructor writes 0 into +0x168 and self-links the terminator at
    //  +0x16C, which is TSExplicitList's own default constructor.
    TSExplicitList<void, 0> m_list;     // +0x168 .. +0x174
    int   m_field174;                   // +0x174
    float m_float178;                   // +0x178
    int   m_field17C;                   // +0x17C
    int   m_field180;                   // +0x180
};

#endif
