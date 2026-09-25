//============================================================================
//  CNetData::SLOTTABLE::SLOTTABLE() / ::~SLOTTABLE() - 0x6F54F430 /
//  0x6F54FA00.  cnd-ctor-agent-2 session, chasing CNetData::CNetData's own
//  prerequisite member subobjects (docs/notes/checksum-provider-registry.md,
//  "cnd-ctor-agent session").
//
//  The constructor is a plain TSHashTable<CPlayer,HASHKEY_NONE> default
//  init: stamp the table's own vtable, default-construct the embedded
//  TSExplicitList<CPlayer> (which self-poisons its own m_linkoffset to
//  0xDDDDDDDD and self-links its terminator - storm.h's own
//  `TSExplicitList()`), then immediately call `SetLinkOffset(0xC)` on it -
//  which is exactly what the shipped body's own "compare against 0xC,
//  never equal, always take the branch" shape is: 0xDDDDDDDD is never 0xC,
//  so the real link offset always gets installed through the same
//  UnlinkAll-then-relink path `SetLinkOffset` already provides (storm.h),
//  reusing that method instead of transcribing its body a second time.
//============================================================================
#include "netdata.h"
#include "storm.h"

const void* const kSlotTableVftable = (const void*)0x6F9590BC;

CNetData::SLOTTABLE::SLOTTABLE()
{
    m_vtable = (void*)kSlotTableVftable;

    //  Embedded TSExplicitList<CPlayer> at +0x04..+0x10 (m_listLinkOffset/
    //  m_listNext/m_listPrevlink) - default-construct it in place, then
    //  install the real link offset the same way the shipped body does.
    new (&m_listLinkOffset) TSExplicitList<CNetData::CPlayer>();
    ((TSExplicitList<CNetData::CPlayer>*)&m_listLinkOffset)->SetLinkOffset(0xC);

    m_probes      = 0;
    m_bucketAlloc = 0;
    m_bucketCount = 0;
    m_buckets     = 0;
    m_bucketChunk = 0;
    m_mask        = 0xFFFFFFFF;

    //  m_playerAlloc..m_field48 (+0x28..+0x48) are deliberately NOT set
    //  here - the real 0x6F54F430 never touches them either.  They are
    //  zeroed by the one real caller, CNetPlayerRecord::CNetPlayerRecord
    //  (netplayerrecordctor.cpp), immediately after this constructor
    //  returns - reproduced there rather than here to match the shipped
    //  instruction stream's own split between the two functions.
}

//  0x6F54F660 - TSHashTable<CPlayer,HASHKEY_NONE>'s own base-class
//  destructor tail (restamps the table's own vtable again, then further
//  teardown this session did not chase - out of scope for "CNetData's own
//  direct prerequisite subobjects", the same DIFFERS/THUNK-leaf allowance
//  netobserver.h's own file header documents for CNetObserver::~CNetObserver).
__declspec(naked) void SlotTableBaseDestructTail(void*)
{
    __asm { mov eax, 0x6F54F660 }
    __asm { jmp eax }
}

CNetData::SLOTTABLE::~SLOTTABLE()
{
    //  0x6F54B1A0 - ReleaseAll(0), already a real reconstruction
    //  (netsessionslottableclear.cpp).
    ReleaseAll(0);

    //  ".PAUCPlayer@CNetData@@" - the same tag Net/netdata.h's own file
    //  header documents for this array's Storm allocation.
    if (m_players)
        SMemFree(m_players, ".PAUCPlayer@CNetData@@", -2, 0);

    m_playerAlloc = 0;
    m_playerCount = 0;
    m_players     = 0;
    m_field48     = 0;

    if (m_referees)
        SMemFree(m_referees, ".PAUCPlayer@CNetData@@", -2, 0);

    //  A third, redundant re-check of `m_players` - genuinely dead (it was
    //  just zeroed above), but present in the shipped stream exactly this
    //  way, so reproduced rather than folded away.
    if (m_players)
        SMemFree(m_players, ".PAUCPlayer@CNetData@@", -2, 0);

    //  Tail: the base class's own destructor (its own further teardown is
    //  out of this session's scope - see the comment above).
    ((void (__thiscall*)(void*))SlotTableBaseDestructTail)(this);
}
