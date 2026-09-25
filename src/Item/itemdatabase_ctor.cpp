//============================================================================
//  0x6F257370 - CItemDatabase::Construct.  Zero every field, then stamp
//  the class's own real vtable.
//
//  Stamps the shipped vtable's own literal runtime address
//  (0x6F92CC68, `tools/worktree_store.py resolve "??_7CItemDatabase@@6B@"`)
//  rather than this build's own compiler-emitted `??_7CItemDatabase@@6B@`
//  (real now that this class derives from CWar3SlkDatabase - see
//  itemdatabase.h's own header note): `Construct()` is a plain method, not
//  a real C++ constructor, so there is no implicit derived-vtable store
//  here for the compiler to emit in the first place, and the shipped
//  literal is genuinely correct regardless. `*(void**)this` reaches the
//  same first four bytes a named `m_vfptr` field would have, without
//  redeclaring the base's own (now-implicit) vtable-pointer slot.
//============================================================================
#include "itemdatabase.h"

#define ITEM_DATABASE_VTABLE ((void*)0x6F92CC68)

void __thiscall CItemDatabase::Construct()
{
    m_recordSet = 0;
    m_haveChecksum = 0;
    m_checksum = 0;
    *(void**)this = ITEM_DATABASE_VTABLE;

    m_itemID.m_wedbResult = 0;             m_itemID.m_registeredValue = 0;
    m_scriptname.m_wedbResult = 0;         m_scriptname.m_registeredValue = 0;
    m_file.m_wedbResult = 0;               m_file.m_registeredValue = 0;
    m_uses.m_wedbResult = 0;               m_uses.m_registeredValue = 0;
    m_goldcost.m_wedbResult = 0;           m_goldcost.m_registeredValue = 0;
    m_lumbercost.m_wedbResult = 0;         m_lumbercost.m_registeredValue = 0;
    m_stockMax.m_wedbResult = 0;           m_stockMax.m_registeredValue = 0;
    m_usable.m_wedbResult = 0;             m_usable.m_registeredValue = 0;
    m_perishable.m_wedbResult = 0;         m_perishable.m_registeredValue = 0;
    m_droppable.m_wedbResult = 0;          m_droppable.m_registeredValue = 0;
    m_powerup.m_wedbResult = 0;            m_powerup.m_registeredValue = 0;
    m_sellable.m_wedbResult = 0;           m_sellable.m_registeredValue = 0;
    m_pawnable.m_wedbResult = 0;           m_pawnable.m_registeredValue = 0;
    m_drop.m_wedbResult = 0;               m_drop.m_registeredValue = 0;
    m_HP.m_wedbResult = 0;                 m_HP.m_registeredValue = 0;
    m_stockRegen.m_wedbResult = 0;         m_stockRegen.m_registeredValue = 0;
    m_stockStart.m_wedbResult = 0;         m_stockStart.m_registeredValue = 0;
    m_targType.m_wedbResult = 0;           m_targType.m_registeredValue = 0;
    m_armor.m_wedbResult = 0;              m_armor.m_registeredValue = 0;
    m_prio.m_wedbResult = 0;               m_prio.m_registeredValue = 0;
    m_abilList.m_wedbResult = 0;           m_abilList.m_registeredValue = 0;
    m_Level.m_wedbResult = 0;              m_Level.m_registeredValue = 0;
    m_morph.m_wedbResult = 0;              m_morph.m_registeredValue = 0;
    m_pickRandom.m_wedbResult = 0;         m_pickRandom.m_registeredValue = 0;
    m_version.m_wedbResult = 0;            m_version.m_registeredValue = 0;
    m_oldLevel.m_wedbResult = 0;           m_oldLevel.m_registeredValue = 0;
    m_class.m_wedbResult = 0;              m_class.m_registeredValue = 0;
    m_cooldownID.m_wedbResult = 0;         m_cooldownID.m_registeredValue = 0;
    m_ignoreCD.m_wedbResult = 0;           m_ignoreCD.m_registeredValue = 0;
    m_scale.m_wedbResult = 0;              m_scale.m_registeredValue = 0;
    m_colorR.m_wedbResult = 0;             m_colorR.m_registeredValue = 0;
    m_colorG.m_wedbResult = 0;             m_colorG.m_registeredValue = 0;
    m_colorB.m_wedbResult = 0;             m_colorB.m_registeredValue = 0;
}
