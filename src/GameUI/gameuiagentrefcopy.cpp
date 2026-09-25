//============================================================================
//  0x6F34D760 - SAgentRefHolder::CopyRef.
//
//  Hand the caller a fresh checked slot built from whatever this holder is
//  pointing at.  The destination is the only stack argument and is also the
//  return value, which is the by-value-return shape
//  docs/msvc-vc8-idioms.md records: `this` in ecx, the buffer pushed last.
//
//  Own translation unit: 0x6F012CF0 is a real call.
//============================================================================
#include "gameuithunks.h"
#include "agenttypedslots.h"

struct SAgentRefHolder
{
    void* m_pAgent;                 // +0x00

    SCheckedDestructableSlot* __thiscall CopyRef(SCheckedDestructableSlot* out) const;
};

SCheckedDestructableSlot* SAgentRefHolder::CopyRef(SCheckedDestructableSlot* out) const
{
    out->Construct((CAgent*)m_pAgent);

    return out;
}
