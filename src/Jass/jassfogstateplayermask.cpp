//============================================================================
//  0x6F408110 - one of the "fog-state get-or-create wrapper" siblings the
//  checksum-provider-registry investigation found calling
//  AcquireGameSingleton(6, 0) - reached from JASS_SetFogStateRadius's own
//  callers (0x6F3C1A30/0x6F3C1AB0/0x6F3C1B20, not yet reconstructed).
//
//  `retn 8`: two stack arguments, a player slot and a flag, plus two
//  register arguments (ecx/edx) that the shipped body never reads at all -
//  confirmed from the disassembly, not assumed from the worklist's own
//  note.  The first instruction reads the CGameWar3 singleton straight out
//  of the global cache slot, with no dependency on ecx or edx; the `xor
//  edx, edx` right before the AcquireGameSingleton(6, 0) call clobbers
//  whatever the caller passed in edx unconditionally.  Every caller this
//  session found passes the player-table's own +0x34 sub-object
//  (`table->m_field34`) in ecx - the same field CGameWar3::ComputeChecksum
//  and CWorldFrameWar3::LatchLocalPlayer both already read - so ecx is
//  live data the caller computed for a *different* sibling in the same
//  family, not something this particular wrapper happens to need.
//
//  If `useRecordMask` is set, returns the player record's own +0x2E0 word
//  (the same "table's own +0x2E0 word" mask Unit/unit_ownerworldmask.cpp
//  already names m_mask2E0); otherwise returns `1 << slot`, the slot's own
//  bit, without touching the record at all.
//============================================================================
extern void* g_unk6FAB65F4;

//  0x6F0074F0 - see GameUI/acquiregamesingleton.cpp.
void* __fastcall AcquireGameSingleton(int index, int unused);

//  Local view, same shape as Unit/unit_ownerworldmask.cpp's own
//  SPlayerWorldMaskRecord - a separate declaration per the repo's own
//  convention (every caller of IndexedArrayHolder_6F3A1650::At declares
//  its own narrow view rather than share one header).
struct IndexedArrayHolder_6F3A1650
{
    int At(unsigned int index);
};

struct SPlayerWorldMaskRecord
{
    char           m_reserved000[0x2E0];
    unsigned short m_mask2E0;   // +0x2E0
};

unsigned short __fastcall FogStatePlayerMaskOrRecordMask(
    void* /*unused_widgetRecord*/, int /*unused*/,
    int slot, int useRecordMask)
{
    void* table = g_unk6FAB65F4;
    if (!table)
    {
        table = AcquireGameSingleton(6, 0);
        g_unk6FAB65F4 = table;
    }

    SPlayerWorldMaskRecord* record =
        (SPlayerWorldMaskRecord*)((IndexedArrayHolder_6F3A1650*)table)->At(slot);

    if (useRecordMask)
        return record->m_mask2E0;

    return (unsigned short)(1 << slot);
}
