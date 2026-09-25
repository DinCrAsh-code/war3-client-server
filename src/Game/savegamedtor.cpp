//============================================================================
//  0x6F34D9A0 - CSaveGame::~CSaveGame.  The three strings in reverse order,
//  then the slot array's block, then the base.
//
//  The slot release is written out rather than left to TSFixedArray, because
//  the shipped code frees the block directly under the element's own RTTI
//  descriptor and never touches the count.
//============================================================================
#include "savegame.h"

//  0x6F535DC0 - the base's own teardown.
void __fastcall SaveGameBaseDestruct(void* self);

//  0x6F4C48F0 - already reconstructed as RCString::~RCString
//  (src/Misc/rcstringdtor.cpp); STStringField is the same object seen
//  through jassnatives.h's view, so the destructor is called through a cast
//  rather than being given a second name for one shipped address.
#include "rcstring.h"

void __fastcall SaveGameDestruct(CSaveGame* self)
{
    ((RCString*)&self->m_string424)->~RCString();
    ((RCString*)&self->m_string418)->~RCString();
    ((RCString*)&self->m_mapPath)->~RCString();

    if (self->m_slots.m_array.m_data != 0)
        SMemFree(self->m_slots.m_array.m_data,
                 ".?AUSLOT@GAMESETUP_WAR3_0003@Net@@", -2, 0);

    SaveGameBaseDestruct(self);
}
