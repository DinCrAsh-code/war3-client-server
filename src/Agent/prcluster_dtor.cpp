//============================================================================
//  0x6F4842E0 - NIpse::CPrCluster::~CPrCluster().  See prcluster.h for the
//  layout.  Releases the CMemBlock sub-object at +0x1C via the already-
//  reconstructed ReleaseCMemBlockBase (Pathfinding/cellbuffer.h, a naked
//  redirect to 0x6F4B3770), then restamps `this` straight to
//  `??_7CPresence@NTempest@@6B@` (0x6F951CA0) - no restamp of CPrCluster's
//  own vtable first, unlike NIpse::CRlAgent's own destructor
//  (Pathfinding/crlagent_dtor.cpp), which does restamp its own vtable
//  defensively before tearing its member down.  Same unreproducible
//  `__CxxFrameHandler3`/`__except_handler4`-shaped frame every other real
//  destructor in this base-chain family carries under `/GS- /EHs-c-`
//  (docs/msvc-vc8-idioms.md's own "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce" section) - the body below matches the dump
//  instruction for instruction past the frame furniture.
//
//  Own translation unit: prcluster.h's own header comment on this
//  declaration explains why nothing else in this session's own batch may
//  see this definition.
//============================================================================
#include "prcluster.h"
#include "cellbuffer.h"

namespace NIpse {

CPrCluster::~CPrCluster()
{
    //  &m_records aliases the same +0x1C..+0x30 bytes the original,
    //  flat-byte member did (this file's own follow-up session gave that
    //  member a real type, SPrClusterRecordTable, for Save/Load's own sake
    //  - Agent/prcluster.h's own header comment) - no behavioural change.
    ReleaseCMemBlockBase(&m_records);

    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@
}

}  // namespace NIpse
