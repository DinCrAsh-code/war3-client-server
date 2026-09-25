//============================================================================
//  0x6F4036B0/0x6F4038D0/0x6F4039C0/0x6F40B310/0x6F40B350/0x6F40B490/
//  0x6F40FB70 - CAbility slot 154's own "DepCheckAlias" walk
//  (asm/sub_6F052C30_0x6F052C30_calltree_asm.md), cability_depth5_closure_
//  worklist.json batch 3.  0x6F3F7110/0x6F4019F0 (ability_typedep.h) are
//  the same TSHashTable<T,K> template every other instantiation in this
//  binary shares, this time for a *non*-virtual node.
//
//  Everything from MeetsDependencyRequirement down takes `this` = the
//  current player's dependency-owner record: the same per-player object
//  UnitVisibilityMaskOwner_6F2834E0 (Unit/unit_visibilitymask.cpp) hands
//  back for the ability's owning unit.  Its own layout is not recovered
//  beyond the one pointer this closure reads at +0x2D0 - a minimal
//  stand-in class, the same reasoning ability_slots.cpp's own
//  CAbilityLevelRowRaw note gives.  Every function below is a genuine
//  member of one of these two minimal stand-ins rather than a free
//  __fastcall, so `this` lands in ecx through ordinary thiscall rules
//  instead of a hand-forced calling convention that MSVC will not attach
//  to a non-member function.
//============================================================================
#include "ability_typedep.h"
#include "ability_typefieldrecord.h"
#include "footprinttype.h"

//  FindOrCreateRecord (0x6F4036B0) is ability_typedepfind.cpp's own -
//  its own translation unit, so the real calls below stay real calls
//  rather than getting inlined away.  See that file's header comment.

class SPlayerDependencyOwner
{
public:
    //  0x6F40B310 - does the player meet a dependency requirement of
    //  `kind` for typeId, against `threshold`?
    int MeetsRequirement(unsigned int typeId, int kind, int threshold);
    //  0x6F40B350 - typeId's own dependency-alias threshold.
    int GetFallbackThreshold(unsigned int typeId);
    //  0x6F40B490 - typeId's own +0x18 flag bit 0.
    int GetAliasFlag18Bit0(unsigned int typeId);
    //  0x6F40FB70 - the group walk itself.
    void CheckDependencyAliasGroup(unsigned int typeId, int groupIndex,
                                   int* outFlag, int gate);

    char                    m_reserved00[0x2D0];
    CDependencyAliasTable*  m_dependencyTable;   // 0x2D0
};

//----------------------------------------------------------------------------
//  0x6F4C3F80 - below this closure's depth; declared, not reconstructed.
//  __fastcall(typeId in ecx, out-buffer in edx), one stack arg, `retn 4`.
//----------------------------------------------------------------------------
void __fastcall WarmDependencyLevelFieldCache(unsigned int typeId,
                                              void* outBuffer, int selector);
__declspec(naked) void __fastcall WarmDependencyLevelFieldCache(unsigned int, void*, int)
{
    __asm { mov eax, 0x6F4C3F80 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F4038D0 - how many of `kind`'s dependency fields typeId itself (and
//  every "dependencyOr"/upgrade-group alias it names, walked recursively)
//  together contribute.  `kind` is a bitset read one byte: bit 0 picks
//  the record's own +0x1C field, bit 4 (only when bit 0 is clear) +0x28
//  instead, bit 1 adds +0x20, bit 2 adds +0x24 - four independent fields
//  of the same per-type record, summed by whichever bits the caller set.
//  Bit 5 skips the "dependencyOr" alias walk; bit 3 recurses the
//  "upgradesIds" group-0 list instead, with bit 1 forced on for that
//  recursive call.
//
//  The shipped body carries a `/GS`-shaped stack cookie around all of
//  this; omitted here the same way the rest of this repo's `/GS-` build
//  omits it elsewhere - this call tree never observes it.
//----------------------------------------------------------------------------
int CDependencyAliasTableOwner::CountRequirement(unsigned int typeId, int kind)
{
    int total = 0;

    CDependencyAliasRecord* record = FindOrCreateRecord(typeId, 0);
    if (record != 0)
    {
        if (kind & 1)
            total = record->m_field1C;
        else if (kind & 0x10)
            total = record->m_field28;

        if (kind & 2)
            total += record->m_field20;
        if (kind & 4)
            total += record->m_field24;
    }

    //  Side effect only - warms whatever WarmDependencyLevelFieldCache's
    //  own out-buffer caches for typeId; nothing here reads it back.
    int levelField;
    WarmDependencyLevelFieldCache(typeId, &levelField, 5);

    if (!(kind & 0x20))
    {
        unsigned int orCount = GetFootprintDependencyOrCount(typeId);
        for (unsigned int i = orCount; i != 0; --i)
        {
            unsigned int aliasType = GetFootprintDependencyOrEntry(typeId, i - 1);
            total += CountRequirement(aliasType, kind);
        }
    }

    if (kind & 8)
    {
        unsigned int idCount = GetFootprintUpgradeIdCount(typeId, 0);
        for (unsigned int i = idCount; i != 0; --i)
        {
            unsigned int upgradeType = GetFootprintUpgradeId(typeId, 0, i - 1);
            total += CountRequirement(upgradeType, kind | 2);
        }
    }

    return total;
}

//----------------------------------------------------------------------------
//  0x6F4039C0 - typeId's own +0x18 threshold, or "no cap" (INT_MAX) when
//  it has no dependency-alias record at all.
//----------------------------------------------------------------------------
int CDependencyAliasTableOwner::GetThreshold(unsigned int typeId)
{
    CDependencyAliasRecord* record = FindOrCreateRecord(typeId, 0);
    if (record == 0)
        return 0x7FFFFFFF;
    return record->m_field18;
}

//----------------------------------------------------------------------------
//  0x6F403600 - below this closure's depth (no dump reached it, and a
//  live ida_query for it did not come back in this session); declared,
//  not reconstructed.  Same FindOrCreate push shape (0, typeId) every
//  sibling accessor in this file has, so `retn 8` follows that evidence
//  rather than a blind guess, but is unconfirmed against the shipped
//  bytes directly.
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//  0x6F40B310 - does the player meet a dependency requirement of `kind`
//  for typeId, against `threshold`?  Forwards `this` to the real table
//  at +0x2D0.
//----------------------------------------------------------------------------
int SPlayerDependencyOwner::MeetsRequirement(unsigned int typeId, int kind,
                                             int threshold)
{
    return (unsigned int)m_dependencyTable->CountRequirement(typeId, kind) >= (unsigned int)threshold;
}

//----------------------------------------------------------------------------
//  0x6F40B350 - typeId's own dependency-alias threshold, forwarded
//  through +0x2D0 the same way MeetsRequirement is.  Written as a naked
//  tail-jump into GetThreshold's own compiled body rather than a plain
//  call: GetThreshold is a two-line function /Ob2 inlines into any
//  ordinary caller in the same TU, which loses the real, out-of-line
//  `jmp` the shipped code has - the same reasoning
//  Net/netclientgame0_deleteselfadjustor.cpp's own adjustor thunk gives.
//----------------------------------------------------------------------------
__declspec(naked) int SPlayerDependencyOwner::GetFallbackThreshold(unsigned int)
{
    __asm { mov ecx, [ecx+2D0h] }
    __asm { jmp CDependencyAliasTableOwner::GetThreshold }
}

//----------------------------------------------------------------------------
//  0x6F40B490 - typeId's own +0x18 flag bit 0, forwarded through +0x2D0
//  the same way; true when there is no record at all.
//----------------------------------------------------------------------------
int SPlayerDependencyOwner::GetAliasFlag18Bit0(unsigned int typeId)
{
    CDependencyAliasRecord* record = m_dependencyTable->LookupFlag18Record(typeId, 0);
    if (record != 0)
        return record->m_field18 & 1;
    return 1;
}

__declspec(naked) CDependencyAliasRecord*
    CDependencyAliasTableOwner::LookupFlag18Record(unsigned int, int)
{
    __asm { mov eax, 0x6F403600 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F40FB70 - CAbility's own slot 154, the group walk itself.  When
//  `gate` is nonzero, look typeId's own ability-dependency record up
//  ("HasDepAbil" - LookupAbilityFieldRecord, ability_typefieldrecord.cpp,
//  a *different* per-type registry from CDependencyAliasTable above) and
//  walk two independent (required-list, optional-threshold-list) pairs
//  off it - +0x1DC/+0x1E0 paired with +0x1F4/+0x1F8 at `kind` 0x18, then
//  +0x20C/+0x210 paired with +0x224/+0x228 at `kind` 9 - testing every
//  required type id against MeetsRequirement with its own paired
//  threshold (defaulting to 1 when the optional list has nothing for
//  that index), falling back to GetFallbackThreshold only when the
//  direct check fails.  `*outFlag` answers 0 once any required entry
//  fails both; every entry in both lists is still walked.
//----------------------------------------------------------------------------
void SPlayerDependencyOwner::CheckDependencyAliasGroup(unsigned int typeId,
                                                        int groupIndex,
                                                        int* outFlag, int gate)
{
    int result = 1;

    if (gate != 0)
    {
        SAbilityFieldRecordNode* record =
            (SAbilityFieldRecordNode*)LookupAbilityFieldRecord(typeId, "HasDepAbil");
        if (record != 0)
        {
            //  Both blocks below are the identical shape written out
            //  twice, not a shared loop, because the shipped code does:
            //  same field pattern, different (kind, field offset) pair.
            if ((unsigned int)groupIndex < record->m_requiredCount)
            {
                unsigned int idCount = record->m_required[groupIndex].m_count;
                unsigned int* ids = record->m_required[groupIndex].m_data;

                unsigned int* thresholds = 0;
                unsigned int thresholdCount = 0;
                if ((unsigned int)groupIndex < record->m_optionalCount)
                {
                    thresholds = record->m_optional[groupIndex].m_data;
                    thresholdCount = record->m_optional[groupIndex].m_count;
                }

                for (unsigned int i = 0; i < idCount; ++i)
                {
                    unsigned int reqTypeId = ids[i];
                    int threshold = (i < thresholdCount) ? (int)thresholds[i] : 1;

                    if (!MeetsRequirement(reqTypeId, 0x18, threshold))
                    {
                        int fallback = GetFallbackThreshold(reqTypeId);
                        if (threshold > fallback)
                            result = 0;
                    }
                }
            }

            if ((unsigned int)groupIndex < record->m_required2Count)
            {
                unsigned int idCount = record->m_required2[groupIndex].m_count;
                unsigned int* ids = record->m_required2[groupIndex].m_data;

                unsigned int* thresholds = 0;
                unsigned int thresholdCount = 0;
                if ((unsigned int)groupIndex < record->m_optional2Count)
                {
                    thresholds = record->m_optional2[groupIndex].m_data;
                    thresholdCount = record->m_optional2[groupIndex].m_count;
                }

                for (unsigned int i = 0; i < idCount; ++i)
                {
                    unsigned int reqTypeId = ids[i];
                    int threshold = (i < thresholdCount) ? (int)thresholds[i] : 1;

                    if (!MeetsRequirement(reqTypeId, 9, threshold))
                    {
                        int fallback = GetFallbackThreshold(reqTypeId);
                        if (threshold > fallback)
                            result = 0;
                    }
                }
            }
        }
    }

    if (outFlag != 0)
        *outFlag = result;
}
