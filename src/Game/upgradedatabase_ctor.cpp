//============================================================================
//  CUpgradeDatabase::Construct - the inline constructor logic inside
//  0x6F25F0B0 (Game/acquireupgradedatabase.cpp).  See upgradedatabase.h's
//  own note on why the SLK field-pair bytes are left untouched here.
//
//  Stamps the shipped vtable's own literal runtime address (0x6F92C70C,
//  `tools/worktree_store.py resolve "??_7CUpgradeDatabase@@6B@"`) rather
//  than a C++-constructor-emitted `??_7CUpgradeDatabase@@6B@` of our own -
//  `Construct()` is not a real C++ constructor (see upgradedatabase.h's
//  own header note, and Item/abilitydatabase.h's identical reasoning for
//  its own hand-written lazy-init stamp), so there is no real constructor
//  call site for the compiler to retarget at our own vtable even though
//  the class has real virtuals as of 2026-09-13.
//============================================================================
#include "upgradedatabase.h"

#define UPGRADE_DATABASE_VTABLE ((void*)0x6F92C70C)

void __thiscall CUpgradeDatabase::Construct()
{
    m_recordSet = 0;
    m_haveChecksum = 0;
    m_checksum = 0;
    *(void**)this = UPGRADE_DATABASE_VTABLE;   // +0x00, the implicit CWar3SlkDatabase vfptr
}
