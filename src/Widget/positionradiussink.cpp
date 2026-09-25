//============================================================================
//  0x6F4766A0 - SPositionRadiusSink::SetRadius: flush a radius to zero if it
//  is small enough, resolve the handle it belongs to, and hand the result
//  on.
//
//  Was a `mov eax,<address> / jmp eax` redirect.  Seventeen instructions,
//  and the only arithmetic in it is a bit-trick this repo already carries
//  verbatim: `((bits - 0x3000000) ^ bits) >> 31` as a flush mask and
//  `bits + 0xFD800000` as the scaled value, exactly as
//  SHandleWithType::FlushedOriginDelta (timesyncbounds.cpp) has it on both
//  axes.  It stays a literal transliteration here for the same reason that
//  one does - see timesyncbounds.h.
//
//  The object it belongs to is the same handle-plus-type pair
//  timesyncbounds.h's SHandleWithType describes (+0x08 and +0x0C), and the
//  resolve is the ordinary LookupHandle.
//
//  The flushed CFloat lands in the incoming argument's own stack slot,
//  which is MSVC reusing a dead parameter slot for a local rather than
//  anything the source says.
//
//  The argument push comes *before* the resolve, which is the same
//  arguments-then-receiver ordering AgentSlot5CGate (agentslot5cgate.cpp)
//  and AttachmentSetVisible turn on - so the whole thing is one expression.
//
//  Own translation unit: CItem::RefreshPlacementRadius reaches it with a
//  real call and it makes two of its own.
//============================================================================
#include "CFloat.h"
#include "game.h"

//  0x6F4A7350 is CPathTrace::SetRadius (pathtraceradius.cpp), already
//  reconstructed - declared through game.h, not re-thunked here.  A second
//  naked body at the same address is the two-symbols-for-one-function defect
//  docs/notes/verifier-gate-link-failures.md warns about, and this file had
//  one until the merge that brought the real reconstruction in caught it.

struct SPositionRadiusSink
{
    void SetRadius(const CFloat* radius);

    char         m_reserved00[8];
    unsigned int m_handle;    // +0x08
    int          m_typeTag;   // +0x0C
};

void SPositionRadiusSink::SetRadius(const CFloat* radius)
{
    int bits = radius->m_bits;
    int flush = ((bits - 0x3000000) ^ bits) >> 31;
    CFloat flushed = CFloat(~flush & (bits + 0xFD800000));

    ((CPathTrace*)LookupHandle(m_handle, m_typeTag))->SetRadius(&flushed);
}
