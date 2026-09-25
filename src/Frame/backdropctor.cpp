//============================================================================
//  0x6F61D5A0 - CBackdropGenerator::CBackdropGenerator.
//
//  Five null texture slots, two counted-string handles and the geometry: an
//  edge width of 0.025 and four zero insets.  Nothing here allocates - the
//  nine pieces are Generate()'s.
//
//  The five pointers are in the member-initialiser list rather than in the
//  body, because that is the only split that puts them *ahead* of the two
//  RCStaticString constructors: MSVC runs the list in declaration order and
//  the two strings are declared after all five (docs/msvc-vc8-idioms.md, "A
//  member-initialiser list runs in declaration order, so split the
//  constructor").  +0x34 is not written by the shipped code at all and is
//  left alone here.
//
//  Score capped by the unified `__except_handler4`-shaped EH frame this
//  toolchain cannot reproduce: eleven instructions of prologue and epilogue
//  and one state store, none of which /GS- /EHs-c- emits.  See framecore.cpp.
//============================================================================
#include "frame.h"
#include "framethunks.h"

CBackdropGenerator::CBackdropGenerator()
    : m_pBackground(0)
    , m_pEdgeLeft(0)
    , m_pEdgeRight(0)
    , m_pEdgeTop(0)
    , m_pEdgeBottom(0)
{
    m_pieces = 0;
    m_tiled = 0;

    m_edgeSize = 0.025f;

    m_3C = 0.0f;
    m_insetTop = 0.0f;
    m_insetBottom = 0.0f;
    m_insetLeft = 0.0f;
    m_insetRight = 0.0f;
}
