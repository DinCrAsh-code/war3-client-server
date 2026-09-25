//============================================================================
//  0x6F455C80 - Node::~Node() (jassnodes.h's own note on why this is
//  declared out of line and defined in its own TU).  jscript-bin1: reached
//  from NodeDebug::~NodeDebug (0x6F460960, jassnodedtors.cpp) and its
//  siblings' own base part, confirmed by xref rather than guessed.
//============================================================================
#include "jassnodes.h"

Node::~Node()
{
}
