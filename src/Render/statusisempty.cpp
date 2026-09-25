//============================================================================
//  0x6F4C3660 - CStatus::IsEmpty.  `m_list` is a TSList<STATUSENTRY,0>
//  (cstatus.h), so its own Tail() already reproduces the branchless
//  `setle`/`sub`/`and` mask storm.h documents for TSList's raw/masked
//  accessor pair; comparing that masked pointer to 0 is what turns the
//  four-instruction mask into the trailing `test`/`setz`.  m_list sits at
//  +0x04, and TSList's own m_linkoffset/m_terminator fields put the
//  terminator's m_prevlink - the field this reads - at +0x0C of `this`,
//  matching the dump directly.
//============================================================================
#include "cstatus.h"

int CStatus::IsEmpty()
{
    return m_list.Tail() == 0;
}
