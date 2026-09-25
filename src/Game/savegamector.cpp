//============================================================================
//  0x6F35D4C0 - CSaveGame::CSaveGame, written as a plain function for the
//  reason OsNet's own two constructors are: the three STStringFields it
//  builds are constructed through a real call each (0x6F011300), and this
//  build's /EHs-c- has no unwind funclets to put behind them.
//
//  The four bytes cleared at +0x348, +0x368, +0x39C and +0x3D2 are the first
//  byte of each of the four strings inside the copied header block.
//============================================================================
#include "savegame.h"
#include "rcstring.h"

#include "gamedatawriter.h"    // SGameDataBase::Construct, 0x6F534CE0


CSaveGame* __fastcall SaveGameConstruct(CSaveGame* self)
{
    ((SGameDataBase*)self)->Construct(0xFFFF, 1);

    self->m_vtable = (void*)g_vftableCSaveGame;

    self->m_header.m_name.m_text[0] = 0;        // +0x348
    self->m_header.m_password.m_text[0] = 0;    // +0x368
    self->m_header.m_mapPath[0] = 0;            // +0x39C
    self->m_header.m_creator[0] = 0;            // +0x3D2

    self->m_slots.m_array.m_alloc = 0;
    self->m_slots.m_array.m_count = 0;
    self->m_slots.m_array.m_data = 0;
    self->m_valid = 0;

    ((RCString*)&self->m_mapPath)->Construct(0);
    ((RCString*)&self->m_string418)->Construct(0);
    ((RCString*)&self->m_string424)->Construct(0);

    return self;
}
