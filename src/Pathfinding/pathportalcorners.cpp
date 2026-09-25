//============================================================================
//  0x6F499CF0 / 0x6F499D60 / 0x6F499DD0 / 0x6F499E40 - the portal expander's
//  four diagonal neighbours.
//
//  Simpler than the spans: a corner is one cell, so there is nothing to
//  split.  The loop only walks *down* the pyramid until the cell resolves to
//  something other than "belongs to a finer block", and there is no
//  recursion and no pair of end flags to report.
//
//  The north-west corner has no gate at all - not an omission here, the
//  shipped body has no `m_mode` test and no call between its loop and its
//  relax.  The other three each take exactly one, and the south-east one
//  reuses PortalGateEast rather than having its own.
//
//  Whether any of the four runs at all is decided by the caller, from the
//  span end flags: a diagonal is offered only when both spans flanking it
//  reported their shared end reachable, which is the same no-cutting-through-
//  a-corner rule CPathAStar::ExpandCell applies one level finer.
//============================================================================
#include "pathastar.h"

//----------------------------------------------------------------------------
//  0x6F499CF0
//----------------------------------------------------------------------------
void CPathAStar::PortalNorthEast(int node, int level, int x, int y)
{
    int found = LookupNode(level, x, y);

    while ((unsigned int)found >= 0xFFFFFFFEu)
    {
        if ((unsigned int)found != 0xFFFFFFFEu)
            return;
        --level;
        found = LookupNode(level, x, y);
    }

    if (m_mode == 2 && !PortalGateNorthEastCorner(x, y))
        return;

    Relax(found, node);
}

//----------------------------------------------------------------------------
//  0x6F499D60
//----------------------------------------------------------------------------
void CPathAStar::PortalSouthEast(int node, int level, int x, int y)
{
    int found = LookupNode(level, x, y);

    while ((unsigned int)found >= 0xFFFFFFFEu)
    {
        if ((unsigned int)found != 0xFFFFFFFEu)
            return;
        --level;
        found = LookupNode(level, x, y);
    }

    if (m_mode == 2 && !PortalGateEast(x, y))
        return;

    Relax(found, node);
}

//----------------------------------------------------------------------------
//  0x6F499DD0
//----------------------------------------------------------------------------
void CPathAStar::PortalSouthWest(int node, int level, int x, int y)
{
    int found = LookupNode(level, x, y);

    while ((unsigned int)found >= 0xFFFFFFFEu)
    {
        if ((unsigned int)found != 0xFFFFFFFEu)
            return;
        --level;
        found = LookupNode(level, x, y);
    }

    if (m_mode == 2 && !PortalGateSouthWestCorner(x, y))
        return;

    Relax(found, node);
}

//----------------------------------------------------------------------------
//  0x6F499E40 - the one with no gate.
//----------------------------------------------------------------------------
void CPathAStar::PortalNorthWest(int node, int level, int x, int y)
{
    int found = LookupNode(level, x, y);

    while ((unsigned int)found >= 0xFFFFFFFEu)
    {
        if ((unsigned int)found != 0xFFFFFFFEu)
            return;
        --level;
        found = LookupNode(level, x, y);
    }

    Relax(found, node);
}
