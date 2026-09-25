//============================================================================
//  0x6F443720 - read the save game's slot record off the stream: a two-byte
//  length, then that many bytes.  A length past the 0x80 the record holds is
//  not an error return - it marks the stream overrun the same way every
//  other reader here does (m_readPos one past m_field10) and falls into the
//  shared "did the stream overrun" test at the bottom, which is what
//  actually zeroes the record.
//
//  `this` is the CSaveGame itself: the save game *is* the data store its
//  own header is read out of (savegame.h).
//============================================================================
#include "savegame.h"
#include "cdatastore.h"

CDataStore* __fastcall SaveGameReadSlotRecord(CDataStore* store,
                                              Net::GAMESETUP_SLOTRECORD* out)
{
    //  A dword slot with only its low half read: the shipped code stores
    //  four bytes of zero into it and then only ever reads `ax` back.
    unsigned int length = 0;
    store->ReadWord((unsigned short*)&length);

    if ((unsigned short)length <= 0x80)
    {
        store->ReadRawAlias(out->m_data, (unsigned short)length);
        out->m_length = (unsigned short)length;
    }
    else
    {
        store->m_readPos = store->m_field10 + 1;
    }

    if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
        out->m_length = 0;

    return store;
}
