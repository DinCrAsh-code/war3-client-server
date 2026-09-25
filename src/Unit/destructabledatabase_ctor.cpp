//============================================================================
//  CDestructableDatabase::Construct - the inline constructor logic inside
//  0x6F267E90 (Unit/acquiredestructabledatabase.cpp).  Same shape as
//  Game/upgradedatabase_ctor.cpp's own CUpgradeDatabase::Construct.
//
//  Stamps the shipped vtable's own literal runtime address (0x6F92C6FC,
//  `tools/worktree_store.py resolve "??_7CDestructableDatabase@@6B@"`)
//  rather than this build's own compiler-emitted `??_7CDestructableDatabase@@6B@`
//  (real now that this class derives from CWar3SlkDatabase - see
//  destructabledatabase.h's own header note) - the numeric literal is the
//  genuinely correct shipped value regardless, and `Construct()` is a
//  plain method rather than a real C++ constructor, so there is no
//  implicit derived-vtable store for the compiler to emit here in the
//  first place. `*(void**)this` reaches the same first four bytes a named
//  `m_vfptr` field would have, without redeclaring the base's own
//  (now-implicit) vtable-pointer slot.
//============================================================================
#include "destructabledatabase.h"

#define DESTRUCTABLE_DATABASE_VTABLE ((void*)0x6F92C6FC)

void __thiscall CDestructableDatabase::Construct()
{
    m_recordSet = 0;
    m_haveChecksum = 0;
    m_checksum = 0;
    *(void**)this = DESTRUCTABLE_DATABASE_VTABLE;
}
