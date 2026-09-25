//============================================================================
//  0x6F2B01A0 / 0x6F2B00C0 - the two questions the player score's upgrade
//  ratio asks the upgrade data table (Player/playerroster.cpp).
//
//  0x6F2B01A0 is one forwarding call; 0x6F2B00C0 does the work.  Neither
//  calls the other, so they share a translation unit - and both have to
//  reach the table's own members with real calls, which is why every one of
//  those lives elsewhere.
//
//  The row handle is released on both the failure and the success path, and
//  the shipped code re-tests it for null each time even though `AcquireRow`
//  has just been checked - that is two separate `if (row)` guards in the
//  source, not one.
//
//  0x6F2B00C0's shipped prologue carries a /GS buffer cookie for its
//  five-byte key buffer; this repo's fixed /GS- cannot emit it.
//
//  0x6F2B0000 (CAbility depth<=5 closure, reached through the same
//  worklist-slot154 tree as 0x6F2B00C0/0x6F2B01A0 above) is the same
//  row/HasRow/AcquireRow/QueryColumn78/RowTableLookup opening as
//  UpgradeAffectsType, minus the race check: it answers the row's own
//  "selected" flag (QueryColumn38) directly rather than gating it against a
//  caller-supplied race, so it never reads QueryColumn18 or calls
//  RaceNameToId at all.  Same GS-cookie caveat as 0x6F2B00C0 - this one's
//  own five-byte key buffer.
//============================================================================
#include "objectdatatable.h"

int __fastcall UpgradeAffectedTypeCount(unsigned int race)
{
    return GetUpgradeDataTable()->CountForRace((int)race);
}

int __fastcall UpgradeAffectsType(unsigned int typeId, unsigned int race)
{
    CObjectDataTable* table = GetUpgradeDataTable();

    char key[5];
    UnpackFourCCString(typeId, key, 5);

    if (table->HasRow(key) == 0)
        return 0;

    void* row = table->AcquireRow(key);

    if (RowTableLookup(table->QueryColumn78(row, (int)typeId)) == 0)
    {
        if (row != 0)
            ReleaseForward_6F4C8870((TRefCnt*)row);
        return 0;
    }

    int selected = table->QueryColumn38(row, (int)typeId);
    int rowRace = RaceNameToId(table->QueryColumn18(row, (int)typeId));

    if (row != 0)
        ReleaseForward_6F4C8870((TRefCnt*)row);

    if (selected == 0)
        return 0;
    if ((int)race != rowRace)
        return 0;

    return 1;
}

//----------------------------------------------------------------------------
//  0x6F2B0000 - see the header note above.
//----------------------------------------------------------------------------
int __fastcall UpgradeSelectedForType(unsigned int typeId)
{
    CObjectDataTable* table = GetUpgradeDataTable();

    char key[5];
    UnpackFourCCString(typeId, key, 5);

    if (table->HasRow(key) == 0)
        return 0;

    void* row = table->AcquireRow(key);

    if (RowTableLookup(table->QueryColumn78(row, (int)typeId)) == 0)
    {
        if (row != 0)
            ReleaseForward_6F4C8870((TRefCnt*)row);
        return 0;
    }

    int selected = table->QueryColumn38(row, (int)typeId);

    if (row != 0)
        ReleaseForward_6F4C8870((TRefCnt*)row);

    return selected;
}
