//============================================================================
//  The highlighted-widget array the pause overlay keeps, and the focus
//  widget beside it.
//
//  The three words at 0x6FAB62C4 are one TSGrowableArray<CAgentPtr> - its
//  capacity, its count and its element pointer - and 0x6F37D260 really does
//  take the whole thing's address to destroy it.  They are declared as three
//  separate globals anyway, because IDA spells them separately and the
//  canonicaliser compares operand *symbols*: one array object would put
//  every reference to the second and third words an offset past a single
//  name and cost each of them its match.  The array-shaped uses take the
//  address of the first, which is the same address.
//============================================================================
#ifndef GAMEUIHIGHLIGHT_H
#define GAMEUIHIGHLIGHT_H

class CSelectable;

extern unsigned int  g_highlightedWidgetAlloc;   // dword_6FAB62C4
extern unsigned int  g_highlightedWidgetCount;   // dword_6FAB62C8
extern CSelectable** g_pHighlightedWidgets;      // dword_6FAB62CC
extern CSelectable*  g_pFocusWidget;             // dword_6FAB62D4

#endif
