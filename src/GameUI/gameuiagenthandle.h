//============================================================================
//  The handle-table object every widget in this batch is checked through,
//  and the checked cast that goes with it.
//
//  Both are already established - see Widget/widgetpathref.cpp, which
//  carries the full note on the `setnz`/`sub 1`/`and` shape - and
//  `LookupHandle` itself comes from Game/game.h rather than being
//  re-declared here.  Spelling it a second time with a different return type
//  is not a style question: MSVC puts the return type in the mangled name,
//  so it made two symbols for one shipped address, `extract_gen.py`
//  qualified the now-ambiguous readable name, and **every** call site of
//  LookupHandle in the repo silently lost its match (forty functions, one
//  instruction each) while the link would have failed in build_mix.py.
//============================================================================
#ifndef GAMEUIAGENTHANDLE_H
#define GAMEUIAGENTHANDLE_H

#include "game.h"           // CHandleObject, SItemHandleObject, LookupHandle

//  '+agl' MSB first - the agile type id every handle-registered object
//  carries.  Same constant as kAgileHandleTag (agentregistry.h).
const unsigned int kAgentTagPathable = 0x2B61676C;

//  The agent's own handle/type pair, at +0x0C and +0x10.
struct SAgentHandlePair
{
    char         m_reserved00[0x0C];
    unsigned int m_handle;       // +0x0C
    int          m_handleType;   // +0x10
};

#endif
