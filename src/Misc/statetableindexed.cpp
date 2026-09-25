//============================================================================
//  0x6F741720 - SStateTable::ApplyIndexed: turn the index into a row of the
//  32-byte array at +0x79C and hand that row the value.
//
//  Its own translation unit because ApplyByKind tail-jumps into it; folded in
//  with that function it would inline and the jump would go.  It has no
//  heading of its own in the dump - it is one of the blocks appended to
//  0x6F763F00 - so nothing scores it directly.
//============================================================================
#include "statetable.h"

void SStateTable::ApplyIndexed(void* value, int index)
{
    ApplyStateRow(m_pRows + (index << 5), value);
}
