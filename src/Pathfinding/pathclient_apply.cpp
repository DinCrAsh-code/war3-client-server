//============================================================================
//  0x6F49B680 - NIpse::CLrPath::Apply, reached through vtable slot 3.  Not
//  CLrPath's own override in the sense of *replacing* the base behaviour -
//  it calls straight into CPresenceTagged's own override
//  (SHandleTableEntry::RegisterOrReuse, 0x6F4AEFF0, handletable.h/.cpp;
//  same base-chain slot-3 column CRlAgent::Apply's own file header walks,
//  Pathfinding/crlagent.h) with `this` unchanged - the same "this class IS
//  the handle-table-entry-shaped base at offset 0" shape
//  agentbaseabs_slot3.cpp's own CRlAgent::Apply already establishes for a
//  sibling class - and only afterward stamps a batch of CLrPath's own
//  freshly-reset search-state fields.
//
//  `+0xB4` (m_radius, a CFloat) is stamped with `g_CFloatZero`'s own bit
//  pattern the same way Pathfinding/crlagent_ctor.cpp's own +0x44 field is
//  (dereferences the global rather than embedding an immediate 0 - a
//  CFloat field, not a raw int).
//============================================================================
#include "pathmove.h"
#include "CFloat.h"

//  Pathfinding/handletable.h's own SHandleTableEntry/RegisterOrReuse, minus
//  that header's own full `#include "game.h"` pull - which redefines
//  `SHandleTable` against a different, incompatible body of this file's own
//  `game.h` (through pathmove.h). Only the one method actually called here
//  is redeclared; the real definition (handletableregister.cpp) is what the
//  linker resolves this call to either way.
struct SHandleRegistrationRequest;
struct SHandleTableEntry
{
    void RegisterOrReuse(SHandleRegistrationRequest* request);
};

void CPathClient::Apply(SHandleRegistrationRequest* request)
{
    ((SHandleTableEntry*)this)->RegisterOrReuse(request);

    m_typeMask         = 0;
    m_radius           = *(const CFloat*)&g_CFloatZero;
    m_flags            = 0x200000;
    m_stamp[0]         = 0;
    m_cursor[0]        = (unsigned int)-1;
    m_sizeClass        = 0x2BC;
    m_cache[0].m_growChunk = 0x80;
    m_cursor[1]        = (unsigned int)-1;
    m_stamp[1]         = 0;
    m_laneKey          = 0x190;
    m_cache[1].m_growChunk = 0x80;
}
