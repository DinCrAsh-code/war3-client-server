//============================================================================
//  0x6F53F160 - IsGameModeOne.
//
//  Own translation unit: both of its callers
//  (CWidget::IsVisibleToLocalPlayer, widget_visibleflags.cpp, and
//  CWidget::RefreshSpriteVisibility, widget_slot61.cpp) reach it with a
//  real `call`, and five instructions is well inside what /Ob2 would
//  inline given the chance.
//
//  Slot 13 of the thread-local table is the same slot GetConfigFloat
//  (configfloat.cpp) reads the game data context out of, so this is that
//  context's cache (+0x10) one pointer further in, and a mode word 0x610
//  bytes into whatever that names.  Only the three offsets this function
//  walks are declared - nothing here claims to know what the block is.
//============================================================================
#include "gamecontext.h"

struct SGameModeBlock
{
    char         m_reserved00[0x610];
    unsigned int m_mode;    // +0x610
};

struct SGameDataCacheHead
{
    char             m_reserved00[8];
    SGameModeBlock*  m_pModeBlock;   // +0x08
};

int __fastcall IsGameModeOne()
{
    CGameDataContext* context =
        (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    SGameDataCacheHead* cache = (SGameDataCacheHead*)context->m_pCache;
    return cache->m_pModeBlock->m_mode == 1;
}
