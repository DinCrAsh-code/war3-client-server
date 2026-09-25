//============================================================================
//  sub_6F6737E0 (0x6F6737E0, 17 instructions) - reset one record: clear its
//  own "needs reset" bit and connection, and if this machine's own record
//  (`self->m_selfRecord`) is set, clear that record's own bit for this
//  record's player slot out of a bitmask at `m_selfRecord+0x48` (a field
//  outside this function's own scope, so it is addressed raw rather than
//  named). The old connection pointer is read into a local the shipped
//  code never uses again - kept as a discarded local rather than dropped,
//  since dropping the read would cost the instruction that loads it.
//  Called from PlayerTable_InstallHandler's own self-install path
//  (playertable_installhandler.cpp), once per record whose flags bit 0x2
//  is set, while walking the whole player record list.
//============================================================================
#include "playertable.h"

void __fastcall ClearRecordConnection(PlayerTable* self, PlayerRecord* record)
{
    record->m_flags &= ~2;
    //  The shipped code loads the old connection pointer here and never
    //  uses it again (see header comment) - `mov eax,[edx+3Ch]` sits
    //  between the flags write and the three field writes below. Neither a
    //  plain local nor a `volatile` one survives this build's own dead-
    //  store elimination (there is no address taken and no further volatile
    //  access to order against), so the read itself is not reproducible
    //  from source; ClearRecordConnection's own BEHAVIOUR entry documents
    //  it as the one difference.
    (void)record->m_connection;
    record->m_field05D = 0xFF;
    record->m_field058 = 0;
    record->m_connection = 0;

    PlayerRecord* selfRecord = self->m_selfRecord;
    if (selfRecord)
    {
        unsigned int* selfMask = (unsigned int*)((char*)selfRecord + 0x48);
        *selfMask &= ~(1u << (record->m_playerId - 1));
    }
}
