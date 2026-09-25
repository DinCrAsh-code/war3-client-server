//============================================================================
//  0x6F32D440/0x6F32D7E0/0x6F32DB40/0x6F32DBA0 - CAbility's depth<=5
//  closure batch 3 (cability_depth5_closure_worklist.json), reached off
//  CAbility slot 154's "DepCheckAlias" walk (Item/ability_typedep.cpp) -
//  four more property reads off the same CFootprintTypeData registry
//  footprinttype.h's other accessors already use, all keyed the same way
//  (GetOrCreateFootprintTypeData, the debug tag never actually read).
//
//  Own translation unit for the same reason footprinttypename.cpp is:
//  the call into GetOrCreateFootprintTypeData has to stay a real call.
//============================================================================
#include "footprinttype.h"

//----------------------------------------------------------------------------
//  0x6F32D440 - "dependencyOrCount".  A plain field read, no clamp.
//----------------------------------------------------------------------------
unsigned int __fastcall GetFootprintDependencyOrCount(unsigned int id)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "dependencyOrCount");
    if (data != 0)
        return data->m_dependencyOrCount;
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F32DBA0 - "dependencyOr".  Same clamp shape as GetFootprintTypeName.
//----------------------------------------------------------------------------
unsigned int __fastcall GetFootprintDependencyOrEntry(unsigned int id,
                                                       unsigned int index)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "dependencyOr");
    if (data != 0)
    {
        unsigned int last = data->m_dependencyOrCount;
        if (last != 0)
        {
            --last;
            if (!(last < index))
                last = index;
            return data->m_dependencyOr[last];
        }
    }
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F32D7E0 - "upgradesIdsCount": one group's own id count, group index
//  clamped to the last group.
//----------------------------------------------------------------------------
unsigned int __fastcall GetFootprintUpgradeIdCount(unsigned int id,
                                                    unsigned int groupIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "upgradesIdsCount");
    if (data != 0)
    {
        unsigned int last = data->m_upgradeGroupCount;
        if (last != 0)
        {
            --last;
            if (!(last < groupIndex))
                last = groupIndex;
            return data->m_upgradeGroups[last].m_idCount;
        }
    }
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F32DB40 - "upgradesIds": one id out of one group, both indices
//  clamped to their own last entry.
//----------------------------------------------------------------------------
unsigned int __fastcall GetFootprintUpgradeId(unsigned int id,
                                              unsigned int groupIndex,
                                              unsigned int idIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "upgradesIds");
    if (data != 0)
    {
        unsigned int lastGroup = data->m_upgradeGroupCount;
        if (lastGroup != 0)
        {
            --lastGroup;
            if (!(lastGroup < groupIndex))
                lastGroup = groupIndex;

            CFootprintTypeData::SUpgradeIdGroup* group = &data->m_upgradeGroups[lastGroup];
            unsigned int idCount = group->m_idCount;
            if (idCount != 0)
            {
                unsigned int lastId = idCount - 1;
                if (!(lastId < idIndex))
                    lastId = idIndex;
                return group->m_ids[lastId];
            }
        }
    }
    return 0;
}

//----------------------------------------------------------------------------
//  0x6FAB5930 - a single fixed, permanently-empty SUpgradeIdGroup the seven
//  "get me the clamped group struct" accessors below hand back when the
//  type has no record at all (`m_field0`/`m_idCount`/`m_ids` all read as
//  zero forever - nothing in this binary ever writes it).  Not given a
//  defining declaration here, same as g_footprintTypeTable itself: it
//  already exists in the shipped data segment.
//----------------------------------------------------------------------------
extern CFootprintTypeData::SUpgradeIdGroup g_emptyFootprintGroup;  // unk_6FAB5930

//----------------------------------------------------------------------------
//  0x6F32D660/0x6F32D6A0/0x6F32D6E0/0x6F32D720/0x6F32D760/0x6F32D7A0 - the
//  six siblings of GetFootprintUpgradeIdCount above, one per dependency
//  list, all following its identical clamp-into-jagged-array shape.
//  (cunit_vtable_closure_worklist.json, slot 107 closure batch 24.)
//----------------------------------------------------------------------------
unsigned int __fastcall GetFootprintBuildIdCount(unsigned int id, unsigned int groupIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "buildsIdsCount");
    if (data != 0)
    {
        unsigned int last = data->m_buildGroupCount;
        if (last != 0)
        {
            --last;
            if (!(last < groupIndex))
                last = groupIndex;
            return data->m_buildGroups[last].m_idCount;
        }
    }
    return 0;
}

unsigned int __fastcall GetFootprintResearchIdCount(unsigned int id, unsigned int groupIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "researchesIdsCount");
    if (data != 0)
    {
        unsigned int last = data->m_researchGroupCount;
        if (last != 0)
        {
            --last;
            if (!(last < groupIndex))
                last = groupIndex;
            return data->m_researchGroups[last].m_idCount;
        }
    }
    return 0;
}

unsigned int __fastcall GetFootprintMakeItemIdCount(unsigned int id, unsigned int groupIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "makesItemIdsCount");
    if (data != 0)
    {
        unsigned int last = data->m_makeItemGroupCount;
        if (last != 0)
        {
            --last;
            if (!(last < groupIndex))
                last = groupIndex;
            return data->m_makeItemGroups[last].m_idCount;
        }
    }
    return 0;
}

unsigned int __fastcall GetFootprintSellItemIdCount(unsigned int id, unsigned int groupIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "sellsItemIdsCount");
    if (data != 0)
    {
        unsigned int last = data->m_sellItemGroupCount;
        if (last != 0)
        {
            --last;
            if (!(last < groupIndex))
                last = groupIndex;
            return data->m_sellItemGroups[last].m_idCount;
        }
    }
    return 0;
}

unsigned int __fastcall GetFootprintSellUnitIdCount(unsigned int id, unsigned int groupIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "sellsUnitIdsCount");
    if (data != 0)
    {
        unsigned int last = data->m_sellUnitGroupCount;
        if (last != 0)
        {
            --last;
            if (!(last < groupIndex))
                last = groupIndex;
            return data->m_sellUnitGroups[last].m_idCount;
        }
    }
    return 0;
}

unsigned int __fastcall GetFootprintTrainIdCount(unsigned int id, unsigned int groupIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "trainsIdsCount");
    if (data != 0)
    {
        unsigned int last = data->m_trainGroupCount;
        if (last != 0)
        {
            --last;
            if (!(last < groupIndex))
                last = groupIndex;
            return data->m_trainGroups[last].m_idCount;
        }
    }
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F32D860/0x6F32D8A0/0x6F32D8E0/0x6F32D920/0x6F32D960/0x6F32D9A0/
//  0x6F32D9E0 - "get me the clamped group struct itself" for each of the
//  seven lists.  Same clamp as the Count accessors above, but the no-record
//  fallback is the fixed empty sentinel rather than 0/null (there is no
//  scalar to fall back to - the return type is a struct pointer).
//----------------------------------------------------------------------------
CFootprintTypeData::SUpgradeIdGroup* __fastcall GetFootprintBuildGroup(unsigned int id, unsigned int groupIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "buildsIds");
    if (data != 0)
    {
        unsigned int last = data->m_buildGroupCount;
        if (last != 0)
        {
            --last;
            if (!(last < groupIndex))
                last = groupIndex;
            return &data->m_buildGroups[last];
        }
    }
    return &g_emptyFootprintGroup;
}

CFootprintTypeData::SUpgradeIdGroup* __fastcall GetFootprintResearchGroup(unsigned int id, unsigned int groupIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "researchesIds");
    if (data != 0)
    {
        unsigned int last = data->m_researchGroupCount;
        if (last != 0)
        {
            --last;
            if (!(last < groupIndex))
                last = groupIndex;
            return &data->m_researchGroups[last];
        }
    }
    return &g_emptyFootprintGroup;
}

CFootprintTypeData::SUpgradeIdGroup* __fastcall GetFootprintMakeItemGroup(unsigned int id, unsigned int groupIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "makesItemIds");
    if (data != 0)
    {
        unsigned int last = data->m_makeItemGroupCount;
        if (last != 0)
        {
            --last;
            if (!(last < groupIndex))
                last = groupIndex;
            return &data->m_makeItemGroups[last];
        }
    }
    return &g_emptyFootprintGroup;
}

CFootprintTypeData::SUpgradeIdGroup* __fastcall GetFootprintSellItemGroup(unsigned int id, unsigned int groupIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "sellsItemIds");
    if (data != 0)
    {
        unsigned int last = data->m_sellItemGroupCount;
        if (last != 0)
        {
            --last;
            if (!(last < groupIndex))
                last = groupIndex;
            return &data->m_sellItemGroups[last];
        }
    }
    return &g_emptyFootprintGroup;
}

CFootprintTypeData::SUpgradeIdGroup* __fastcall GetFootprintSellUnitGroup(unsigned int id, unsigned int groupIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "sellsUnitIds");
    if (data != 0)
    {
        unsigned int last = data->m_sellUnitGroupCount;
        if (last != 0)
        {
            --last;
            if (!(last < groupIndex))
                last = groupIndex;
            return &data->m_sellUnitGroups[last];
        }
    }
    return &g_emptyFootprintGroup;
}

CFootprintTypeData::SUpgradeIdGroup* __fastcall GetFootprintTrainGroup(unsigned int id, unsigned int groupIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "trainsIds");
    if (data != 0)
    {
        unsigned int last = data->m_trainGroupCount;
        if (last != 0)
        {
            --last;
            if (!(last < groupIndex))
                last = groupIndex;
            return &data->m_trainGroups[last];
        }
    }
    return &g_emptyFootprintGroup;
}

CFootprintTypeData::SUpgradeIdGroup* __fastcall GetFootprintUpgradeGroup(unsigned int id, unsigned int groupIndex)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "upgradesIds");
    if (data != 0)
    {
        unsigned int last = data->m_upgradeGroupCount;
        if (last != 0)
        {
            --last;
            if (!(last < groupIndex))
                last = groupIndex;
            return &data->m_upgradeGroups[last];
        }
    }
    return &g_emptyFootprintGroup;
}

//----------------------------------------------------------------------------
//  0x6F32DA20 - "revive": a single rawcode, read back raw with no clamp.
//----------------------------------------------------------------------------
unsigned int __fastcall GetFootprintReviveId(unsigned int id)
{
    //  Guard-clause-first, not the found-arm-first ternary every sibling
    //  above uses: the shipped code's not-found path is the immediate
    //  `retn` with the already-zero GetOrCreateFootprintTypeData result
    //  still in eax, and the found path is out of line after the branch.
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "revive");
    if (data == 0)
        return 0;
    return data->m_reviveId;
}
