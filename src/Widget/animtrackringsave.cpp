//============================================================================
//  0x6F4D5EE0 - SaveAnimTrackRing, and the whole four-deep nested Save
//  family it roots: SaveAnimTrackRing (this file) -> SaveAnimRecordArray
//  (animrecordarraysave.cpp) -> SaveAnimTrackRecord
//  (animtrackrecordsave.cpp) -> SaveAnimKeyArrayHeader
//  (animkeyarrayheadersave.cpp).  Reached through
//  CWidget::NegateAndForwardOffset's closure.
//
//  All four take the store in ecx and the object being saved in edx, no
//  stack arguments (`retn` with nothing to clean) - exactly the fastcall
//  shape widget_save.cpp's own WriteCFloat already uses for a
//  CDataStoreScratch-plus-one-pointer writer, so these are free functions
//  in that same style rather than members of the type being saved.  Each
//  returns the store itself (`this`), matching every CDataStoreScratch
//  writer's own fluent-chain return.
//
//  The nesting is exact, not inferred: every intermediate pointer this
//  session traced (ring+0x04 as &SAnimRecordArray, record+0x08 as
//  &SAnimKeyArrayHeader) is the same object animtrackring.h and
//  animrecordarraysetalloc.cpp already establish, reached by the identical
//  address arithmetic those files use, and every writer called from any of
//  the four (WriteByte/WriteDword/WriteDwordAlt/WriteDwordAlt3) is already
//  reconstructed in cdatastorescratch.cpp.
//
//  Four separate translation units, not one: the shipped body calls each
//  level out of line (a real `call`, never inlined into its caller), and
//  this compiler's own /Ob2 *does* inline a same-TU callee this small -
//  putting all four in one file scored SaveAnimRecordArray at 0.205
//  (SaveAnimTrackRecord and SaveAnimKeyArrayHeader folded straight into it)
//  before the split.  One TU per level, each seeing only a forward
//  declaration of the next, reproduces the shipped out-of-line calls
//  exactly - the same reason CLAUDE.md gives for putting a callee in its
//  own TU rather than reaching for `__declspec(noinline)`.
//
//  The read side of this same family (CDataStore::ReadDword-family
//  counterparts of these four) is not reconstructed: sub_6F4D77C0 and
//  sub_6F4D5B30 (the SAnimRecordArray/SAnimKeyArrayHeader load-side growers)
//  both call a key-array allocator (sub_6F4D5620) this session found no
//  `asm/` dump for and could not reach IDA to fetch one for either
//  (ida_query timed out - no worker answered), so its own `retn <n>` is
//  unknown and a thunk for it cannot be declared without guessing the ABI -
//  exactly the class of guess CLAUDE.md rules out.  Left for a session with
//  a live IDA connection; sub_6F4D8140 stays a thunk.
//============================================================================
#include "animtrackring.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall SaveAnimRecordArray(CDataStoreScratch* store,
                                                   SAnimRecordArray* records);

CDataStoreScratch* __fastcall SaveAnimTrackRing(CDataStoreScratch* store,
                                                 CAnimTrackRing* ring)
{
    store->WriteByte(ring->m_count);
    store->WriteByte(ring->m_head);
    store->WriteByte(ring->m_write);
    SaveAnimRecordArray(store, (SAnimRecordArray*)&ring->m_storage);
    return store;
}
