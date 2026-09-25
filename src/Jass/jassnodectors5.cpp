//============================================================================
//  Jass2/Nodes.h - AST node constructors, jassvm-batch-5's own closure
//  (NodeAnd/NodeOr/NodeCompare/NodeMath/NodeMod/NodeBool/NodeNull/NodeInt/
//  NodeReal/NodeNot/NodeUMinus/NodeArgList/NodeDebug/NodeFunction - a
//  disjoint address set from jassvm-batch-4's own 20 in jassnodectors.cpp,
//  no address in common).  Every one of these constructors is defined
//  inline inside its own struct in jassnodes.h; this translation unit
//  places one `new (g_jassNodePool.Alloc(...)) NodeXxx(...)` per
//  constructor so each is actually emitted rather than optimised away as
//  dead code - the same forcing shape jassvm-batch-4's own
//  `JassNodeCtors_Batch4_Reference` uses, and for the same reason: the
//  placement address comes from a real function call the compiler cannot
//  prove has no observable effect, so the constructor that follows it
//  cannot be proven dead either.
//============================================================================
#include "jassnodes.h"

extern "C" void JassNodeCtors_Batch5_Reference()
{
    JassCompileContext* p = 0;
    Node* n = 0;
    NodeArgList* args = 0;

    new (g_jassNodePool.Alloc(0, 0, 0)) NodeAnd(p, n, n);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeOr(p, n, n);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeCompare(0, p, n, n);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeMath(0, p, n, n);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeMod(p, n, n);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeBool(p, 0);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeNull(p);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeInt(p, 0);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeReal(p, 0.0f);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeNot(p, n);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeUMinus(p, n);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeArgList(p, n, args);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeDebug(p, n);
    new (g_jassNodePool.Alloc(0, 0, 0)) NodeFunction(p, 0, args, 0);
}
