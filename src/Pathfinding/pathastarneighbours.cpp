//============================================================================
//  0x6F4995E0 - 0x6F4998C0 - the nine neighbour tests CPathAStar::ExpandCell
//  runs, one per direction plus one for the link table.
//
//  All nine do the same three things: resolve the cell to a node, refuse if
//  there is none, and relax it against the parent.  What differs is the gate
//  in between, and it only ever applies at quality mode 2:
//
//    * the four cardinals each have a gate of their own, and the two whose
//      step is vertical - north and south... no: north and *west* - carry a
//      second gate as well, taken only on an odd perpendicular coordinate and
//      only for a node that is of a portal kind.  That parity test is what
//      makes the pair asymmetric: a step across a half-cell boundary has one
//      more cell to clear than a step within one;
//    * the three diagonals NE, SE and SW have a single gate;
//    * NW has none at all, and neither does the link neighbour - which also
//      relaxes through a different function.
//
//  Only the four cardinals return anything: ExpandCell reads their results to
//  decide which diagonals it may try at all, and ignores everything else, so
//  the other five are void.  Those four are also the reason the four bodies
//  are wrapped in `if (node != -1) { ... }` with a single trailing
//  `return 0` rather than an early return: it is what puts the failure
//  epilogue out of line at the end, where the shipped code has it, and lets
//  the gate failures share it.
//
//  Own translation unit: ExpandCell reaches all nine with real calls.
//============================================================================
#include "pathastar.h"

//----------------------------------------------------------------------------
//  0x6F4995E0 - the north step.  Two gates: GateNorth, then GateNorthHalf
//  on an odd x and a portal-kind node.
//----------------------------------------------------------------------------
int CPathAStar::TestNorth(int parent, int x, int y)
{
    int node = LookupNode(0, x, y);
    if (node != -1)
    {
        if (m_mode == 2)
        {
            if (!GateNorth(x, y))
                return 0;

            if ((x & 1) != 0 && Nodes()[node].m_type > 0)
            {
                if (!GateNorthHalf(x, y))
                    return 0;
            }
        }

        Relax(node, parent);
        return 1;
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F499660 - the east step.  One gate: GateEast.
//----------------------------------------------------------------------------
int CPathAStar::TestEast(int parent, int x, int y)
{
    int node = LookupNode(0, x, y);
    if (node != -1)
    {
        if (m_mode == 2)
        {
            if (!GateEast(x, y))
                return 0;
        }

        Relax(node, parent);
        return 1;
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4996C0 - the south step.  One gate: GateSouth.
//----------------------------------------------------------------------------
int CPathAStar::TestSouth(int parent, int x, int y)
{
    int node = LookupNode(0, x, y);
    if (node != -1)
    {
        if (m_mode == 2)
        {
            if (!GateSouth(x, y))
                return 0;
        }

        Relax(node, parent);
        return 1;
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F499720 - the west step.  Two gates: GateWest, then GateWestHalf
//  on an odd y and a portal-kind node.
//----------------------------------------------------------------------------
int CPathAStar::TestWest(int parent, int x, int y)
{
    int node = LookupNode(0, x, y);
    if (node != -1)
    {
        if (m_mode == 2)
        {
            if (!GateWest(x, y))
                return 0;

            if ((y & 1) != 0 && Nodes()[node].m_type > 0)
            {
                if (!GateWestHalf(x, y))
                    return 0;
            }
        }

        Relax(node, parent);
        return 1;
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4997A0 - the north-east step.  One gate: GateNorthEast.
//----------------------------------------------------------------------------
void CPathAStar::TestNorthEast(int parent, int x, int y)
{
    int node = LookupNode(0, x, y);
    if (node == -1)
        return;

    if (m_mode == 2)
    {
        if (!GateNorthEast(x, y))
            return;
    }

    Relax(node, parent);
}

//----------------------------------------------------------------------------
//  0x6F4997F0 - the south-east step.  One gate: GateSouthEast.
//----------------------------------------------------------------------------
void CPathAStar::TestSouthEast(int parent, int x, int y)
{
    int node = LookupNode(0, x, y);
    if (node == -1)
        return;

    if (m_mode == 2)
    {
        if (!GateSouthEast(x, y))
            return;
    }

    Relax(node, parent);
}

//----------------------------------------------------------------------------
//  0x6F499840 - the south-west step.  One gate: GateSouthWest.
//----------------------------------------------------------------------------
void CPathAStar::TestSouthWest(int parent, int x, int y)
{
    int node = LookupNode(0, x, y);
    if (node == -1)
        return;

    if (m_mode == 2)
    {
        if (!GateSouthWest(x, y))
            return;
    }

    Relax(node, parent);
}

//----------------------------------------------------------------------------
//  0x6F499890 - the north-west step.  No gate at any mode.
//----------------------------------------------------------------------------
void CPathAStar::TestNorthWest(int parent, int x, int y)
{
    int node = LookupNode(0, x, y);
    if (node == -1)
        return;

    Relax(node, parent);
}

//----------------------------------------------------------------------------
//  0x6F4998C0 - the link-table step.  No gate at any mode.
//----------------------------------------------------------------------------
void CPathAStar::TestLink(int parent, int x, int y)
{
    int node = LookupNode(0, x, y);
    if (node == -1)
        return;

    RelaxLink(node, parent);
}

