//============================================================================
//  0x6F549790 - build the local player's end-of-game summary blob.
//
//  Everything the score screen shows for the player at the world object's
//  "acting slot" (+0x28), written into a stack CDataStore in one run and
//  then copied out as a length byte followed by that many bytes.  It is
//  what supplies the *meaning* of the whole Player/ score cluster: the
//  order the fields go into the stream is the order the score screen reads
//  them back.
//
//  Three things about the shape are worth stating, because they are
//  deliberate and look like transcription slips otherwise:
//
//    * **one stack slot does three unrelated jobs.**  The shipped frame
//      keeps the hero value, then the upgrade ratio as a `float`, then the
//      blob's own buffer pointer, all in the same four bytes.  Declaring
//      three locals allocates three slots and shifts every `[esp+N]` below
//      them, so the source has to find a dead local to reuse the way the
//      shipped build did (docs/msvc-vc8-idioms.md, "One extra never-reused
//      local shifts every later displacement").
//    * **the hero value is written raw and again divided by ten**, from
//      that same slot, with twelve stream writes between the two.
//    * **the three-word records at +0x2E4 are capped at three** and their
//      array base is re-read for each of the three words, because a call
//      sits between them and MSVC cannot prove the store did not move it.
//
//  What this build cannot reproduce is the frame: the shipped function
//  carries VC8's unified `__try` frame (a single pushed handler pointer, an
//  `fs:0` chain and a /GS cookie XORed into esp) for the CDataStore local's
//  own destructor, which this repo's fixed `/GS- /EHs-c-` never emits - see
//  docs/msvc-vc8-idioms.md, "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce".  The destructor's body is written out by
//  hand at the end because the shipped code inlines it there and only the
//  unwind funclet calls the out-of-line copy.
//============================================================================
#include "netdata.h"
#include "cdatastore.h"
#include "cdatastorescratch.h"
#include "playerscore.h"
#include "playerheroeval.h"

extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int size);

//  dword_6FAB65F4 again - the world object, viewed here only as the indexed
//  player table Misc/misc_field_getters.cpp already reconstructed the
//  accessor for.
extern void* g_unk6FAB65F4;

//  Re-declared to match Misc/misc_field_getters.cpp exactly, so the call
//  binds to that definition rather than to a second symbol.
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//  The world object's own "which slot is acting" word, at +0x28.
struct SWorldActingSlot
{
    char           m_reserved00[0x28];
    unsigned short m_actingSlot;   // +0x28
};

//  The +0x2E4 record SPlayerScore::GetRecord2E4 hands back: a three-word
//  array header whose elements are twelve bytes each.
struct SPlayerTripleArray
{
    unsigned int m_alloc;   // +0x00
    unsigned int m_count;   // +0x04
    int*         m_data;    // +0x08
};

//  The stream's own vtable slot 9: hand back the finished bytes and how
//  many there are.
typedef void (__thiscall *DataStoreFinishFn)(void* self, void** outBuffer,
                                             int* outLength, int flags);

static float AsHardwareFloat(const CFloat& value)
{
    return *(const float*)&value;
}

int __fastcall BuildGameResultBlob(unsigned char* out)
{
    void* world = g_unk6FAB65F4;
    if (world == 0)
        return 0;

    SPlayerScore* player = (SPlayerScore*)
        ((IndexedArrayHolder_6F3A1650*)world)
            ->At(((SWorldActingSlot*)world)->m_actingSlot);
    if (player == 0)
        return 0;

    CDataStore stream;
    stream.m_field4 = 0;
    stream.m_field8 = 0;
    stream.m_field0xC = 0;
    stream.m_field10 = 0;
    stream.m_readPos = -1;

    CDataStoreScratch* ds = (CDataStoreScratch*)&stream;

    ds->WriteDword(*(unsigned int*)((char*)player + 0x260));
    ds->WriteDword(*(unsigned int*)((char*)player + 0x264));
    ds->WriteDword(*(unsigned int*)((char*)player + 0x278));

    //  A published field at +0x40 turned into "is it *not* set": the
    //  shipped `xor ecx,ecx / cmp eax,ebx / setz cl` is an equality against
    //  the zero already in a register, not a `!`.
    ds->WriteDword(((HandleRefFieldOwner_6F473170*)((char*)player + 0x40))
                       ->QueryField78() == 0);

    //  One slot, three jobs - see the header note.
    int scratch = ((SPlayerHeroes*)player)->TotalHeroValue(0);

    ds->WriteDword(player->TotalTalliedCost());
    ds->WriteDword(scratch);
    ds->WriteDword(player->QueryTenth130Plus140());
    ds->WriteDword(player->TotalFlag8Count1C());
    ds->WriteDword(player->TotalFlag8Count2C());
    ds->WriteDword(player->CountFlag4WithCost());
    ds->WriteDword(player->TotalFlag4Count2C());
    ds->WriteDword(player->QueryPeak24C());

    SPlayerTripleArray* entries = (SPlayerTripleArray*)player->GetRecord2E4();
    unsigned int count = entries->m_count;
    if (count >= 3)
        count = 3;
    ds->WriteDword(count);

    if (count > 0)
    {
        unsigned int offset = 0;
        do
        {
            ds->WriteDword(*(unsigned int*)((char*)entries->m_data + offset));
            ds->WriteDword(*(unsigned int*)((char*)entries->m_data + offset + 4));
            ds->WriteDword(*(unsigned int*)((char*)entries->m_data + offset + 8));
            offset += 12;
            count--;
        }
        while (count != 0);
    }

    ds->WriteDword(player->QueryCounter250());
    ds->WriteDword(player->QueryCounter254());
    ds->WriteDword(player->QueryCounter258());
    ds->WriteDword((unsigned int)scratch / 10);
    ds->WriteDword(player->QueryTenth130());
    ds->WriteDword(player->QueryTenth140());

    int pair = player->QueryField190();
    pair += player->QueryField1A0();
    ds->WriteDword(pair);

    pair = player->QueryField1B0();
    pair += player->QueryField1C0();
    ds->WriteDword(pair);

    //  The ratio is read straight out of the buffer the by-value return
    //  left in eax and rounded into a `float` local - the shipped
    //  `fld [eax]` / `fstp [slot]` / `fld [slot]` pair - and the truncation
    //  that follows is the *unsigned* one, which is the whole
    //  fnstcw/or 0C00h/fldcw/fistp block (a signed `(int)` would have been
    //  one `jmp __ftol2_sse`).
    *(float*)&scratch = AsHardwareFloat(player->GetUpgradeRatio());
    ds->WriteDword((unsigned int)*(float*)&scratch);

    ds->WriteDword(player->QueryTenth150());

    int length;
    stream.m_readPos = 0;
    ((DataStoreFinishFn)(*(void***)&stream)[0x24 / 4])(
        &stream, (void**)&scratch, &length, 0);

    unsigned char written = (unsigned char)length;
    out[0] = written;
    memcpy(out + 1, *(void**)&scratch, written);

    //  ~CDataStore runs here.  The shipped code has its body *inlined* -
//  the vtable re-stamp and the conditional hand-back through
//  off_6F932C0C - because its definition was visible in that translation
//  unit; here it is a real call, and it has to be: writing the body out by
//  hand as well would run the release twice, which is a live double-free
//  that no score could see.  Nine instructions, deliberately given up.
    return 1;
}
