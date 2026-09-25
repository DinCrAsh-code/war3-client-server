//============================================================================
//  Net::CGameDescStore - 0x6F650820 and 0x6F654510.
//
//  The game description a save game carries arrives obfuscated: 0x80 bytes
//  in which every eighth byte is a *mask* byte and the seven after it carry
//  their real low bit in that mask, one bit per byte, shifted out as the run
//  is walked.  The unscrambler writes the recovered bytes into this store's
//  own 0x68-byte buffer; the decoder then reads them straight back out of it
//  field by field, which is why one object is both written and read.
//
//  `??_7CGameDescStore@Net@@6B@` (0x6F970E80) is stamped over the
//  CDataStoreScratch base 0x6F654190 has just built, which is what says the
//  two are related by inheritance.  Its eleven vtable slots are the same
//  eleven CDataStoreCache<128> has except for the four the derived class
//  overrides; only slot 7 is reached from here, and only as a hand-written
//  dispatch, because this build models the base with the four slots its own
//  reconstruction proves and not the eleven the image has.
//============================================================================
#include "cgamedescstore.h"
#include "cdatastore.h"
#include "cdatastorescratch.h"

//  0x6F970E38 - the five dwords the decoder substitutes for the 20-byte
//  block when the stream carries none.
extern "C" const unsigned int g_defaultGameDescBlock[5];

//  0x6F650770 - Net/netmsgio_650_readblock.cpp.
void __fastcall NetMsgReadBlock20(CDataStore* self, void* dest);

namespace Net
{

//----------------------------------------------------------------------------
//  vtable slot 7 (0x6F2C8410) - "start again at the front of the buffer",
//  called once before the unscrambled bytes are written.  A hand-written
//  dispatch: this build's CDataStoreScratch carries the four slots its own
//  reconstruction proves, not the eleven the shipped vtable has, so there is
//  no declaration to reach slot 7 through.
//----------------------------------------------------------------------------
typedef void(__thiscall* GameDescStoreRewindFn)(void* self);

//----------------------------------------------------------------------------
//  0x6F650820 - unscramble `src` into the store.  Returns 1 when the run
//  ended on its NUL inside 0x80 bytes, 0 when it ran to 0x80 without one or
//  produced more than the 0x68 the buffer holds.
//----------------------------------------------------------------------------
int CGameDescStore::Unscramble(const char* src)
{
    //  Both counters are zeroed *above* the rewind call: it cannot observe
    //  either, and the shipped build schedules them there.
    unsigned int index = 0;
    unsigned int written = 0;

    ((GameDescStoreRewindFn)(*(void***)this)[0x1C / 4])(this);

    unsigned char mask = 0;

    while (index < 0x80)
    {
        unsigned char byte = src[index];
        if (byte == 0)
            break;

        //  The mask-byte arm is the fall-through and the payload arm is
        //  branched to, which is the way round the shipped code tests it.
        if ((index & 7) == 0)
        {
            mask = (unsigned char)(byte >> 1);
        }
        else
        {
            //  Put the mask's low bit into the byte's low bit, then shift
            //  the mask along.
            byte ^= (unsigned char)((byte ^ mask) & 1);
            mask = (unsigned char)(mask >> 1);

            if (written >= 0x68)
                return 0;

            ((CDataStoreScratch*)this)->WriteByte(byte);
            written++;
        }

        index++;
    }

    return index < 0x80;
}

}  // namespace Net

//----------------------------------------------------------------------------
//  0x6F654510 - decode one save game's header block into a GAMEDESC.
//
//  The buffer is a separate local rather than a member of the store: the
//  shipped constructor call takes its address off esp, not off `this`.
//----------------------------------------------------------------------------
int __fastcall GameDescDecode(Net::GAMEDESC* desc, const SSaveGameHeaderBlock* block)
{
    desc->m_name = block->m_name;

    if (desc->m_name.m_text[0] == 0)
        return 0;

    char buffer[0x68];
    Net::CGameDescStore store(buffer, (void*)0x68);

    if (!store.Unscramble(block->m_long))
        return 0;

    store.m_field14 = 0;

    CDataStore* reader = (CDataStore*)&store;

    reader->ReadByte(&desc->m_playerCount);
    reader->ReadDword(&desc->m_word34);
    reader->ReadWord(&desc->m_word38);
    reader->ReadWord(&desc->m_word3A);
    reader->ReadDword(&desc->m_word3C);

    reader->ReadString(desc->m_mapPath, 0x36);
    if ((unsigned int)store.m_field14 > (unsigned int)store.m_field10)
        desc->m_mapPath[0] = 0;

    reader->ReadString(desc->m_creator, 0x10);
    if ((unsigned int)store.m_field14 > (unsigned int)store.m_field10)
        desc->m_creator[0] = 0;

    reader->ReadByte(&desc->m_byte30);

    if (store.m_field14 != store.m_field10)
        NetMsgReadBlock20(reader, desc->m_block40);
    else
        for (int i = 0; i < 5; i++)
            desc->m_block40[i] = g_defaultGameDescBlock[i];

    if (store.m_field14 != store.m_field10)
        return 0;

    desc->m_password = block->m_short;

    //  Named once: the shipped code loads it into a register and tests the
    //  register twice, where three separate reads of the field are three
    //  loads (docs/msvc-vc8-idioms.md, "Name the object once").
    unsigned int slotCount = block->m_wordB0;
    desc->m_word9C = slotCount;
    if (slotCount < 1 || slotCount > 0x10)
        return 0;

    desc->m_wordA0 = block->m_wordB4;
    return 1;
}
