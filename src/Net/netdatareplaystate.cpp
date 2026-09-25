//============================================================================
//  0x6F537D20 and 0x6F537DB0 - CNetData's two state setters, and both names
//  come straight out of their own diagnostic strings:
//  "CNetData::SetReplayState (from %s to %s)" and
//  "CNetData::SetRecordState (from %s to %s)".
//
//  Each is a transition table written as a switch on the *new* state, with
//  the current one tested inside each arm.  A transition that is not allowed
//  prints both names through the module's own `SysMessagePrintf` and answers
//  0 without changing anything; an allowed one stores and answers 1.
//
//  Assigning the state it already holds is always allowed, and it is tested
//  first and separately - `xor eax,eax` / `cmp` / `setz al` / `test`, not a
//  branch - which is what says it is its own leading condition rather than a
//  case of the table.
//============================================================================
#include "netdata.h"

//  Replay/replaystatenames.cpp - the two diagnostic name tables.
const char* __fastcall ReplayStateName(unsigned int state);
const char* __fastcall RecordStateName(unsigned int state);

struct SNetDataStates
{
    int SetReplayState(unsigned int state);     // 0x6F537D20
    int SetRecordState(unsigned int state);     // 0x6F537DB0

    char         m_reserved000[0x614];
    unsigned int m_replayState;     // +0x614
    char         m_reserved618[0x5CC];
    unsigned int m_recordState;     // +0xBE4
};

//----------------------------------------------------------------------------
//  0x6F537D20 - five states and a jump table.
//
//  Every "is this the state we are already in" test is written as a *value*
//  (`int ok = (a == b); if (ok)`), not as a branch: the shipped stream is
//  `xor eax,eax` / `cmp` / `setz al` / `test eax,eax` / `jnz`, which a plain
//  `if (a == b)` does not produce.  The two arms that test *two* values
//  (1 and 4) branch directly instead, and that asymmetry is the shipped
//  code's, not a slip.
//----------------------------------------------------------------------------
int SNetDataStates::SetReplayState(unsigned int state)
{
    unsigned int current = m_replayState;

    //  The shipped stream materialises this as a value first -
    //  `xor eax,eax` / `cmp` / `setz al` / `test eax,eax` / `jnz` - where
    //  this build folds it straight into a `cmp`/`je`.  Spelling it as a
    //  named `int`, as a `bool`, and as `(a == b) != 0` were all measured
    //  and all fold the same way; it is left named because that is what the
    //  shipped source must have had.
    int unchanged = (state == current);
    if (!unchanged)
    {
        switch (state)
        {
        case 0:
            break;

        case 1:
            if (current != 0 && current != 1)
                goto refuse;
            break;

        case 2:
            {
                int ok = (current == 1);
                if (!ok)
                    goto refuse;
            }
            break;

        case 3:
            {
                int ok = (current == 0);
                if (!ok)
                    goto refuse;
            }
            break;

        case 4:
            if (current != 2 && current != 3)
                goto refuse;
            break;

        default:
            goto refuse;
        }
    }

    m_replayState = state;
    return 1;

refuse:
    SysMessagePrintf("CNetData::SetReplayState (from %s to %s) is not a legal transition",
                     ReplayStateName(current), ReplayStateName(state));
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F537DB0 - two states, so a `sub`/`jz` chain rather than a table
//  (docs/msvc-vc8-idioms.md, "A two-label `switch` lowers to `sub`/`jz`
//  pairs; an if/else-if does not").
//----------------------------------------------------------------------------
int SNetDataStates::SetRecordState(unsigned int state)
{
    unsigned int current = m_recordState;

    //  The shipped stream materialises this as a value first -
    //  `xor eax,eax` / `cmp` / `setz al` / `test eax,eax` / `jnz` - where
    //  this build folds it straight into a `cmp`/`je`.  Spelling it as a
    //  named `int`, as a `bool`, and as `(a == b) != 0` were all measured
    //  and all fold the same way; it is left named because that is what the
    //  shipped source must have had.
    int unchanged = (state == current);
    if (!unchanged)
    {
        switch (state)
        {
        case 0:
            break;

        case 1:
            {
                int ok = (current == 0);
                if (!ok)
                    goto refuse;
            }
            break;

        default:
            goto refuse;
        }
    }

    m_recordState = state;
    return 1;

refuse:
    SysMessagePrintf("CNetData::SetRecordState (from %s to %s) is not a legal transition",
                     RecordStateName(current), RecordStateName(state));
    return 0;
}
