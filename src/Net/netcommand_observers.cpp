//============================================================================
//  0x6F5378E0 - CNetPlayerRecord::FireToObserverLists.  Its own TU
//  (CLAUDE.md, "One translation unit per original module") - kept in the
//  same file as CNetData::FireCommand (netcommand_fire.cpp) it scored
//  EXACT on its own out-of-line definition, but FireCommand's own call
//  site still got a second, inlined copy instead of the real `call`
//  instruction the target has, corrupting FireCommand's own score.
//============================================================================
#include "netcommand.h"

int CNetPlayerRecord::FireToObserverLists(CNetCommandBase* cmd)
{
    SAgentMessage* msg = (SAgentMessage*)cmd;
    int delivered = 0;
    if (m_pauseObserverList.Method_0x10(msg) != 0)
        delivered = 1;
    if (m_uiObserverList.Method_0x10(msg) != 0)
        return 1;
    return delivered;
}
